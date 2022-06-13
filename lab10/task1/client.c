#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <poll.h>

#define MAX_MESSAGE_LENGTH 256

int serverSocket;

char otherSymbol, thisSymbol;

char *name;

int otherMoves[9];

int otherMovesCount = 0;

int thisMoves[9];

int thisMovesCount = 0;

void error(char *msg) {
    perror(msg);
    exit(1);
}

typedef struct Client {
    char name[32];
    int socket;
    int available;
    int opponent;
} client_t;

char *recvMsg(int fd) {
    char *buf = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
    if (read(fd, buf, MAX_MESSAGE_LENGTH) == -1) {
        error("Cannot receive message");
    }
    return buf;
}

void sendMsg(int fd, char *buf) {
    if (write(fd, buf, MAX_MESSAGE_LENGTH) == -1) {
        error("Cannot send message");
    }
}

void connectLocal(char *path);
void connectNetwork(char *port);
void registerOnServer();
void listenToServer();
void endSession();
void printBoard();
void move();
int checkMove(int num);
void checkStatus();
char getWinner();

void endSession() {
    printf("Sending end to server\n");
    sendMsg(serverSocket, "end");
    exit(0);
}

void listenToServer() {
    struct pollfd *fd = calloc(1, sizeof(struct pollfd));
    fd->fd = serverSocket;
    fd->events = POLLIN;
    fd->revents = 0;

    while (1) {
        poll(fd, 1, -1);
        char *buf = recvMsg(serverSocket);
//        printf("%s\n", buf);
        char *cmd = strtok(buf, " ");
        if (strcmp(cmd, "add") == 0) {
            printf("Registration success\n");
        } else if (strcmp(cmd, "ping") == 0) {
            sendMsg(serverSocket, "ping");
        } else if (strcmp(cmd, "no_opponent") == 0) {
            printf("Waiting for opponent...\n");
        } else if (strcmp(cmd, "name_taken") == 0) {
            printf("Name already taken\n");
            shutdown(serverSocket, SHUT_RDWR);
            close(serverSocket);
            exit(0);
        } else if (strcmp(cmd, "X") == 0) {
            printf("My symbol X\n");
            thisSymbol = 'X';
            otherSymbol = 'O';
        } else if (strcmp(cmd, "O") == 0) {
            printf("My symbol O\n");
            thisSymbol = 'O';
            otherSymbol = 'X';
            printBoard();
            move();
        } else if (strcmp(cmd, "move") == 0) {
            int m = atoi(strtok(NULL, " "));
            otherMoves[otherMovesCount] = m;
            otherMovesCount++;
            printBoard();
            move();
        } else if (strcmp(cmd, "lose") == 0) {
            printf("You lose!\n");
            sleep(1);
            endSession();
        } else if (strcmp(cmd, "draw") == 0) {
            printf("It's a draw\n");
            sleep(1);
            endSession();
        } else if (strcmp(cmd, "win") == 0) {
            printf("You won!\n");
            sleep(1);
            endSession();
        }

    }
}

void move() {
    int m;
    do {
        printf("Your move: ");
        scanf("%d", &m);
    } while (!checkMove(m));

    thisMoves[thisMovesCount] = m;
    thisMovesCount++;
    printBoard();
    checkStatus();

    char buf[MAX_MESSAGE_LENGTH];
    sprintf(buf, "move %d", m);
    sendMsg(serverSocket, buf);

}

void checkStatus() {
    char symbol = getWinner();

    if (symbol != ' ') {
        sendMsg(serverSocket, "win");
    }
    if (thisMovesCount + otherMovesCount == 9) {
        sendMsg(serverSocket, "draw");
    }
}

char getWinner() {
    char board[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
    for (int i = 0; i < thisMovesCount; i++) {
        int m = thisMoves[i] - 1;
        board[m / 3][m % 3] = thisSymbol;
    }
    for (int i = 0; i < otherMovesCount; i++) {
        int m = otherMoves[i] - 1;
        board[m / 3][m % 3] = otherSymbol;
    }
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][0] == board[i][2] && board[i][0] != ' ') {
            return board[i][0];

        } else if (board[0][i] == board[1][i] && board[0][i] == board[2][i] && board[0][i] != ' ') {
            return board[0][i];
        }
    }

    if (board[0][0] == board[1][1] && board[0][0] == board[2][2] && board[0][0] != ' ') {
        return board[0][0];
    }
    if (board[0][2] == board[1][1] && board[0][2] == board[2][0] && board[0][2] != ' ') {
        return board[0][2];
    }
    return ' ';
}

int checkMove(int num) {
    for (int i = 0; i < thisMovesCount; ++i) {
        if (thisMoves[i] == num) {
            return 0;
        }
    }
    for (int i = 0; i < otherMovesCount; ++i) {
        if (otherMoves[i] == num) {
            return 0;
        }
    }
    return 1;
}

void printBoard() {
    char board[3][3];
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) board[i][j] = ' ';
    for (int i = 0; i < thisMovesCount; i++) {
        int m = thisMoves[i] - 1;
        board[m / 3][m % 3] = thisSymbol;
    }
    for (int i = 0; i < otherMovesCount; i++) {
        int m = otherMoves[i] - 1;
        board[m / 3][m % 3] = otherSymbol;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("|%c", board[i][j]);
        }
        printf("|\n");
    }
}

void registerOnServer() {
    char buf[MAX_MESSAGE_LENGTH];
    sprintf(buf, "register %s", name);
    sendMsg(serverSocket, buf);
}

void connectNetwork(char *port) {
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        error("Cannot create network socket");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htobe16(atoi(port));
    addr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);

    if (connect(serverSocket, (const struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
        error("Cannot connect");
    }
}

void connectLocal(char *path) {
    if ((serverSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        error("Cannot create local socket");
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    if (connect(serverSocket, (const struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
        error("Cannot connect");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        error("Wrong number of arguments: <name> <local|network> <pathname|port>");
    }

    signal(SIGINT, endSession);

    name = argv[1];
    if (strcmp(argv[2], "local") == 0) {
        connectLocal(argv[3]);
    } else if (strcmp(argv[2], "network") == 0) {
        connectNetwork(argv[3]);
    } else {
        error("Wrong connection type");
    }
    printf("Connected to the server\n");
    registerOnServer();
    listenToServer();
    return 0;
}
