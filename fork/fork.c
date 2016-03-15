#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, const char** argv) {
	int pipe_fd[2];
	if (pipe(pipe_fd)) {
		perror("Can't create pipe\n");
		return 1;
	}

	pid_t id_cat = fork();

	if (id_cat == 0) {
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		execlp("cat", "cat", argv[1], NULL);
	} else {
		pid_t id_grep = fork();
		if (id_grep == 0) {
			dup2(pipe_fd[0], STDIN_FILENO);
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			execlp("grep", "grep", "int", NULL);
		} else {
			int res;
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			waitpid(id_cat, &res, 0);
			waitpid(id_grep, &res, 0);
		}
	}
	return 0;
}
