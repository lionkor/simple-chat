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

    size_t user_id = 0;
    // begin custom user_id handshake
    // (see serverside equivalent for details)
    // expect REQ_IDENTIFY
    unsigned int id_req[sizeof(REQ_IDENTIFY)];
    read(conn_fd, id_req, sizeof(REQ_IDENTIFY));
    if (memcmp(id_req, REQ_IDENTIFY, sizeof(REQ_IDENTIFY)) != 0) {
        printf("did not receive REQ_IDENTIFY\n");
        return 1;
    } else {
        printf("request received\n");
    }
    send_message_raw(conn_fd, &IDENTIFY_ANSWER_OK, sizeof(IDENTIFY_ANSWER_OK));
    printf("sent IDENTIFY_ANSWER_OK\n");
    read(conn_fd, &user_id, sizeof(user_id));
    printf("received user id: %lu\n", user_id);
    unsigned char id_response = ACK;
    send_message_raw(conn_fd, &id_response, sizeof(unsigned char));
    printf("responded with ACK\n");
    // end custom user_id handshake


    // READ / WRITE HERE
    String* msg_str  = string_create_empty(MAXLINE);
    String* response = string_create_empty(MAXLINE);
    for (;;) {
        printf("[%lu] > ", user_id);
        string_readline(msg_str);

        message_t msg;
        msg.user_id = user_id;
        if (strlen(msg_str->chars) > sizeof(msg.text)) {
            printf("message too long!\n");
        }

        strncpy(msg.text, msg_str->chars, msg_str->len);

        send_message_raw(conn_fd, &msg, sizeof(message_t));
        read(conn_fd, response->chars, response->len);
        char c = ACK;
        if (memcmp(response->chars, &c, sizeof(c)) != 0) {
            printf("[%10lu] received: %s\n", clock(), response->chars);
        }
        // clear response buffer
        string_clear(msg_str);
        string_clear(response);
    }
    string_free(msg_str);
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

