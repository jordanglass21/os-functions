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

#define MAX_ARGS 10

/* ---------- */
int ARGC = -1;
char *ARGV[MAX_ARGS];

/* write these functions 
*/
int read(int fd, void *ptr, int len); // we did this in part 1?
int write(int fd, void *ptr, int len); // we did this in part 1?
void exit(int err); // we did this i part 1?
int split(char **argv, int max_argc, char *line);

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
	return syscall(__NR_lseek, fd, offset, flag);
}

void *mmap(void *addr, int len, int prot, int flags, int fd, int offset) {
	return (void *)syscall(__NR_mmap, addr, len, prot, flags, fd, offset);
}

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
        write(1, "> ", 2);
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
                        buf[idx++] =  c;
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
	char newline = '\0';
	write(1, &newline, 1);
}

// char *getarg(int i) - returns argument i, or 0 if there weren't that many arguments.
char *do_getarg(int i) {
	// return the arument if i is in bounds
	if(i >= 0 && i < ARGC) {
        	return ARGV[i];
        }

	// else return 0
        return 0;
}
/* ---------- */

/* the guts of part 2
 *   read the ELF header
 *   for each section, if b_type == PT_LOAD:
 *     create mmap region
 *     read from file into region
 *   function call to hdr.e_entry
 *   munmap each mmap'ed region so we don't crash the 2nd time
 */
void load_file(char *filename) {
        int fd;
        if((fd = open(filename, O_RDONLY)) < 0) {
                do_print("Unable to open file specified.\n");
                return;
        }
        struct elf64_ehdr hdr;
        read(fd, &hdr, sizeof(hdr));
        int i, n = hdr.e_phnum;
        struct elf64_phdr phdrs[n];
        lseek(fd, hdr.e_phoff, SEEK_SET);
        read(fd, phdrs, sizeof(phdrs));
        int offset = 0x8000000;
        void* mappedRegions[hdr.e_phnum];
        int mapped = 0;
        for(i = 0; i < hdr.e_phnum; i++) {
                if(phdrs[i].p_type == PT_LOAD) {
                        int len = ROUND_UP(phdrs[i].p_memsz, 4096);
                        uint64_t addr = ROUND_DOWN((uint64_t)(phdrs[i].p_vaddr), 4096);
                        void *region = mmap(
					(void *)addr+offset,
					len,
					PROT_READ | PROT_WRITE | PROT_EXEC,
					MAP_PRIVATE | MAP_ANONYMOUS,
					-1,
					0);
			if(region == MAP_FAILED) {
				do_print("mmap failed\n");
				exit(1);
			}
			mappedRegions[mapped++] = region;
                        lseek(fd, (int)phdrs[i].p_offset, SEEK_SET);
                        read(fd, region, (int)phdrs[i].p_filesz);
                }
        }
        close(fd);
        void (*f)();
        f = hdr.e_entry + offset;
        f();
        for(int j = 0; j < mapped; j++) {
                munmap(mappedRegions[j], 4096);
        }
}
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
	do_print("Hello, this program simulates a shell\n");
	do_print("The supported commands are wait, hello, ugrep and quit\n");
	while(1) {
		// read a line of input
		char buf[bufSize];

		do_readline(buf, bufSize); 

		// split it into words
		ARGC = split(ARGV, MAX_ARGS, buf);		

		// exit if first word is quit
		if(checkQuit(ARGV[0])){// if the first word is quit...
			break;
                }

		// load the file named by the first word into memory
		load_file(do_getarg(0));
		// provide "system call" for the loaded program

		// call the loaded programs's entry point
		//for(int i = 0; i < argc; i++) {
       	 	//	do_print(argv[i]);// this should be load into the program
		//}
		// repeat
	}	
}

void main(void)
{
	vector[0] = do_readline;
	vector[1] = do_print;
	vector[2] = do_getarg;

	controller();
	
	exit(0);
}

