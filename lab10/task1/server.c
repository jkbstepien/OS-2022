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
#define MAX_MESSAGE_LENGTH 256

typedef struct Client {
    char name[32];
    int socket;
    int available;
    int opponent;
} client_t;

// Client.
int socket_server;

// Server.
int socket_local;
int socket_network;
int no_clients;

char *user_name;

char player1_sign;
char player2_sign;

int player1_moves[9];
int player1_moves_count = 0;
int player2_moves[9];
int player2_moves_count = 0;

int validate_player_move(int move_number);

void get_current_game_status();

char game_result();

void draw_board();

void get_error_info(char *e) {
    perror(e);
    exit(1);
}

void end_session_handler() {
    printf("Client signed out, sending info to server..\n");
    if (write(socket_server, "end", MAX_MESSAGE_LENGTH) == -1) {
        get_error_info("Cannot send message");
    }
    exit(0);
}

void send_message(int fd, char *buffer) {
    if (write(fd, buffer, MAX_MESSAGE_LENGTH) == -1) {
        get_error_info("Message not sent :(");
    }
}

void draw_board() {
    char b[3][3];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            b[i][j] = ' ';
        }
    }

    for (int i = 0; i < player1_moves_count; i++) {
        int m = player1_moves[i] - 1;
        b[m / 3][m % 3] = player1_sign;
    }

    for (int i = 0; i < player2_moves_count; i++) {
        int m = player2_moves[i] - 1;
        b[m / 3][m % 3] = player2_sign;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("|%c", b[i][j]);
        }
        printf("|\n");
    }
}

void player_turn() {
    int move_no;

    do {
        printf("Pick move: ");
        scanf("%d", &move_no);
    } while (!validate_player_move(move_no));

    player1_moves[player1_moves_count] = move_no;
    player1_moves_count++;

    draw_board();
    get_current_game_status();

    char buf[MAX_MESSAGE_LENGTH];
    sprintf(buf, "player_turn %d", move_no);
    send_message(socket_server, buf);

}

int validate_player_move(int move_number) {

    for (int i = 0; i < player1_moves_count; ++i) {
        if (player1_moves[i] == move_number) {
            return 0;
        }
    }

    for (int i = 0; i < player2_moves_count; ++i) {
        if (player2_moves[i] == move_number) {
            return 0;
        }
    }

    return 1;
}

void get_current_game_status() {
    char sign = game_result();

    if (sign != ' ') {
        send_message(socket_server, "win");
    }
    if (player1_moves_count + player2_moves_count == 9) {
        send_message(socket_server, "draw");
    }
}

char game_result() {
    char b[3][3];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            b[i][j] = ' ';
        }
    }

    for (int i = 0; i < player1_moves_count; i++) {
        int m = player1_moves[i] - 1;
        b[m / 3][m % 3] = player1_sign;
    }

    for (int i = 0; i < player2_moves_count; i++) {
        int m = player2_moves[i] - 1;
        b[m / 3][m % 3] = player2_sign;
    }

    for (int i = 0; i < 3; i++) {
        if (b[i][0] == b[i][1]
            && b[i][0] == b[i][2]
            && b[i][0] != ' ') {
            return b[i][0];

        } else if (b[0][i] == b[1][i]
                   && b[0][i] == b[2][i]
                   && b[0][i] != ' ') {
            return b[0][i];
        }
    }

    if (b[0][0] == b[1][1]
        && b[0][0] == b[2][2]
        && b[0][0] != ' ') {
        return b[0][0];
    }
    if (b[0][2] == b[1][1]
        && b[0][2] == b[2][0]
        && b[0][2] != ' ') {
        return b[0][2];
    }
    return ' ';
}

void error(char *msg) {
    perror(msg);
    exit(1);
}



void sendMsg(int fd, char *buf) {
    if (write(fd, buf, MAX_MESSAGE_LENGTH) == -1) {
        error("Cannot send message");
    }
}

int localSocket;
int networkSocket;
int clientsCount;

client_t clients[MAX_NUMBER_OF_PLAYERS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void setupNetworkSocket(char *port);

void setupLocalSocket(char *path);

void serverListen();

int monitorClients();

int addClient(char *name, int socket);

void addOpponent(int fd);

int getRandom();

void removeClient(int fd);

void *pingRoutine(void *);

void removeUnavailableClients();

void makeAvailable(int fd);

int getClient(int fd);

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc != 3) {
        error("Wrong number of arguments: <port> <pathname>");
    }
    setupNetworkSocket(argv[1]);
    setupLocalSocket(argv[2]);
    serverListen();

    return 0;
}

