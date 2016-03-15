#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main() {
	char buf[BUF_SIZE];
	
	int read_amount;
	while ((read_amount = read(0, buf, BUF_SIZE)) > 0) {
		int write_amount = 0;
		while (write_amount < read_amount) {
			int temp = write(1, buf, read_amount);
			if (temp < 0) {
				write(2, "An error occured in the write\n", 31);
				return 1;
			}
			write_amount += temp;
		}
	}

	if (read_amount < 0) {
		write(2, "An error occured in the read\n", 30);
		return 1;
	}
	return 0;
}
