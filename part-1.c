/*
 * file:        part-1.c
 * description: Part 1, CS5600 Project 1, 2025 SP
 */

/* THE ONLY INCLUDE FILE */
#include "sysdefs.h"

// Quick little definition specifing the buffer size. +1 for \0.
#define bufSize 201

/* write these functions */
// int read(int fd, void *ptr, int len);
// int write(int fd, void *ptr, int len);
// void exit(int err);

/* ---------- */

/* Factor, factor! Don't put all your code in main()! 
*/

// Wrapper that calls the exit system call with the error number.
// 	syscall function found in syscall.S.
// 	__NR_exit 60 (system call number for exit).
void exit(int err) {
	syscall(__NR_exit, err);
}
// Wrapper that calls the read system call with associated arguments.
// 	__NR_read 0 (system call number for read)
int read(int fd, void *ptr, int len) {
	return syscall(__NR_read, fd, ptr, len);
}

// Wrapper that calls the write system call with associated arguments.
// 	__NR_write 1 (system call number)
int write(int fd, void *ptr, int len) {
	return syscall(__NR_write, fd, ptr, len);
}

/* ----- Helper functions ----- */

/* Function that takes in a char pointer to a buffer and a buffer size to
 * print to the designated file descriptor, hard coded as STDOUT.
 *
 * Processing the buffer byte by byte and sending it to STDOUT by calling
 * the write wrapper defined above.		
 */
int myPrint(char *buffer, int bytesRead) {
	// counter for the number of byte written to STDOUT.
	int idx = 0;
	// pointer to interact with the buffer.
	char *s = buffer;
	// loop to go through the entire buffer by byte.
	while(idx < bytesRead) {
		// writing a byte to STDOUT
		write(1, s, 1);
		// updating buffer pointer and bytes written.
		s=s+1;
		idx++;
	}
	// End of buffer, append new line and null terminator.
	buffer[idx++] = '\n';
	buffer[idx] = '\0';
	// Write the prior two appends to STDOUT.
	write(1, s, 2);
	// return number of bytes written.
	return idx;
}

/* Function that take in a char point and a buffersize. Interacts with it
 * to read in line from a designated file descriptor. Hard coded as STDIN.
 *
 * Process the data from STDIN byte by byte and throws it into the buffer
 * using the read system call wrapper defined above.
 */
int myScan(char *buffer, int bufferSize) {
	/* Small buffer that holds the tiny string to be written to STDOUT
	 * to differentiate between user input and echoed output.
	 */
	char buf[2] = "> ";
	write(1,buf,2);
	// counter keeping track of how many bytes have been read.
	int idx = 0;
	// variable to store the byte read in and throw it into the buffer.
	char c = 's';
	// Buffer size is the defined 200. Upper bound of line length is 200. 
	while(idx < bufferSize) {
		// read 1 byte from STDIN, store it in c; 
		read(0, &c, 1);
		/* if new line, break from the loop.
		 * otherwise append c to the buffer.
		 */
		if(c == '\n') {
			break;
		} else {
			buffer[idx++] =  c;
		}
	}
	// end of data for buffer. Append null terminator.
	buffer[idx] = '\0';
	// return number of bytes read.
	return idx;
}

/* Function that checks the start of the buffer for 'quit'.
 * return 1 if start of buffer is quit, telling main to break from loop and
 *	exit
 * return 0 otherwise
 */
int checkQuit(char* buffer, int bytesRead) {
	// buffer size less than 4. impossible to type quit, return 0.
	if(bytesRead < 4) {
		return 0;
	}
	// some useful flag declarations.
	int qflag=0, uflag=0, iflag=0, tflag=0;
	for(int i = 0; i < 4; i++) {
		/* check char at this position in buffer, if matches
		 * set flag.
		 */
		if(buffer[i] == 'q' && i == 0){qflag = 1;}
		if(buffer[i] == 'u' && i == 1){uflag = 1;}
		if(buffer[i] == 'i' && i == 2){iflag = 1;}
		if(buffer[i] == 't' && i == 3){tflag = 1;}
	} 
	// all flags set? return 1.
	if(qflag && uflag && iflag && tflag){
		return 1;
	}
	return 0;
}
/* Function that is the 'brain'. Handles logic for reading, checking and
 * subsequently writing.
 * 
 * Creates buffer of buffer size, scans STDIN for input into buffer.
 * Checks buffer for "quit" and handles action based on result accordingly.
 * Write buffer to STDOUT.
 */
void controller() {
	char buf [bufSize];
	while(1) {
		int bytesRead = myScan(buf, bufSize);
		int toExit = checkQuit(buf, bytesRead);
		if(toExit) {
			break;
		}
		myPrint(buf, bytesRead);
	}
}

/* ----- Main Function ----- */

void main(void)
{
	controller();
	exit(0);
}
