#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <unistd.h>

#define PORT 6942
#define MAXLINE 1024
#define MAXUSERNAMELEN 20
#define BACKLOG 5

void print_version();

typedef struct String {
    char*  chars;
    size_t len;
} String;

String* string_create(const char* str, size_t len);
String* string_create_empty(size_t buffer_size);
void    string_readline(String* str);
void    string_free(String* str);

#include "c-vector/Vector.h"

#endif // COMMON_H
