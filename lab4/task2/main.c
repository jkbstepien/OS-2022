#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

void handler(int sigint, siginfo_t *info, void *context);

int main(void) {
  
  printf("1st TEST (SIGUSR1, SIGUSR2):\n");
  struct sigaction first_action;
  first_action.sa_flags = SA_SIGINFO;
  first_action.sa_sigaction = handler;
  sigemptyset(&first_action.sa_mask);

  if (sigaction(SIGUSR1, &first_action, NULL) == -1) {
    fprintf(stderr, "No signal handled!\n");
    exit(1);
  }
  if (sigaction(SIGUSR2, &first_action, NULL) == -1) {
    fprintf(stderr, "No signal handled!\n");
    exit(1);
  }

  raise(SIGUSR1);
  raise(SIGUSR2);

  printf("2nd TEST (SIGALARM):\n");
  struct sigaction second_action;
  second_action.sa_flags = SA_SIGINFO;
  second_action.sa_sigaction = handler;
  sigemptyset(&second_action.sa_mask);
  if (sigaction(SIGALRM, &second_action, NULL) < 0) {
      fprintf(stderr, "Couldn't catch SIGALRM!");
  }
  alarm(2);
  pause();

  printf("3rd TEST (SIGCHLD):\n");
  struct sigaction action;
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = handler;
  sigemptyset(&action.sa_mask);

  if (sigaction(SIGCHLD, &action, NULL) < 0) {
    fprintf(stderr, "Couldn't catch SIGCHLD!");
  }
  if (fork() == 0) {
    exit(1);
  }
  wait(NULL);

  return 0;
}

void handler(int sigint, siginfo_t *data, void *context) {
    printf("\tReceived signal number %d from a process (PID %d)\n", data -> si_signo, (int) data -> si_pid);
    printf("\t\tUID of sending process: %d\n\t\tErrno: %d\n\n", (int) data -> si_uid, (int) data -> si_errno);
}