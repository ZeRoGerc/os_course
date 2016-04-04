#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int received = 0;

void sig_handler(int signum, siginfo_t *info, void *context) 
{
	received = 1;
	if (signum == SIGUSR1) {
		printf("SIGUSR1 from %ld", (long)info->si_pid);
	} else if (signum == SIGUSR2) {
		printf("SIGUSR2 from %ld", (long)info->si_pid);
	}
}

int main()
{
	struct sigaction act;
	memset(&act, '\0', sizeof(act));
	act.sa_sigaction = &sig_handler;
	act.sa_flags = SA_SIGINFO;

	if ((sigaction(SIGUSR1, &act, NULL) < 0) || (sigaction(SIGUSR2, &act, NULL) < 0)) 
	{
		perror("sigaction failed");
		return 1;
	} else {
		sleep(10);
		if (received == 0) {
			printf("No signals were caught");
		}
	}
}
