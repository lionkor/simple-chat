#include <stdio.h>
#include <sys/socket.h>
#include "common.h"

int main() {
    print_version();

    int socket_desc;
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1) {
        printf("Could not create socket.\n");
        return 1;
    }

    return 0;
}
