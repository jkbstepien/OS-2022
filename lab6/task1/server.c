#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>

#include "common.h"

int first_available_ID = 0;
int squeue;
key_t cQueues[MAX_NO_CLIENTS];

void service_INIT(message_buffer *messageBuffer);

void service_STOP(int cID);

void service_END();

void service_LIST(int cID);

void service_2ONE(message_buffer *messageBuffer);

void service_2ALL(message_buffer *messageBuffer);

void store_message_logs(message_buffer *messageBuffer);

int main() {
    // Initialize client queue with dummy data.
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        cQueues[i] = -1;
    }

    key_t queue_key = ftok(HOME_PATH, SERVER_ID);
    squeue = msgget(queue_key, IPC_CREAT | 0666);

    signal(SIGINT, service_END);

    message_buffer *pMessageBuffer = malloc(sizeof(message_buffer));
    while (1) {
        msgrcv(squeue, pMessageBuffer, MSG_SIZE, -6, 0);

        switch (pMessageBuffer->message_type) {
            case INIT:
                service_INIT(pMessageBuffer);
                break;
            case LIST:
                service_LIST(pMessageBuffer->client_id);
                store_message_logs(pMessageBuffer);
                break;
            case TO_ONE:
                service_2ONE(pMessageBuffer);
                store_message_logs(pMessageBuffer);
                break;
            case TO_ALL:
                service_2ALL(pMessageBuffer);
                store_message_logs(pMessageBuffer);
                break;
            case STOP:
                service_STOP(pMessageBuffer->client_id);
                store_message_logs(pMessageBuffer);
                break;
            default:
                printf("Unexpected message type!\n");
        }
    }
}

void service_INIT(message_buffer *messageBuffer) {
    while (cQueues[first_available_ID] != -1 && first_available_ID < MAX_NO_CLIENTS - 1) {
        first_available_ID++;
    }

    // Check if client limit had been reached or process normally for INIT.
    if (cQueues[first_available_ID] != -1 && first_available_ID == MAX_NO_CLIENTS - 1) {
        messageBuffer->client_id = -1;
    } else {
        messageBuffer->client_id = first_available_ID;
        cQueues[first_available_ID] = messageBuffer->queue_key;

        if (first_available_ID < MAX_NO_CLIENTS - 1) {
            first_available_ID++;
        }
    }

    int cQueueID = msgget(messageBuffer->queue_key, 0);
    msgsnd(cQueueID, messageBuffer, MSG_SIZE, 0);
    store_message_logs(messageBuffer);
}

void service_STOP(int cID) {
    cQueues[cID] = -1;

    if (cID < first_available_ID) {
        first_available_ID = cID;
    }
}

void service_END() {
    message_buffer *pMessageBuffer = malloc(sizeof(message_buffer));

    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (cQueues[i] != -1) {
            pMessageBuffer->message_type = STOP;
            int cQueueID = msgget(cQueues[i], 0);
            msgsnd(cQueueID, pMessageBuffer, MSG_SIZE, 0);
            msgrcv(squeue, pMessageBuffer, MSG_SIZE, STOP, 0);
        }
    }

    msgctl(squeue, IPC_RMID, NULL);
    exit(0);
}

void service_LIST(int cID) {
    message_buffer *pMessageBuffer = malloc(sizeof(message_buffer));
    strcpy(pMessageBuffer->message_content, "");

    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (cQueues[i] != -1) {
            sprintf(pMessageBuffer->message_content + strlen(pMessageBuffer->message_content), "ID %d is running..\n", i);
        }
    }

    pMessageBuffer->message_type = LIST;
    int cQueueID = msgget(cQueues[cID], 0);

    msgsnd(cQueueID, pMessageBuffer, MSG_SIZE, 0);
}

void service_2ONE(message_buffer *messageBuffer) {
    int otherQueueID = msgget(cQueues[messageBuffer->other_id], 0);
    msgsnd(otherQueueID, messageBuffer, MSG_SIZE, 0);
}

void service_2ALL(message_buffer *messageBuffer) {
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (cQueues[i] != -1 && i != messageBuffer->client_id) {
            int otherQueueID = msgget(cQueues[i], 0);
            msgsnd(otherQueueID, messageBuffer, MSG_SIZE, 0);
        }
    }
}

void store_message_logs(message_buffer *messageBuffer) {
    struct tm my_time = messageBuffer->ltimestruct;

    FILE *result_file = fopen("logs.txt", "a");

    switch (messageBuffer->message_type) {
        case INIT:
            if (messageBuffer->client_id == -1) {
                fprintf(result_file, "(INIT) Max number of clients is reached!\n");
            } else {
                fprintf(result_file, "(INIT) Client ID: %d\n", messageBuffer->client_id);
            }
            break;
        case LIST:
            fprintf(result_file, "(LIST) Client ID: %d\n", messageBuffer->client_id);
            break;
        case TO_ONE:
            fprintf(result_file, "Message: %s\n", messageBuffer->message_content);
            fprintf(result_file, "(2ONE) Sender ID: %d, Receiver ID %d\n", messageBuffer->client_id, messageBuffer->other_id);
            break;
        case TO_ALL:
            fprintf(result_file, "Message: %s\n", messageBuffer->message_content);
            fprintf(result_file, "(2ALL) Client ID: %d\n", messageBuffer->client_id);
            break;
        case STOP:
            fprintf(result_file, "(STOP) Client ID: %d\n", messageBuffer->client_id);
            break;
    }

    fprintf(result_file, "sent at: %02d:%02d:%02d\n\n\n",
            my_time.tm_hour,
            my_time.tm_min,
            my_time.tm_sec);

    fclose(result_file);
}
