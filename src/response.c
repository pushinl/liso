#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/socket.h>

#include "response.h"
#include "parse.h"

#define http_version "HTTP/1.1"

void handle_request(int client_sock, ssize_t readret, char* recv_buf) {
    Request *request = parse(recv_buf, readret, client_sock);

    if (request == NULL) {
        response400(client_sock);
        return;
    }
    if (!strcmp(request->http_method, "GET")) {
        send(client_sock, recv_buf, readret, 0);
    } else if (!strcmp(request->http_method, "HEAD")) {
        send(client_sock, recv_buf, readret, 0);
    } else if (!strcmp(request->http_method, "POST")) {
        send(client_sock, recv_buf, readret, 0);
    } else {
        response501(client_sock);
    }

    free(request->headers);
    free(request);
}

void response400(int client_sock) {
    char response[1024];
    strcat(response, http_version);
    strcat(response, " 400 Bad request\r\n\r\n");
    send(client_sock, response, strlen(response), 0);
}

void response501(int client_sock) {
    char response[1024];
    strcat(response, http_version);
    strcat(response, " 501 Not Implemented\r\n\r\n");
    send(client_sock, response, strlen(response), 0);
}