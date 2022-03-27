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

#define RESPONSE_200 " 200 OK\r\n"
#define RESPONSE_400 " 400 Bad request\r\n\r\n"
#define RESPONSE_404 " 404 Not Found\r\n\r\n"
#define RESPONSE_501 " 501 Not Implemented\r\n\r\n"
#define RESPONSE_505 " 505 HTTP Version not supported\r\n\r\n"

#define HTTP_VERSION "HTTP/1.1"
#define BUF_SIZE 8192


void handle_request(int client_sock, ssize_t readret, char* recv_buf) {
    Request *request = parse(recv_buf, readret, client_sock);
    char res_buf[BUF_SIZE];
    
    memset(res_buf, 0, BUF_SIZE);

    if (request == NULL) {
        add_res_statu_line(client_sock, res_buf, RESPONSE_400);
        send(client_sock, res_buf, strlen(res_buf), 0);
        return;
    }

    if(strcmp(request->http_version, HTTP_VERSION) != 0) {
        add_res_statu_line(client_sock, res_buf, RESPONSE_505);
        send(client_sock, res_buf, strlen(res_buf), 0);
        return;
    }

    if (!strcmp(request->http_method, "GET")) {
        handle_get(client_sock, request, res_buf);
    } else if (!strcmp(request->http_method, "HEAD")) {
        handle_head(client_sock, request, res_buf);
    } else if (!strcmp(request->http_method, "POST")) {
        send(client_sock, recv_buf, readret, 0);
        return;
    } else {
        add_res_statu_line(client_sock, res_buf, RESPONSE_501);
    }

    send(client_sock, res_buf, strlen(res_buf), 0);
    free(request->headers);
    free(request);
}

void handle_head(int client_sock, Request* request, char* res_buf) {
    char path[1024];
    strcpy(path, "static_site");
    if(strcmp(request->http_uri, "/") == 0){
        strcat(path, "/index.html");
    } else {
        add_res_statu_line(client_sock, res_buf, RESPONSE_404);
        return;
    };

    FILE *fd = fopen(path, "rb");
    if(fd == NULL) {
        add_res_statu_line(client_sock, res_buf, RESPONSE_404);
        return;
    }
    
    struct stat buf;
    stat(path, &buf);
    int file_size = buf.st_size;
    char file_size_header[1024];
    sprintf(file_size_header, "Content-Lenght: %d\r\n", file_size);

    add_res_statu_line(client_sock, res_buf, RESPONSE_200);
    
    strcat(res_buf, "Content-Type: text/html\r\n");
    strcat(res_buf, file_size_header);
}

void handle_get(int client_sock, Request* request, char* res_buf) {
    char path[1024];
    strcpy(path, "static_site");
    if(strcmp(request->http_uri, "/") == 0){
        strcat(path, "/index.html");
    } else {
        add_res_statu_line(client_sock, res_buf, RESPONSE_404);
        return;
    };

    FILE *fd = fopen(path, "rb");
    struct stat buf;
    stat(path, &buf);
    if(fd == NULL) {
        add_res_statu_line(client_sock, res_buf, RESPONSE_404);
        return;
    }

    int file_size = buf.st_size;
    char file_size_header[1024];
    sprintf(file_size_header, "Content-Lenght: %d\r\n", file_size);

    char file[BUF_SIZE];
    fread(file, 1, file_size, fd);

    add_res_statu_line(client_sock, res_buf, RESPONSE_200);
    
    strcat(res_buf, "Content-Type: text/html\r\n");
    strcat(res_buf, file_size_header);

    strcat(res_buf, "\r\n");
    strcat(res_buf, file);
}

void add_res_statu_line(int client_sock, char* res_buf, char* res_status) {
    strcat(res_buf, HTTP_VERSION);
    strcat(res_buf, res_status);
}