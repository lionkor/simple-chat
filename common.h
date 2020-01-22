#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <unistd.h>

#define PORT 6942
#define MAXLINE 1024
#define MAXUSERNAMELEN 20
#define BACKLOG 5

typedef enum ascii_ctrl
{
    NUL = 0x00, // null
    SOH,        // start of heading
    STX,        // start of text
    ETX,        // end of text
    EOT,        // end of transmission
    ENQ,        // enquiry
    ACK,        // acknowledge
    BEL,        // bell (alarm)
    NAK = 0x15, // negative acknowledge
    CAN = 0x18, // cancel
} ascii_ctrl_t;

typedef enum custom_ctrl
{
    REQ = 0xFF00FF00,
    ID  = 0x4944,
} custom_ctrl_t;

static const unsigned int REQ_IDENTIFY[] = {
    REQ, ID, EOT
};

static const unsigned int IDENTIFY_ANSWER_OK[] = {
    ACK, ID, EOT
};

typedef struct message {
    size_t user_id;
    char   text[MAXLINE - sizeof(size_t)];
} message_t;

void print_version();

typedef struct String {
    char*  chars;
    size_t len;
} String;

String*      string_create(const char* str);
String*      string_create_empty(size_t buffer_size);
void         string_readline(String* str);
void         string_clear(String* str);
int /*bool*/ string_equals(String* s, const char* str);
void         string_free(String* str);

int send_message(int fd, String* msg);
int send_message_raw(int fd, const void* data, size_t size);

#include "c-vector/Vector.h"

#endif // COMMON_H