void serverListen() {
    pthread_t thread;
    pthread_create(&thread, NULL, &pingRoutine, NULL);
    while (1) {
        int fd = monitorClients();
        char *buf = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
        if (read(fd, buf, MAX_MESSAGE_LENGTH) == -1) {
            error("Cannot receive message");
        }
        char *cmd = strtok(buf, " ");
//        printf("cmd: %s\n", cmd);
        pthread_mutex_lock(&mutex);
        if (strcmp(cmd, "register") == 0) {
            char *arg = strtok(NULL, " ");
            int res = addClient(arg, fd);
            switch (res) {
                case -1:
                    sendMsg(fd, "name_taken");
                    break;
                case 0:
                    sendMsg(fd, "add");
                    addOpponent(fd);
                    break;
            }
        } else if (strcmp(cmd, "move") == 0) {
            int i = getClient(fd);
            char *move = strtok(NULL, " ");
            char buf[MAX_MESSAGE_LENGTH];
            sprintf(buf, "move %s", move);
            sendMsg(clients[i].opponent, buf);
        } else if (strcmp(cmd, "win") == 0) {
            int i = getClient(fd);
            sendMsg(clients[i].opponent, "lose");
            sendMsg(fd, "win");
        } else if (strcmp(cmd, "draw") == 0) {
            int i = getClient(fd);
            sendMsg(clients[i].opponent, "draw");
            sendMsg(fd, "draw");
        } else if (strcmp(cmd, "ping") == 0) {
            makeAvailable(fd);
        } else if (strcmp(cmd, "end") == 0) {
            printf("Removing client\n");
            removeClient(fd);
        }
        pthread_mutex_unlock(&mutex);
    }
}

int getClient(int fd) {
    for (int i = 0; i < clientsCount; ++i) {
        if (clients[i].socket == fd) {
            return i;
        }
    }
    return -1;
}

void makeAvailable(int fd) {
    int i = getClient(fd);
    clients[i].available = 1;
}

void *pingRoutine(void *) {
    while (1) {
        printf("Ping clients...\n");
        pthread_mutex_lock(&mutex);
        removeUnavailableClients();
        for (int i = 0; i < clientsCount; ++i) {
            if (clients[i].available) {
                sendMsg(clients[i].socket, "ping");
                clients[i].available = 0;
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(15);
    }
}

void removeUnavailableClients() {
    for (int i = 0; i < clientsCount; ++i) {
        if (!clients[i].available) {
            if (i < clientsCount - 1) {
                sendMsg(clients[i].socket, "end");
                clients[i] = clients[clientsCount - 1];
            }
            clientsCount--;
        }
    }
}

void removeClient(int fd) {
    int i = getClient(fd);
    if (i < clientsCount - 1) {
        if (clients[i].opponent != -1) {
            for (int j = 0; j < clientsCount; ++j) {
                if (clients[j].socket == clients[i].opponent) {
                    clients[j].available = 0;
                    break;
                }
            }
        }
        clients[i] = clients[clientsCount - 1];
    }
    clientsCount--;
}

void addOpponent(int fd) {
    int opponentFound = 0;
    for (int i = 0; i < clientsCount; ++i) {
        if (clients[i].socket != fd && clients[i].opponent == -1) {
            opponentFound = 1;
            clients[i].opponent = fd;
            clients[clientsCount - 1].opponent = clients[i].socket;

            if (getRandom() % 2 == 0) {
                sendMsg(fd, "O");
                sendMsg(clients[i].socket, "X");
            } else {
                sendMsg(fd, "X");
                sendMsg(clients[i].socket, "O");
            }
            break;
        }
    }
    if (!opponentFound) {
        sendMsg(fd, "no_opponent");
    }
}

int getRandom() {
    return rand() % 100;
}

int addClient(char *name, int socket) {
    for (int i = 0; i < clientsCount; ++i) {
        if (clients[i].socket == socket || strcmp(clients[i].name, name) == 0) {
            return -1;
        }
    }
    client_t *client = calloc(1, sizeof(client_t));
    client->socket = socket;
    strcpy(client->name, name);
    client->available = 1;
    client->opponent = -1;

    clients[clientsCount] = *client;
    clientsCount++;
//    printf("%s registered\n", user_name);
    return 0;
}

int monitorClients() {

    struct pollfd *fds = calloc(clientsCount + 2, sizeof(struct pollfd));

    fds[0].fd = localSocket;
    fds[0].events = POLLIN;

    fds[1].fd = networkSocket;
    fds[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i = 2; i < clientsCount + 2; ++i) {
        fds[i].fd = clients[i - 2].socket;
        fds[i].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);

    poll(fds, clientsCount + 2, -1);
    int res;
    for (int i = 0; i < clientsCount + 2; ++i) {
        if (fds[i].revents & POLLIN) {
            res = fds[i].fd;
            break;
        }
    }
    if (res == localSocket || res == networkSocket) {
        res = accept(res, NULL, NULL);
    }
    free(fds);
    return res;
}

void setupLocalSocket(char *path) {
    if ((localSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        error("Cannot create local socket");
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    unlink(path);

    if (bind(localSocket, (const struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
        error("Cannot bind");
    }

    if (listen(localSocket, MAX_NUMBER_OF_PLAYERS) == -1) {
        error("Cannot listen");
    }
}

void setupNetworkSocket(char *port) {
    if ((networkSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        error("Cannot create network socket");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htobe16(atoi(port));
    addr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);

    if (bind(networkSocket, (const struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
        error("Cannot bind");
    }

    if (listen(networkSocket, MAX_NUMBER_OF_PLAYERS) == -1) {
        error("Cannot listen");
    }
}