/*
 * file:        part-1.c
 * description: Part 1, CS5600 Project 1, 2025 SP
 */

/* THE ONLY INCLUDE FILE */
#include "sysdefs.h"

#define bufSize 200
/* write these functions */

// int read(int fd, void *ptr, int len);
// int write(int fd, void *ptr, int len);
// void exit(int err);

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

int myPrint(char *buffer, int bytesRead) {
	int idx = 0;
	char *s = buffer;
	while(idx < bytesRead) {
		write(1, s, 1);
		s=s+1;
		idx++;
	}
	buffer[idx++] = '\n';
	buffer[idx] = '\0';
	write(1, s, 2);
	return idx;
}
int myScan(char *buffer, int bufferSize) {
	char buf[2] = "> ";
	write(1,buf,2);
	int idx = 0;
	char c = 's';
	int byteRead = 0;
	int qflag=0, uflag=0, iflag=0, tflag=0;

	while(idx < bufferSize) {
		byteRead = read(0, &c, 1);
		
		if(c == '\n') {
			buffer[idx] = '\0';
			break;
		} else {
			buffer[idx++] =  c;
			if(c == 'q' && idx == 1){qflag = 1;}
			if(c == 'u' && idx == 2){uflag = 1;}
			if(c == 'i' && idx == 3){iflag = 1;}
			if(c == 't' && idx == 4){tflag = 1;}
		}
	}
	if(qflag && uflag && iflag && tflag){
		exit(0);
	}
	return idx;
}

// helper functions
/* ---------- */

void main(void)
{
	char buf [bufSize];
	while(1) {
		int bytesRead = myScan(buf, bufSize);
		myPrint(buf, bytesRead);
	}
}
