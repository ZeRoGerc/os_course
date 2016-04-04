#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
	char buf[BUF_SIZE];
	
	int in = 0;
	if (argc > 1) {
		in = open(argv[1], O_RDONLY);
		if (in == -1) {
			perror(strerror(errno));
			return 1;
		}
	}

	int read_amount;
	while (1) {
		read_amount = read(in, buf, BUF_SIZE);
		if (read_amount == -1) {
			perror(strerror(errno));
			if (errno == EINTR) 
				continue;
			else
				return 1;
		}
		if (read_amount == 0) {
			break;
		}

		int write_amount = 0;
		while (write_amount < read_amount) {
			int temp = write(1, buf, read_amount);
			if (temp == -1) {
				perror(strerror(errno));
				if (errno == EINTR) {
					continue;
				} else {
					return 1;
				}
			}
			write_amount += temp;
		}
	}
	return 0;
}
