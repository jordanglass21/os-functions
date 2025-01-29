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

/* ---------- */

/* Factor, factor! Don't put all your code in main()! 
*/

// __NR_exit 60 (system call number)
void exit(int err) {
	// flush buffer data
	
	// close all open files

	// remove temporary files

	// return statusi
	syscall(__NR_exit);

	//if(err) {// exit failure
	//	return 1;
	//}
	//return 0; // exit success
}

/* read one line from stdin (file descriptor 0) into a buffer: */
// __NR_read 0
int read(int fd, void *ptr, int len) {
	return 1;
}

/* print a string to stdout (file descriptor 1) */
// __NR_write 1 (system call number)
int write(int fd, void *ptr, int len) {
	return 1;
}
/* ---------- */

void main(void)
{
	exit(0);
}
