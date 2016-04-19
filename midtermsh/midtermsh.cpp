#include<stdio.h>
#include<string>
#include<vector>
#include<unistd.h>
#include<iostream>
#include<sys/wait.h>
#include<errno.h>
#include<string.h>
#include<signal.h>

#define BUF_SIZE 1024

using namespace std;

vector<int*> pipes;
vector<int> processes;

void sigint_handler(int signum, siginfo_t *info, void *context) {
	for (int p : processes) {
		kill(p, SIGINT);
	}
	exit(1);
}

vector<string> get_tokens(string const& command) {
	vector<string> result;
	int i = 0;
	string current;
	while (i < command.size()) {
		current.clear();
		while (i < command.size() && command[i] != ' ' && command[i] != '\n') {
			current += command[i++];
		}
		if (current.size() != 0) {
			result.push_back(current);
		}
		i++;
	}
	return result;
}

void exec(vector<string> args) {
	char* argv[args.size() + 1];
	for (int i = 0; i < args.size(); i++) {
		argv[i] = const_cast<char*>(args[i].c_str());
	}
	argv[args.size()] = NULL;
	execvp(argv[0], argv);
}

void exec_command(string command, bool first = false) {
	int pipe_fd[2];
	if (pipe(pipe_fd)) {
		perror("Can't open pipe\n");
		exit(1);
	}

	size_t pos = command.find('|');
	string newcommand;
	if (pos != string::npos) {
		newcommand = command.substr(pos + 1, command.size());
		command = command.substr(0, pos);
	}
	vector<string> args = get_tokens(command);
	
	pid_t pid = fork();

	if (pid == 0) {
		if (!first) {
			int* out_pipe = pipes.back();
			dup2(out_pipe[0], STDIN_FILENO);
			close(out_pipe[0]);
		}
		if (pos != string::npos) {
			dup2(pipe_fd[1], STDOUT_FILENO);
			close(pipe_fd[1]);
		}
		processes.push_back(getpid());
		exec(args);
	} else {
		int res;
		processes.push_back(getpid());
		close(pipe_fd[1]);
		if (pos != string::npos) {
			pipes.push_back(pipe_fd);
			exec_command(newcommand);
		}
		close(pipe_fd[0]);
		waitpid(pid, &res, 0);
	}
}

int main() {
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = &sigint_handler;
	act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGINT, &act, NULL) < 0) {
		perror("sigaction failed");
		return errno;
	}

	vector<char> buf(BUF_SIZE);
	string command;

	write(1, "$\n", 2);
	while (true) {
		command.clear();
		int read_amount = 0;
		read_amount = read(0, buf.data(), BUF_SIZE);
		if (read_amount == 0) {
			exit(1);
		}
		while (read_amount > 0) {
			for (int i = 0; i < read_amount; i++) {
				command += buf[i];
			}
			if (command[command.size() - 1] == '\n') {
				break;
			}
			read_amount = read(0, buf.data(), BUF_SIZE);
		}
		exec_command(command, true);
		pipes.clear();
		write(1, "$\n", 2);
	}
}
