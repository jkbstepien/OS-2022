#ifndef LAB6_COMMON_H
#define LAB6_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>

// Macros
#define HOME_PATH getenv("HOME")
#define SERVER_ID 1
#define MAX_NO_CLIENTS 10
#define MAX_SIZE_MESSAGE 512

// Necessary structures and enum types
typedef struct message_buffer {
    key_t queue_key;
    int client_id;
    int other_id;
    long message_type;
    char message_content[MAX_SIZE_MESSAGE];
    struct tm ltimestruct;
} message_buffer;

typedef enum message_type {
    INIT = 1,
    LIST = 2,
    TO_ONE = 3,
    TO_ALL = 4,
    STOP = 5
} message_type;

// Global variables
const int MSG_SIZE = sizeof(message_buffer);

#endif //LAB6_COMMON_H
