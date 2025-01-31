/*
 * file:        part-2.c
 * description: Part 2, CS5600 Project 1, 2025 SP
 */

/* NO OTHER INCLUDE FILES */
#include "elf64.h"
#include "sysdefs.h"
extern void *vector[];

// Quick little definition specifing the buffer size. +1 for \0.
#define bufSize 201

/* ---------- */

/* write these functions 
*/
int read(int fd, void *ptr, int len); // we did this in part 1?
int write(int fd, void *ptr, int len); // we did this in part 1?
void exit(int err); // we did this i part 1?

// __NR_open
int open(char *path, int flags);
// __NR_close
int close(int fd);
// __NR_lseek
int lseek(int fd, int offset, int flag);
// __NR_mmap
void *mmap(void *addr, int len, int prot, int flags, int fd, int offset);
// __NR_munmap
int munmap(void *addr, int len);

// Wrapper that calls the exit system call with the error number.
//      syscall function found in syscall.S.
//      __NR_exit 60 (system call number for exit).
void exit(int err) {
        syscall(__NR_exit, err);
}
// Wrapper that calls the read system call with associated arguments.
//      __NR_read 0 (system call number for read)
int read(int fd, void *ptr, int len) {
        return syscall(__NR_read, fd, ptr, len);
}

// Wrapper that calls the write system call with associated arguments.
//      __NR_write 1 (system call number)
int write(int fd, void *ptr, int len) {
        return syscall(__NR_write, fd, ptr, len);
}

int open(char *path, int flags) {
	return syscall(__NR_open, path, flags);
}

int close(int fd) {
	return syscall(__NR_close, fd); 
}

int lseek(int fd, int offset, int flag) {
	return syscall(__NR_lseek, offset, flag);
}

//void *mmap(void *addr, int len, int prot, int flags, int fd, int offset) {
	//return *syscall(__NR_mmap, addr, len, flags, fd, offset);
//}

int munmap(void *addr, int len) {
	return syscall(__NR_munmap, addr, len);
}

/* ---------- */

/* the three 'system call' functions - readline, print, getarg 
 * hints: 
 *  - read() or write() one byte at a time. It's OK to be slow.
 *  - stdin is file desc. 0, stdout is file descriptor 1
 *  - use global variables for getarg
 */
// they gave us this function signature dont change it
void do_readline(char *buf, int len) {
        /* Small buffer that holds the tiny string to be written to STDOUT
         * to differentiate between user input and echoed output.
         */
        char carrot[2] = "> ";
        write(1,carrot,2);
        // counter keeping track of how many bytes have been read.
        int idx = 0;
        // variable to store the byte read in and throw it into the buffer.
        char c = 's';
        // Buffer size is the defined 200. Upper bound of line length is 200.
        while(idx < len) {
                // read 1 byte from STDIN, store it in c;
                read(__NR_read, &c, 1);
                /* if new line, break from the loop.
                 * otherwise append c to the buffer.
                 */
                if(c == '\n') {
                        break;
                } else {
                        buf[idx++] =  c;
                }
        }
        // end of data for buffer. Append null terminator.
        buf[idx] = '\0';
}

/* Function that takes in a char pointer to a buffer and a buffer size to
 * print to the designated file descriptor, hard coded as STDOUT.
 *
 * Processing the buffer byte by byte and sending it to STDOUT by calling
 * the write wrapper defined above.
 */
// they gave us this function signature dont change
void do_print(char *buffer) {
        // loop to go through the entire buffer by byte.
        while(*buffer != '\0') {
                // writing a byte to STDOUT
                write(1, buffer, 1);
                // updating buffer pointer and bytes written.
                buffer++;
        }
	char newline = '\n';
	write(1, &newline, 1);
}

// they gave us this function signature, needs to be implemnted
//char *do_getarg(int i);         

/* ---------- */

/* the guts of part 2
 *   read the ELF header
 *   for each section, if b_type == PT_LOAD:
 *     create mmap region
 *     read from file into region
 *   function call to hdr.e_entry
 *   munmap each mmap'ed region so we don't crash the 2nd time
 */

/* your code here */
/* Function that checks the start of the buffer for 'quit'.
 * return 1 if start of buffer is quit, telling main to break from loop and
 *      exit
 * return 0 otherwise
 */
int checkQuit(char* buffer) {
        // some useful flag declarations.
        int qflag=0, uflag=0, iflag=0, tflag=0, i = 0;
        while(*buffer != '\0') {
                /* check char at this position in buffer, if matches
                 * set flag.
                 */
                if(*buffer == 'q' && i == 0){qflag = 1;}
                if(*buffer == 'u' && i == 1){uflag = 1;}
                if(*buffer == 'i' && i == 2){iflag = 1;}
                if(*buffer == 't' && i == 3){tflag = 1;}
		i++;
		buffer++;
        }

        // all flags set? return 1, 0 otherwise
        return qflag && uflag && iflag && tflag;
}
/* ---------- */

/* simple function to split a line:
 *   char buffer[200];
 *   <read line into 'buffer'>
 *   char *argv[10];
 *   int argc = split(argv, 10, buffer);
 *   ... pointers to words are in argv[0], ... argv[argc-1]
 */
// they gave us this function dont change
int split(char **argv, int max_argc, char *line)
{
	int i = 0;
	char *p = line;

	while (i < max_argc) {
		while (*p != 0 && (*p == ' ' || *p == '\t' || *p == '\n'))
			*p++ = 0;
		if (*p == 0)
			return i;
		argv[i++] = p;
		while (*p != 0 && *p != ' ' && *p != '\t' && *p != '\n')
			p++;
	}
	return i;
}

/* ---------- */

void controller() {
	do_print("Hello, this program simulates a shell");
	do_print("The supported commands are wait, hello, ugrep and quit");
	while(1) {
		char buf[bufSize];
		do_readline(buf, bufSize);
		if(checkQuit(buf)){
                        break;
                }
       	 	do_print(buf);// this should be load in the program
	}	
}

void main(void)
{
	//this was given to us, might be useful
	//vector[0] = do_readline;
	//vector[1] = do_print;
	//vector[2] = do_getarg;

	controller();
	/* YOUR CODE HERE */
	exit(0);
}

