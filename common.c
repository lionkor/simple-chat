#include <stdio.h>

#include "common.h"

const char* const VERSION_STRING = "0.1.0";

void print_version() {
    printf("simple-chat version %s\n", VERSION_STRING);
}
