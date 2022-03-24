#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>


void handle_request(int client_sock, ssize_t readret, char* recv_buf);
void response400(int client_sock);
void response501(int client_sock);