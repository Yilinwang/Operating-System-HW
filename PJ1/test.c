#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
int main() {
	syscall(349); //show
	printf("multiply(3, 4) = %ld\n", syscall(350, 3, 4)); //multiply
	printf("min(4, 3) = %ld\n", syscall(351, 4, 3)); //min
	syscall(352); //bonus: print the process status
	return 0;
}
