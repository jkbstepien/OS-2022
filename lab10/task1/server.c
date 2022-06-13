#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>

#define MAX_NUMBER_OF_PLAYERS 12
#define MAX_MESSAGE_LENGTH 512

int socket_local;
int socket_network;

int number_of_clients;

typedef struct Client {
    char name[32];
    int socket;
    int available;
    int opponent;
} client_t;

client_t clients_container[MAX_NUMBER_OF_PLAYERS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void get_player_available(int f_desc);
int get_player(int f_desc);

int set_new_user(char *user_name, int socket);
void create_new_opponent(int fd);
void disconnect_player();
void delete_player(int fd);

void *ping_loop(void *);
int observe_active_players();

char *message_receive(int f_desc) {
    char *buffer = calloc(MAX_MESSAGE_LENGTH, sizeof(char));

    if (read(f_desc, buffer, MAX_MESSAGE_LENGTH) == -1) {
        perror("Message not received :(");
        exit(1);
    }

    return buffer;
}

void message_send(int fd, char *buf) {
    if (write(fd, buf, MAX_MESSAGE_LENGTH) == -1) {
        perror("Message not sent :(");
        exit(1);
    }
}

void get_player_available(int f_desc) {
    int i = get_player(f_desc);
    clients_container[i].available = 1;
}

int get_player(int f_desc) {
    for (int i = 0; i < number_of_clients; ++i) {
        if (clients_container[i].socket == f_desc) {
            return i;
        }
    }

    return -1;
}

int set_new_user(char *user_name, int socket) {

    for (int i = 0; i < number_of_clients; ++i) {
        if (clients_container[i].socket == socket || strcmp(clients_container[i].name, user_name) == 0) {
            return -1;
        }
    }

    client_t *client = calloc(1, sizeof(client_t));
    client->socket = socket;
    strcpy(client->name, user_name);
    client->available = 1;
    client->opponent = -1;
    clients_container[number_of_clients] = *client;
    number_of_clients++;

    return 0;
}

void create_new_opponent(int fd) {
    int opponentFound = 0;
    for (int i = 0; i < number_of_clients; ++i) {
        if (clients_container[i].socket != fd && clients_container[i].opponent == -1) {
            opponentFound = 1;
            clients_container[i].opponent = fd;
            clients_container[number_of_clients - 1].opponent = clients_container[i].socket;

            if ((rand() % 100) % 2 == 0) {
                message_send(fd, "O");
                message_send(clients_container[i].socket, "X");
            } else {
                message_send(fd, "X");
                message_send(clients_container[i].socket, "O");
            }
            break;
        }
    }
    if (!opponentFound) {
        message_send(fd, "no_opponent");
    }
}

void disconnect_player() {
    for (int i = 0; i < number_of_clients; ++i) {
        if (!clients_container[i].available) {
            if (i < number_of_clients - 1) {
                message_send(clients_container[i].socket, "end");
                clients_container[i] = clients_container[number_of_clients - 1];
            }
            number_of_clients--;
        }
    }
}

void delete_player(int fd) {
    int i = get_player(fd);
    if (i < number_of_clients - 1) {
        if (clients_container[i].opponent != -1) {
            for (int j = 0; j < number_of_clients; ++j) {
                if (clients_container[j].socket == clients_container[i].opponent) {
                    clients_container[j].available = 0;
                    break;
                }
            }
        }
        clients_container[i] = clients_container[number_of_clients - 1];
    }
    number_of_clients--;
}

void *ping_loop(void *) {
    while (1) {
        printf("Pinging existing players..\n");
        pthread_mutex_lock(&mutex);

        disconnect_player();
        for (int i = 0; i < number_of_clients; ++i) {
            if (clients_container[i].available) {
                message_send(clients_container[i].socket, "ping");
                clients_container[i].available = 0;
            }
        }

        pthread_mutex_unlock(&mutex);
        sleep(10);
    }
}

int observe_active_players() {

    struct pollfd *fds = calloc(number_of_clients + 2, sizeof(struct pollfd));

    fds[0].fd = socket_local;
    fds[0].events = POLLIN;

    fds[1].fd = socket_network;
    fds[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i = 2; i < number_of_clients + 2; ++i) {
        fds[i].fd = clients_container[i - 2].socket;
        fds[i].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);

    poll(fds, number_of_clients + 2, -1);
    int res;
    for (int i = 0; i < number_of_clients + 2; ++i) {
        if (fds[i].revents & POLLIN) {
            res = fds[i].fd;
            break;
        }
    }
    if (res == socket_local || res == socket_network) {
        res = accept(res, NULL, NULL);
    }
    free(fds);
    return res;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        perror("Incorrect number of arguments! Try: [port] [pathname]");
        exit(1);
    }

    // Set network socket.
    if ((socket_network = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create network socket :(");
        exit(1);
    }

    struct sockaddr_in add_net;
    add_net.sin_family = AF_INET;
    add_net.sin_port = htobe16(atoi(argv[1]));
    add_net.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);

    if (bind(socket_network, (const struct sockaddr *) &add_net, sizeof(struct sockaddr)) == -1) {
        perror("Failed to bind socket :(");
        exit(1);
    }

    if (listen(socket_network, MAX_NUMBER_OF_PLAYERS) == -1) {
        perror("Failed to listen :(");
        exit(1);
    }

    // Set local socket.
    if ((socket_local = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create local socket :(");
        exit(1);
    }

    struct sockaddr_un add_loc;
    add_loc.sun_family = AF_UNIX;
    strcpy(add_loc.sun_path, argv[2]);
    unlink(argv[2]);

    if (bind(socket_local, (const struct sockaddr *) &add_loc, sizeof(struct sockaddr)) == -1) {
        perror("Failed to bind");
        exit(1);
    }

    if (listen(socket_local, MAX_NUMBER_OF_PLAYERS) == -1) {
        perror("Failed to listen :(");
        exit(1);
    }

    // Server listens.
    pthread_t t;
    pthread_create(&t, NULL, &ping_loop, NULL);

    while (1) {
        int file_desc = observe_active_players();
        char *buffer = message_receive(file_desc);
        char *command = strtok(buffer, " ");

        pthread_mutex_lock(&mutex);

        if (strcmp(command, "register") == 0) {
            char *argument = strtok(NULL, " ");
            int output = set_new_user(argument, file_desc);
            switch (output) {
                case -1:
                    message_send(file_desc, "name_taken");
                    break;
                case 0:
                    message_send(file_desc, "add");
                    create_new_opponent(file_desc);
                    break;
            }
        }  else if (strcmp(command, "ping") == 0) {
            get_player_available(file_desc);
        } else if (strcmp(command, "player_turn") == 0) {
            int player = get_player(file_desc);
            char *move = strtok(NULL, " ");
            char b_msg[MAX_MESSAGE_LENGTH];

            sprintf(b_msg, "player_turn %s", move);
            message_send(clients_container[player].opponent, b_msg);
        } else if (strcmp(command, "win") == 0) {
            int player = get_player(file_desc);

            message_send(clients_container[player].opponent, "lose");
            message_send(file_desc, "win");
        } else if (strcmp(command, "draw") == 0) {
            int player = get_player(file_desc);

            message_send(clients_container[player].opponent, "draw");
            message_send(file_desc, "draw");
        } else if (strcmp(command, "end") == 0) {
            printf("Removed client from server list.\n");
            delete_player(file_desc);
        }

        pthread_mutex_unlock(&mutex);
    }
}
