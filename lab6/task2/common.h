#ifndef LAB6_COMMON_H
#define LAB6_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>

// Macros
#define LENN 3
#define SQUEUE "/SERVER"
#define MAX_NO_CLIENTS 10
#define MAX_MESSAGE_SIZE 512

// Necessary structures and enum types
typedef struct message_buffer{
    int clientID;
    int otherID;
    long message_type;
    char message_content[MAX_MESSAGE_SIZE];
    struct tm ltimestruct;
} message_buffer;

typedef enum message_type {
    INIT = 1,
    LIST = 2,
    TOONE = 3,
    TOALL = 4,
    STOP = 5
} message_type;

// Global variables
const int MSG_SIZE = sizeof(message_buffer);

// Supplemental functions
char randomC() {
    return rand() % ('Z' - 'A' + 1) + 'A';
}

mqd_t create_queue(const char* name) {
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_NO_CLIENTS;
    attr.mq_msgsize = MSG_SIZE;
    return mq_open(name, O_RDWR | O_CREAT, 0666, &attr);
}

#endif //LAB6_COMMON_H
