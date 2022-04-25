#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "common.h"

mqd_t sQueue;
char *cQueue[MAX_NO_CLIENTS];
int first_available_ID = 0;


void man_s_INIT(message_buffer message);
void man_s_STOP(int cID);
void man_s_END();
void man_LIST(int cID);
void man_2ALL(message_buffer message);
void man_2ONE(message_buffer message);
void store_logs(message_buffer message);

int main() {
    // Initialize client queue with dummy data.
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        cQueue[i] = NULL;
    }

    mq_unlink(SQUEUE);
    sQueue = create_queue(SQUEUE);

    signal(SIGINT, man_s_END);
    message_buffer messageBuffer;

    while (1) {
        mq_receive(sQueue, (char *) &messageBuffer, MSG_SIZE, NULL);
        switch (messageBuffer.message_type) {
            case INIT:
                man_s_INIT(messageBuffer);
                break;
            case LIST:
                man_LIST(messageBuffer.clientID);
                store_logs(messageBuffer);
                break;
            case TOALL:
                man_2ALL(messageBuffer);
                store_logs(messageBuffer);
                break;
            case TOONE:
                man_2ONE(messageBuffer);
                store_logs(messageBuffer);
                break;
            case STOP:
                man_s_STOP(messageBuffer.clientID);
                store_logs(messageBuffer);
                break;
            default:
                printf("Command not recognized!\n");
        }
    }
}

void man_s_INIT(message_buffer message) {
    while (cQueue[first_available_ID] != NULL && first_available_ID < MAX_NO_CLIENTS - 1) {
        first_available_ID++;
    }

    if (cQueue[first_available_ID] != NULL && first_available_ID == MAX_NO_CLIENTS - 1) {
        message.clientID = -1;
    } else {
        message.clientID = first_available_ID;
        cQueue[first_available_ID] = (char *) calloc(LENN, sizeof(char));
        strcpy(cQueue[first_available_ID], message.message_content);

        if (first_available_ID < MAX_NO_CLIENTS - 1) {
            first_available_ID++;
        }
    }

    mqd_t cQueueDesc = mq_open(message.message_content, O_RDWR);
    mq_send(cQueueDesc, (char *) &message, MSG_SIZE, 0);
    mq_close(cQueueDesc);
    store_logs(message);
}

void man_s_STOP(int cID) {
    cQueue[cID] = NULL;

    if (cID < first_available_ID) {
        first_available_ID = cID;
    }
}

void man_s_END() {
    message_buffer messageBuffer;
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (cQueue[i] != NULL) {
            messageBuffer.message_type = STOP;
            mqd_t otherQueueDecs = mq_open(cQueue[i], O_RDWR);

            mq_send(otherQueueDecs, (char *) &messageBuffer, MSG_SIZE, 0);
            mq_receive(sQueue, (char *) &messageBuffer, MSG_SIZE, NULL);
            mq_close(otherQueueDecs);
        }
    }

    mq_close(sQueue);
    exit(0);
}

void man_LIST(int cID) {
    message_buffer messageBuffer;
    strcpy(messageBuffer.message_content, "");

    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (cQueue[i] != NULL) {
            sprintf(messageBuffer.message_content + strlen(messageBuffer.message_content), "ID %d is running..\n", i);
        }
    }

    mqd_t cQueueDesc = mq_open(cQueue[cID], O_RDWR);
    mq_send(cQueueDesc, (char *) &messageBuffer, MSG_SIZE, 0);
    mq_close(cQueueDesc);
}

void man_2ALL(message_buffer message) {
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (i != message.clientID && cQueue[i] != NULL) {
            mqd_t other_client_queue_desc = mq_open(cQueue[i], O_RDWR);

            mq_send(other_client_queue_desc, (char *) &message, MSG_SIZE, 0);
            mq_close(other_client_queue_desc);
        }
    }
}

void man_2ONE(message_buffer message) {
    mqd_t otherQueueDesc = mq_open(cQueue[message.otherID], O_RDWR);

    mq_send(otherQueueDesc, (char *) &message, MSG_SIZE, 0);
    mq_close(otherQueueDesc);
}

void store_logs(message_buffer message) {
    struct tm my_time = message.ltimestruct;

    FILE *result_file = fopen("logs.txt", "a");

    switch (message.message_type) {
        case INIT:
            if (message.clientID == -1) {
                fprintf(result_file, "(INIT) Max number of clients is reached!\n");
            } else {
                fprintf(result_file, "(INIT) Client ID: %d\n", message.clientID);
            }
            break;
        case LIST:
            fprintf(result_file, "(LIST) Client ID: %d\n", message.clientID);
            break;
        case TOALL:
            fprintf(result_file, "Message: %s\n", message.message_content);
            fprintf(result_file, "(2ALL) Client ID: %d\n", message.clientID);
            break;
        case TOONE:
            fprintf(result_file, "Message: %s\n", message.message_content);
            fprintf(result_file, "(2ONE) Sender ID: %d, Receiver ID %d\n", message.clientID, message.otherID);
            break;
        case STOP:
            fprintf(result_file, "(STOP) Client ID: %d\n", message.clientID);
            break;
    }

    fprintf(result_file, "sent at: %02d:%02d:%02d\n\n\n",
            my_time.tm_hour,
            my_time.tm_min,
            my_time.tm_sec);

    fclose(result_file);
}
