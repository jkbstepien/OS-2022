#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <poll.h>

#define MAX_MESSAGE_LENGTH 512

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
char *message_receive(int f_desc);
void end_game_session();
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
            perror("Error while creating network socket :(");
            exit(1);
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htobe16(atoi(argv[3]));
        addr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);

        if (connect(socket_server, (const struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
            perror("Error while connecting :(");
            exit(1);
        }
    } else if (strcmp(argv[2], "local") == 0) {
        if ((socket_server = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("Error while creating local socket :(");
            exit(1);
        }

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, argv[3]);

        if (connect(socket_server, (const struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
            perror("Error while connecting :(");
            exit(1);
        }
    } else {
        perror("Invalid connection type! Try: [network|local].");
        exit(1);
    }

    printf("Connection to the server established.\n");

    // Register user on the server.
    char b[MAX_MESSAGE_LENGTH];
    sprintf(b, "register %s", username);
    message_send(socket_server, b);

    // Server listen.
    struct pollfd *pPollfd = calloc(1, sizeof(struct pollfd));

    pPollfd->fd = socket_server;
    pPollfd->events = POLLIN;
    pPollfd->revents = 0;

    while (1) {
        poll(pPollfd, 1, -1);

        char *buffer = message_receive(socket_server);
        char *command = strtok(buffer, " ");

        if (strcmp(command, "add") == 0) {
            printf("Successfully registered user.\n");
        } else if (strcmp(command, "ping") == 0) {
            message_send(socket_server, "ping");
        } else if (strcmp(command, "name_taken") == 0) {
            printf("Username taken! Try another.\n");
            shutdown(socket_server, SHUT_RDWR);
            close(socket_server);
            exit(0);
        } else if (strcmp(command, "no_opponent") == 0) {
            printf("Waiting for opponent to join the game..\n");
        } else if (strcmp(command, "player_turn") == 0) {
            int m = atoi(strtok(NULL, " "));
            player2_moves[player2_no_moves] = m;
            player2_no_moves++;
            draw_board();
            printf("\n");
            player_turn();
        } else if (strcmp(command, "X") == 0) {
            printf("\nYou play with: X\n");
            player1_sign = 'X';
            player2_sign = 'O';
        } else if (strcmp(command, "O") == 0) {
            printf("\nYou play with: O\n");
            player1_sign = 'O';
            player2_sign = 'X';
            draw_board();
            player_turn();
        } else if (strcmp(command, "win") == 0) {
            printf("Victory!\n");
            sleep(1);
            end_game_session();
        } else if (strcmp(command, "lose") == 0) {
            printf("Loss!\n");
            sleep(1);
            end_game_session();
        } else if (strcmp(command, "draw") == 0) {
            printf("Draw.\n");
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
    int move_no;
    do {
        printf("Chosen move: ");
        scanf("%d", &move_no);
    } while (!validate_player_move(move_no));

    player1_moves[player1_no_moves] = move_no;
    player1_no_moves++;

    draw_board();
    validate_game_status();

    char buffer[MAX_MESSAGE_LENGTH];
    sprintf(buffer, "player_turn %d", move_no);
    message_send(socket_server, buffer);

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
    char sign = get_game_result();

    if (sign != ' ') {
        message_send(socket_server, "win");
    }

    if (player1_no_moves + player2_no_moves == 9) {
        message_send(socket_server, "draw");
    }
}

char get_game_result() {
    char b[3][3];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            b[i][j] = ' ';
        }
    }

    for (int i = 0; i < player1_no_moves; i++) {
        int move_no = player1_moves[i] - 1;
        b[move_no / 3][move_no % 3] = player1_sign;
    }

    for (int i = 0; i < player2_no_moves; i++) {
        int move_no = player2_moves[i] - 1;
        b[move_no / 3][move_no % 3] = player2_sign;
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

void draw_board() {
    char b[3][3];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            b[i][j] = ' ';
        }
    }

    for (int i = 0; i < player1_no_moves; i++) {
        int move_no = player1_moves[i] - 1;
        b[move_no / 3][move_no % 3] = player1_sign;
    }

    for (int i = 0; i < player2_no_moves; i++) {
        int move_no = player2_moves[i] - 1;
        b[move_no / 3][move_no % 3] = player2_sign;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("|%c", b[i][j]);
        }
        printf("|\n");
    }
}
