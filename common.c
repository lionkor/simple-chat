#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "common.h"

const char* const SC_VERSION_STRING = "0.1.0";

void print_version() {
    printf("simple-chat version %s\n", SC_VERSION_STRING);
}

void string_readline(String* str) {
    size_t  size;
    ssize_t n         = getline(&str->chars, &size, stdin) + 1;
    str->chars[n - 2] = 0;
}

String* string_create(const char* str, size_t len) {
    String* new_string = (String*)malloc(sizeof(String));
    assert(new_string != NULL);
    new_string->chars = calloc(len, sizeof(char));
    memset(new_string->chars, 0, new_string->len);
    strcpy(new_string->chars, str);
    new_string->len = len;
    return new_string;
}

String* string_create_empty(size_t buffer_size) {
    String* new_string = (String*)malloc(sizeof(String));
    assert(new_string != NULL);
    new_string->chars = calloc(buffer_size, sizeof(char));
    memset(new_string->chars, 0, buffer_size);
    new_string->len = buffer_size;
    return new_string;
}

void string_free(String* str) {
    if (str)
        free(str->chars);
    free(str);
}
