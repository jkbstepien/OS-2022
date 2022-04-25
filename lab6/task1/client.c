#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>

#include "common.h"

key_t qKey;
int qID;
int squeueID;
int client_id;

int manage_init();

void manage_stop();

void handle_server_message();

void manage_LIST();

void manage_2ONE(int otherID, char *message);

void manage_2ALL(char *message);

int main() {
    srand(time(NULL));

    qKey = ftok(HOME_PATH, rand() % 255 + 1);
    qID = msgget(qKey, IPC_CREAT | 0666);
    key_t sKey = ftok(HOME_PATH, SERVER_ID);
    squeueID = msgget(sKey, 0);
    client_id = manage_init();

    signal(SIGINT, manage_stop);

    char *command = NULL;
    size_t len = 0;
    ssize_t read;

    while (1) {
        printf("User command: ");
        read = getline(&command, &len, stdin);
        command[read - 1] = '\0';

        handle_server_message();

        if (strcmp(command, "") == 0) {
            continue;
        }

        char *curr_cmd = strtok(command, " ");
        if (strcmp(curr_cmd, "LIST") == 0) {
            manage_LIST();
        } else if (strcmp(curr_cmd, "2ALL") == 0) {
            curr_cmd = strtok(NULL, " ");
            char *message = curr_cmd;
            manage_2ALL(message);
        } else if (strcmp(curr_cmd, "2ONE") == 0) {
            curr_cmd = strtok(NULL, " ");
            int id_to_send = atoi(curr_cmd);
            curr_cmd = strtok(NULL, " ");
            char *message = curr_cmd;
            manage_2ONE(id_to_send, message);
        } else if (strcmp(curr_cmd, "STOP") == 0) {
            manage_stop();
        } else {
            printf("Command not recognized!\n");
        }
    }
}

int manage_init() {
    time_t my_time = time(NULL);
    message_buffer *pMessageBuffer = malloc(sizeof(message_buffer));

    pMessageBuffer->ltimestruct = *localtime(&my_time);
    pMessageBuffer->message_type = INIT;
    pMessageBuffer->queue_key = qKey;

    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
    msgrcv(qID, pMessageBuffer, MSG_SIZE, 0, 0);

    int client_id = pMessageBuffer->client_id;
    if (client_id == -1) {
        printf("Client limit had been reached, leaving..\n");
        exit(0);
    }

    return client_id;
}

void manage_LIST() {
    time_t my_time = time(NULL);
    message_buffer *pMessageBuffer = malloc(sizeof(message_buffer));

    pMessageBuffer->ltimestruct = *localtime(&my_time);
    pMessageBuffer->message_type = LIST;
    pMessageBuffer->client_id = client_id;

    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
    msgrcv(qID, pMessageBuffer, MSG_SIZE, 0, 0);
    printf("%s\n", pMessageBuffer->message_content);
}

void manage_2ONE(int otherID, char *message) {
    time_t my_time = time(NULL);
    message_buffer *pMessageBuffer = malloc(sizeof(message_buffer));

    pMessageBuffer->ltimestruct = *localtime(&my_time);
    pMessageBuffer->message_type = TO_ONE;
    strcpy(pMessageBuffer->message_content, message);

    pMessageBuffer->client_id = client_id;
    pMessageBuffer->other_id = otherID;
    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
}

void manage_2ALL(char *message) {
    time_t my_time = time(NULL);
    message_buffer *pMessageBuffer = malloc(sizeof(message_buffer));

    pMessageBuffer->ltimestruct = *localtime(&my_time);
    pMessageBuffer->message_type = TO_ALL;
    strcpy(pMessageBuffer->message_content, message);

    pMessageBuffer->client_id = client_id;
    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
}

void manage_stop() {
    time_t my_time = time(NULL);
    message_buffer *pMessageBuffer = malloc(sizeof(message_buffer));

    pMessageBuffer->ltimestruct = *localtime(&my_time);
    pMessageBuffer->message_type = STOP;
    pMessageBuffer->client_id = client_id;

    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
    msgctl(qID, IPC_RMID, NULL);
    exit(0);
}

void handle_server_message() {
    message_buffer *msg_rcv = malloc(sizeof(message_buffer));
    while (msgrcv(qID, msg_rcv, MSG_SIZE, 0, IPC_NOWAIT) >= 0) {
        if (msg_rcv->message_type == STOP) {
            printf("Received stop message, leaving..\n");
            manage_stop();
        } else {
            struct tm my_time = msg_rcv->ltimestruct;
            printf("Msg from: %d has been sent at %02d:%02d:%02d:\n%s\n",
                   msg_rcv->client_id,
                   my_time.tm_hour,
                   my_time.tm_min,
                   my_time.tm_sec,
                   msg_rcv->message_content);
        }
    }
}