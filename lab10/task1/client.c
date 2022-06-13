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

typedef struct Client {
    char name[32];
    int socket;
    int available;
    int opponent;
} client_t;

char player1_sign;
char player2_sign;
int player1_moves[9];
int player2_moves[9];
int player1_no_moves = 0;
int player2_no_moves = 0;

char *username;
int socket_server;

void message_send(int file_desc, char *buffer);
char *message_receive(int file_desc);
void end_game_session();
void server_listener();
void player_turn();
int validate_player_move(int move_number);
void validate_game_status();
char get_game_result();
void draw_board();

int main(int argc, char **argv) {

    if (argc != 4) {
        perror("Incorrect number of arguments! Try: [username] [network|local] [pathname|port]");
        exit(1);
    }

    signal(SIGINT, end_game_session);

    username = argv[1];
    if (strcmp(argv[2], "network") == 0) {
        if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Cannot create network socket");
            exit(1);
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htobe16(atoi(argv[3]));
        addr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);

        if (connect(socket_server, (const struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
            perror("Cannot connect");
            exit(1);
        }
    } else if (strcmp(argv[2], "local") == 0) {
        if ((socket_server = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("Cannot create local socket");
            exit(1);
        }

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, argv[3]);

        if (connect(socket_server, (const struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
            perror("Cannot connect");
            exit(1);
        }
    } else {
        perror("Invalid connection type! Try: [network|local].");
        exit(1);
    }

    printf("Connection to the server established.\n");

    // Register user on the server.
    char buf[MAX_MESSAGE_LENGTH];
    sprintf(buf, "register %s", username);
    message_send(socket_server, buf);

    // Server listen.
    struct pollfd *fd = calloc(1, sizeof(struct pollfd));
    fd->fd = socket_server;
    fd->events = POLLIN;
    fd->revents = 0;

    while (1) {
        poll(fd, 1, -1);
        char *buf = message_receive(socket_server);
        char *cmd = strtok(buf, " ");
        if (strcmp(cmd, "add") == 0) {
            printf("Registration success\n");
        } else if (strcmp(cmd, "ping") == 0) {
            message_send(socket_server, "ping");
        } else if (strcmp(cmd, "no_opponent") == 0) {
            printf("Waiting for opponent...\n");
        } else if (strcmp(cmd, "name_taken") == 0) {
            printf("Name already taken\n");
            shutdown(socket_server, SHUT_RDWR);
            close(socket_server);
            exit(0);
        } else if (strcmp(cmd, "X") == 0) {
            printf("My symbol X\n");
            player1_sign = 'X';
            player2_sign = 'O';
        } else if (strcmp(cmd, "O") == 0) {
            printf("My symbol O\n");
            player1_sign = 'O';
            player2_sign = 'X';
            draw_board();
            player_turn();
        } else if (strcmp(cmd, "player_turn") == 0) {
            int m = atoi(strtok(NULL, " "));
            player2_moves[player2_no_moves] = m;
            player2_no_moves++;
            draw_board();
            player_turn();
        } else if (strcmp(cmd, "lose") == 0) {
            printf("You lose!\n");
            sleep(1);
            end_game_session();
        } else if (strcmp(cmd, "draw") == 0) {
            printf("It's a draw\n");
            sleep(1);
            end_game_session();
        } else if (strcmp(cmd, "win") == 0) {
            printf("You won!\n");
            sleep(1);
            end_game_session();
        }
    }
}

char *message_receive(int file_desc) {
    char *buffer = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
    if (read(file_desc, buffer, MAX_MESSAGE_LENGTH) == -1) {
        perror("Message not received :(");
        exit(1);
    }
    return buffer;
}

void message_send(int file_desc, char *buffer) {
    if (write(file_desc, buffer, MAX_MESSAGE_LENGTH) == -1) {
        perror("Message not sent :(");
        exit(1);
    }
}

void end_game_session() {
    printf("\nPlayer signed out, sending info to the server..\n");
    message_send(socket_server, "end");
    exit(0);
}

void player_turn() {
    int m;
    do {
        printf("Your player_turn: ");
        scanf("%d", &m);
    } while (!validate_player_move(m));

    player1_moves[player1_no_moves] = m;
    player1_no_moves++;
    draw_board();
    validate_game_status();

    char buf[MAX_MESSAGE_LENGTH];
    sprintf(buf, "player_turn %d", m);
    message_send(socket_server, buf);

}

int validate_player_move(int move_number) {
    for (int i = 0; i < player1_no_moves; ++i) {
        if (player1_moves[i] == move_number) {
            return 0;
        }
    }
    for (int i = 0; i < player2_no_moves; ++i) {
        if (player2_moves[i] == move_number) {
            return 0;
        }
    }
    return 1;
}

void validate_game_status() {
    char symbol = get_game_result();

    if (symbol != ' ') {
        message_send(socket_server, "win");
    }
    if (player1_no_moves + player2_no_moves == 9) {
        message_send(socket_server, "draw");
    }
}

char get_game_result() {
    char board[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
    for (int i = 0; i < player1_no_moves; i++) {
        int m = player1_moves[i] - 1;
        board[m / 3][m % 3] = player1_sign;
    }
    for (int i = 0; i < player2_no_moves; i++) {
        int m = player2_moves[i] - 1;
        board[m / 3][m % 3] = player2_sign;
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

void draw_board() {
    char board[3][3];
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) board[i][j] = ' ';
    for (int i = 0; i < player1_no_moves; i++) {
        int m = player1_moves[i] - 1;
        board[m / 3][m % 3] = player1_sign;
    }
    for (int i = 0; i < player2_no_moves; i++) {
        int m = player2_moves[i] - 1;
        board[m / 3][m % 3] = player2_sign;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("|%c", board[i][j]);
        }
        printf("|\n");
    }
}
