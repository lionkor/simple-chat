#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#include "common.h"

typedef struct server {
    int listen_fd;
} server_t;

static server_t server = { 0 };

static Vector* conn_vec;

int send_message(int fd, String* msg);

void sig_handler(int signo) {
    if (signo == SIGINT) {
        printf("received SIGINT\n");
        shutdown(server.listen_fd, SHUT_RDWR);
        for (size_t i = 0; i < conn_vec->size; ++i) {
            pthread_t* t = vec_at(conn_vec, i);
            printf("joining pthread %p\n", t);
            pthread_join(*t, NULL);
        }
        exit(0);
    }
}

int server_listen(server_t* server);
int server_accept(server_t* server);

typedef struct connection_info {
    int fd;
} connection_info_t;

void pong(void* data) {
    connection_info_t* conn = data;
    String*            msg  = string_create("pong");
    send_message(conn->fd, msg);
    string_free(msg);
}

int main() {
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
    int err = 0;
    err     = server_listen(&server);
    if (err) {
        printf("failed to listen");
        return err;
    }

    conn_vec = vec_create_size(2, sizeof(pthread_t));

    for (;;) {
        err = server_accept(&server);
        if (err) {
            printf("failed accepting connection\n");
            return err;
        }
    }

    vec_free(conn_vec);

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

int send_message(int fd, String* msg) {
    send(fd, msg->chars, msg->len, MSG_NOSIGNAL);
    printf("[%10lu] sent    : %s\n", clock(), msg->chars);
    return 0;
}

void* connection_thread(void* data) {
    connection_info_t* conn = data;
    printf("client connected\n");

    String* buf = string_create_empty(MAXLINE);
    while (send(conn->fd, NULL, 0, MSG_NOSIGNAL) != -1) {
        read(conn->fd, buf->chars, buf->len);
        printf("[%10lu] received: %s\n", clock(), buf->chars);
        if (string_equals(buf, "ping")) {
            pong(conn);
        } else {
            String* msg = string_create("???");
            send_message(conn->fd, msg);
            string_free(msg);
        }
    }

    int err = close(conn->fd);
    if (err == -1) {
        perror("close");
        printf("failed to close\n");
    }

    free(conn);

    return NULL;
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

    connection_info_t* conn_info = malloc(sizeof(connection_info_t));
    conn_info->fd                = conn_fd;

    pthread_t thread;
    err = pthread_create(&thread, NULL, connection_thread, conn_info);
    if (err != 0) {
        perror("pthread_create");
        printf("failed to create thread");
        return err;
    }

    vec_push_back(conn_vec, &thread);

    return err;
}
