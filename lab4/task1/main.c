#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <wait.h>

void handler(int sigint);

int main(int argc, char** argv) {
	
    if (argc != 3) {
		printf("Usage: main 'ignore|handler|mask|pending' 'fork|exec'\n");
		exit(1);
	}

    if (strcmp(argv[1], "ignore") == 0) {
		signal(SIGUSR1, SIG_IGN);
		raise(SIGUSR1);
	} else if (strcmp(argv[1], "handler") == 0) {
		signal(SIGUSR1, handler);
		raise(SIGUSR1);
	} else if (strcmp(argv[1], "mask") == 0) {
		sigset_t blocked_signals;
		sigemptyset(&blocked_signals);
		sigaddset(&blocked_signals, SIGUSR1);
		sigprocmask(SIG_BLOCK, &blocked_signals, NULL);
		raise(SIGUSR1);
	} else if (strcmp(argv[1], "pending") == 0) {
        sigset_t blocked_signals;
		sigemptyset(&blocked_signals);
		sigaddset(&blocked_signals, SIGUSR1);
		sigprocmask(SIG_BLOCK, &blocked_signals, NULL);
		raise(SIGUSR1);

		sigset_t c_signals;
		sigpending(&c_signals);
		printf("PENDING - SIGUSR1 in parent: %s\n", sigismember(&c_signals, SIGUSR1) ? "yes" : "no");

		// if (strcmp(argv[1], "pending") == 0) {
		// 	sigset_t c_signals;
		// 	sigpending(&c_signals);
		// 	printf("PENDING - SIGUSR1 in parent: %s\n", sigismember(&c_signals, SIGUSR1) ? "yes" : "no");
		// }
    }

    if (strcmp(argv[2], "exec") == 0) {
        execl("./test_child", "./test_child", argv[1], NULL);
    }

	if (strcmp(argv[2], "fork") == 0) {
		pid_t child = fork();
		if (child == 0) {
            if (strcmp(argv[1], "ignore") == 0) {
		        raise(SIGUSR1);
				printf("IGNORED - SIGUSR1 in child\n");
	        }
            else if (strcmp(argv[1], "handler") == 0) {
		        raise(SIGUSR1);
	        }
            else if (strcmp(argv[1], "mask") == 0) {
		        raise(SIGUSR1);
                printf("MASK - SIGUSR1 in child\n");
            }
            else if (strcmp(argv[1], "pending") == 0) {
			    sigset_t c_signals;
			    sigpending(&c_signals);
			    printf("PENDING - SIGUSR1 in child: %s\n", sigismember(&c_signals, SIGUSR1) ? "yes" : "no");
		    }
		} else if (child > 0) {
			wait(0);
		}

	}

	return 0;
}

void handler(int sigint) {
    printf("Received: SIGUSR1\n");
}