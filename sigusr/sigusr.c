#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

volatile int received = 0;

void sig_handler(int signum, siginfo_t *info, void *context) 
{
	received = 1;
	printf("%d from %d", signum, info->si_pid);
}

int main()
{
	struct sigaction act;
	sigfillset(&act.sa_mask);
	sigdelset(&act.sa_mask, SIGSTOP);
	sigdelset(&act.sa_mask, SIGKILL);
	act.sa_sigaction = &sig_handler;
	act.sa_flags = SA_SIGINFO;

	for (int i = 1; i <= 31; i++) {
		if (i == SIGSTOP || i == SIGKILL) {
			continue;
		}
		if ((sigaction(i, &act, NULL) < 0)) {
			perror("sigaction failed");
			return errno;
		}
	}

	sleep(10);
	if (received == 0) {
		printf("No signals were caught");
	}
}
