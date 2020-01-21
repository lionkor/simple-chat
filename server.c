#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common.h"

typedef struct server {
    int listen_fd;
} server_t;


int server_listen(server_t* server);
int server_accept(server_t* server);

int main() {
    int      err    = 0;
    server_t server = { 0 };
    err             = server_listen(&server);
    if (err) {
        printf("failed to listen");
        return err;
    }

    for (;;) {
        err = server_accept(&server);
        if (err) {
            printf("failed accepting connection\n");
            return err;
        }
    }
    return 0;
}

int server_listen(server_t* server) {
    int err = 0;
    err     = (server->listen_fd = socket(AF_INET, SOCK_STREAM, 0));
    if (err == -1) {
        perror("socket");
        printf("Failed to create socket endpoint\n");
        return err;
    }

    struct sockaddr_in server_addr = { 0 };

    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(PORT);

    err = bind(server->listen_fd, (struct sockaddr*)&server_addr,
        sizeof(server_addr));
    if (err == -1) {
        perror("bind");
        printf("binding failed\n");
        return err;
    }

    err = listen(server->listen_fd, BACKLOG);
    if (err == -1) {
        perror("listen");
        printf("failed to listen\n");
        return err;
    }

    return 0;
}

int server_accept(server_t* server) {
    int                err = 0;
    int                conn_fd;
    socklen_t          client_len;
    struct sockaddr_in client_addr;
    client_len = sizeof(client_addr);

    err = (conn_fd = accept(server->listen_fd,
               (struct sockaddr*)&client_addr, &client_len));
    if (err == -1) {
        perror("accept");
        printf("failed accepting connection\n");
        return err;
    }


    printf("client connected\n");

    err = close(conn_fd);
    if (err == -1) {
        perror("close");
        printf("failed to close\n");
        return err;
    }

    return err;
}
