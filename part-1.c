/*
 * file:        part-1.c
 * description: Part 1, CS5600 Project 1, 2025 SP
 */

/* THE ONLY INCLUDE FILE */
#include "sysdefs.h"

/* write these functions */

int read(int fd, void *ptr, int len);
int write(int fd, void *ptr, int len);
void exit(int err);
int getBufferSize(char *buffer); 
/* ---------- */

/* Factor, factor! Don't put all your code in main()! 
*/

// __NR_exit 60 (system call number)
void exit(int err) {
	syscall(__NR_exit, err);
}

/* read one line from stdin (file descriptor 0) into a buffer: */
// __NR_read 0
int read(int fd, void *ptr, int len) {
	return syscall(__NR_read, fd, ptr, len);
}

/* print a string to stdout (file descriptor 1) */
// __NR_write 1 (system call number)
int write(int fd, void *ptr, int len) {
	return syscall(__NR_write, fd, ptr, len);
}

int myPrint(void *buffer) {
	int std_out = 1;
	return write(std_out, buffer, getBufferSize(buffer));
}

int myScan(char *buffer) {
	int std_in = 0;
	return read(std_in, buffer, getBufferSize(buffer));
}

// helper functions
int getBufferSize(char *buffer) {
	int size = 0;
	while(1) {
		if(buffer[size] == '\0') {
			break;
		}
		size ++;
	}
	return size;
}
/* ---------- */

void main(void)
{
	char buffer_1 [] = "hello this is my buffer\n";
	myPrint(buffer_1);
	char buffer_2 [200];
	myScan(buffer_2);
	myPrint(buffer_2);
	exit(0);
}
