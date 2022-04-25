#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "common.h"

int server_queue;
int receiver_queue = -1;
int client_id;
int client_queue;
pid_t child_pid;

void init();

void stop();

void sigint_handle(int signum);

void to_command_worker(int sig, siginfo_t *siginfo, void *context);

int main() {
    char *path = getenv("HOME");

    key_t server_key = ftok(path, SERVER_ID);
    server_queue = msgget(server_key, 0666);

    key_t client_key = ftok(path, getpid());
    client_queue = msgget(client_key, IPC_CREAT | 0666);

    signal(SIGINT, sigint_handle);
    init();

    pid_t parent_pid = getpid();

    child_pid = fork();
    if (child_pid == 0) {
        signal(SIGINT, SIG_DFL);
        int type;
        message reply;
        while (1) {
            if (msgrcv(client_queue, &reply, MSG_LEN, -INIT - 1, 0) >= 0) {
                type = reply.type;
                if (type == CONNECT) {
                    receiver_queue = atoi(reply.text);
                    union sigval value;
                    value.sival_int = receiver_queue;
                    sigqueue(parent_pid, SIGUSR1, value);
                    break;
                } else if (type == DISCONNECT) {
                    receiver_queue = -1;
                    break;
                } else if (type == SEND) {
                    printf("Received a message: %s\n", reply.text);
                    break;
                } else if (type == STOP) {
                    kill(parent_pid, SIGINT);
                    break;
                } else if (type == LIST) {
                    puts(reply.text);
                    break;
                }
            } else {
                exit(0);
            }
        }
    } else {
        struct sigaction fpe;
        fpe.sa_sigaction = to_command_worker;
        fpe.sa_flags = SA_SIGINFO;
        sigemptyset(&fpe.sa_mask);
        sigaction(SIGUSR1, &fpe, NULL);

        message msg;
        int sent;
        char line[MSG_LEN];
        while (1) {
            sent = 0;
            msg.type = 0;
            fgets(line, MSG_LEN, stdin);

            if (!strcmp("CONNECT\n", line)) {
                msg.type = CONNECT;
                strtok(line, " ");
                int p2p_id = atoi(strtok(NULL, " "));
                sprintf(msg.text, "%d %d", client_id, p2p_id);
            }
            if (!strcmp("DISCONNECT\n", line) && receiver_queue != -1) {
                msg.type = DISCONNECT;
                sprintf(msg.text, "%d", client_id);
                receiver_queue = -1;
            }
            if (!strcmp("SEND\n", line) && receiver_queue != -1) {
                msg.type = SEND;
                sprintf(msg.text, "%s", strchr(line, ' ') + 1);
                sent = 1;
            }
            if (!strcmp("STOP\n", line)) {
                kill(child_pid, SIGKILL);
                stop();
                exit(0);
            }
            if (!strcmp("LIST\n", line)) {
                msg.type = LIST;
                sprintf(msg.text, "%d", client_id);
            }
            if (msg.type != 0) {
                int receiver;
                if (sent) receiver = receiver_queue;
                else receiver = server_queue;
                msgsnd(receiver, &msg, MSG_LEN, 0);
            }
        }
    }
    return 0;
}

void init() {
    message msg;
    msg.type = INIT;
    sprintf(msg.text, "%d", client_queue);
    msgsnd(server_queue, &msg, MSG_LEN, 0);

    memset(&msg, 0, sizeof(message));
    msgrcv(client_queue, &msg, MSG_LEN, INIT, 0);
    client_id = atoi(msg.text);
    printf("Client ID: %d\n", client_id);
}

void stop() {
    message msg;
    msg.type = STOP;
    sprintf(msg.text, "%d", client_id);
    msgsnd(server_queue, &msg, MSG_LEN, 0);

    printf("Closing client...\n");
    msgctl(client_queue, IPC_RMID, NULL);
}

void sigint_handle(int signum) {
    kill(child_pid, SIGKILL);
    stop();
    exit(0);
}

void to_command_worker(int sig, siginfo_t *siginfo, void *context) {
    receiver_queue = siginfo->si_value.sival_int;
}