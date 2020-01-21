#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include "common.h"

int main() {
    print_version();
    printf("I'm the client!\n");

    const char* address_str = "172.217.18.110"; //"134.255.233.88";

    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    assert(socket_desc != -1);

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(address_str);
    server.sin_family      = AF_INET;
    server.sin_port        = htons(80);

    // connect to server
    if (connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("connect error\n");
        return 1;
    }

    printf("connected\n");
    return 0;
}
