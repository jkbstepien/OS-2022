#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Incorrect number of arguments passed. Expected 2.\n");
        exit(0);
    }

    int n = atoi(argv[1]);

    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            printf("Output comes from process with PID = %d\n", (int)getpid());
            exit(0);
        }
    }

    return 0;
}