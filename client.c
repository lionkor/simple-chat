#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <time.h>

#include "common.h"

#ifndef ADDRESS
#define ADDRESS "127.0.0.1"
#endif

// Driver code
int main() {
    int                ret = 0;
    int                conn_fd;
    struct sockaddr_in server_addr = { 0 };

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);

    ret = inet_pton(AF_INET, ADDRESS, &server_addr.sin_addr);
    if (ret != 1) {
        if (ret == -1) {
            perror("inet_pton");
        }
        fprintf(stderr,
            "failed to convert address %s "
            "to binary net address\n",
            ADDRESS);
        return -1;
    }

    fprintf(stdout, "CONNECTING: address=%s port=%d\n", ADDRESS, PORT);

    conn_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_fd == -1) {
        perror("socket");
        return -1;
    }

    ret = connect(conn_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (ret == -1) {
        perror("connect");
        return -1;
    }

    // READ / WRITE HERE
    String* msg      = string_create_empty(MAXLINE);
    String* response = string_create_empty(MAXLINE);
    for (;;) {
        string_readline(msg);
        send(conn_fd, msg->chars, msg->len, 0);
        printf("[%10lu] sent    : %s\n", clock(), msg->chars);
        read(conn_fd, response->chars, response->len);
        printf("[%10lu] received: %s\n", clock(), response->chars);
        // clear response buffer
        string_clear(msg);
        string_clear(response);
    }
    string_free(msg);
    string_free(response);

    ret = shutdown(conn_fd, SHUT_RDWR);
    if (ret == -1) {
        perror("shutdown");
        return -1;
    }

    ret = close(conn_fd);
    if (ret == -1) {
        perror("close");
        return -1;
    }

    return 0;
}

