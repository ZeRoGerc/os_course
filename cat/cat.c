#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUF_SIZE 8

int main() {
	char buf[BUF_SIZE];
	
	int readed;
	while ((readed = read(0, buf, BUF_SIZE)) > 0) {
		write(1, buf, readed);
	}

	if (readed < 0) {
		write(2, "An error occured in the read\n", 30);
	}
}
