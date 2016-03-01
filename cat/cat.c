#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main() {
	char buf[BUF_SIZE];
	
	int readed;
	while ((readed = read(0, buf, BUF_SIZE)) > 0) {
		int writed = 0;
		while (writed < readed) {
			int temp = write(1, buf, readed);
			if (temp < 0) {
				write(2, "An error occured in the write\n", 31);
				return 0;
			}
			writed += temp;
		}
	}

	if (readed < 0) {
		write(2, "An error occured in the read\n", 30);
	}
}
