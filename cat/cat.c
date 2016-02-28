#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main() {
	char[256] buf;
	int readed = read(0, buf, 256);

	if (readed < 0) {
		write(2, "An error occured in the read\n", 30);
	} else {
		write(1, buf, readed);
	}

	exit(0);
}
