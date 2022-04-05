#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int main(int argc, char** argv) {

	if (strcmp(argv[1], "ignore") == 0) {
		raise(SIGUSR1);
		printf("IGNORED - SIGUSR1 in child\n");
	}
    if (strcmp(argv[1], "mask") == 0) {
		raise(SIGUSR1);
		printf("MASK - SIGUSR1 in child\n");
	}
	if (strcmp(argv[1], "pending") == 0) {
		sigset_t c_signals;
		sigpending(&c_signals);
		printf("PENDING - SIGUSR1 in child: %s\n", sigismember(&c_signals, SIGUSR1) ? "yes" : "no");
	}

	return 0;
}