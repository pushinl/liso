#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

#include "parse.h"


void handle_request(int client_sock, ssize_t readret, char* recv_buf);
void handle_get(int client_sock, Request* request, char* res_buf);
void handle_head(int client_sock, Request* request, char* res_buf);
void add_res_statu_line(int client_sock, char* res_buf, char* res_status);
int get_file_size(char* file_name);