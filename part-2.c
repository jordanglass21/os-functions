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

int open(char *path, int flags);
int close(int fd);
int lseek(int fd, int offset, int flag);
void *mmap(void *addr, int len, int prot, int flags, int fd, int offset);
int munmap(void *addr, int len);

/**
 * Function wrapper for the exit system call.
 * 
 * Utiliizes __NR_exit, which is defined as system call number 60 for exit.
 * 
 * @param err Error code for exiting
 */
void exit(int err) {
        syscall(__NR_exit, err);
}
/**
 * Function wrapper for the read system call.
 * 
 * Utiliizes __NR_read, which is defined as system call number 0 for read.
 * 
 * @param fd    File descriptor to read data from.
 * @param *ptr  Pointer to location where the data read from fd is stored
 * @param len   Number of bytes of data to read
 */
int read(int fd, void *ptr, int len) {
        return syscall(__NR_read, fd, ptr, len);
}

/**
 * Function wrapper for the write system call.
 * 
 * Utiliizes __NR_write, which is defined as system call number 1 for write.
 * 
 * @param fd    File descriptor to write data to
 * @param *ptr  Pointer to location where the data is read and written to fd
 * @param len   Number of bytes of data to write
 */
int write(int fd, void *ptr, int len) {
        return syscall(__NR_write, fd, ptr, len);
}

/**
 * Function wrapper for the open system call.
 * 
 * Utiliizes __NR_open, which is defined as system call number 2 for open.
 * 
 * @param *path         Path to the file to open.
 * @param flags         Flags to determine permissions fo the file opened.
 */
int open(char *path, int flags) {
	return syscall(__NR_open, path, flags);
}

/**
 * Function wrapper for the close system call.
 * 
 * Utiliizes __NR_close, which is defined as system call number 3 for close.
 * 
 * @param fd    File descriptor to close.
 */
int close(int fd) {
	return syscall(__NR_close, fd); 
}

/**
 * Function wrapper for the lseek system call.
 * 
 * Utiliizes __NR_lseek, which is defined as system call number 8 for exit.
 * 
 * @param fd            File descriptor to operate on.        
 * @param offset        The offset for the file in question.
 * @param flag          Flags on how operate on the offset
 *                              (SEEK_SET, SEEK_CUR, SEEK_END)
 */
int lseek(int fd, int offset, int flag) {
	return syscall(__NR_lseek, fd, offset, flag);
}

/**
 * Function wrapper for the mmap system call.
 * Utiliizes __NR_mmap, which is defined as system call number 9 for mmap.
 * 
 * @param *addr         A hint for mmap on where to allocate the memory region
 * @param len           How big is the memory region to be created
 * @param prot          Permissions on the memory region (R/W/X)
 * @param flags         How is the map created (MAP_PRIVATE/MAP_ANONYMOUS)
 * @param fd            File descriptor to create the mapping from
 * @param offset        Where in the file to start the mapping from
 */
void *mmap(void *addr, int len, int prot, int flags, int fd, int offset) {
	return (void *)syscall(__NR_mmap, addr, len, prot, flags, fd, offset);
}

/**
 * Function wrapper for the munmap system call.
 * Utiliizes __NR_munmap, which is defined as system call number 11 for munmap.
 * 
 * @param *addr         Pointer to location where the memory is mapped to
 * @param len           Size of the memory location to unmap
 */
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

 /**
  * Function that reads up to len-1 bytes into buf. It then adds a terminating zero byte at the end. 
  * Stops reading into buf when either:
  *     (a) the last character read was '\n'
  *     (b) len-1 bytes were read
  * 
  * @param *buf         Pointer to buffer that data is to be read into
  * @param len          Maximum bytes to read
  */
void do_readline(char *buf, int len) {
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

 /**
  * Writes the null-terminating string to standard output. 
  * Going through each byte of buf and writing to STDOUT before stopping at the zero 
  * byte that terminates the string.
  * 
  * @param *buf         Pointer to buffer that data is to be read from
  *                             to print to STDOUT.
  */
void do_print(char *buf) {
        // loop to go through the entire buffer by byte.
        while(*buf != '\0') {
                // writing a byte to STDOUT
                write(1, buf, 1);
                // updating buffer pointer and bytes written.
                buf++;
        }
	char newline = '\0';
	write(1, &newline, 1);
}
/**
 * Function that returns argument i, or 0 if there weren't that many arguments
 * from a global argv variable
 * 
 * @param i the i-th index in the argv array to fetch an element from
 */
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

 /**
  * The guts of part 2
  * 
  * This is where we read in the elf header if valid file found.
  *     Then we loop through the sections, if the section type is PT_LOAD:
  *     We create a mmap region and read from the file into this region
  *     Then we create a function pointer to hdr.e_entry and call it
  *     munmap the mmap'ed memory regions after exiting to prevent crashing 
  *     via out command loop the second iteration through.
  * 
  * @param *filename    The file that we want to open, load into memory
  *                             and execute.
  */
void load_file(char *filename) {
        int fd;
        // Trying to open file, printing error and returning if file not found
        if((fd = open(filename, O_RDONLY)) < 0) {
                do_print("Unable to open file specified.\n");
                return;
        }
        struct elf64_ehdr hdr;
        // reading elf header into struct above
        read(fd, &hdr, sizeof(hdr));
        int i, n = hdr.e_phnum;
        struct elf64_phdr phdrs[n];
        // seeking executable for pgrm hdrs via offset found in elf header.
        lseek(fd, hdr.e_phoff, SEEK_SET);
        // read pgrm headers into struct phdrs[n]
        read(fd, phdrs, sizeof(phdrs));
        // Defining a valid offset
        int offset = 0x8000000;
        // Array that stores the mapped region pointers so that we can munmap
        void* mappedRegions[hdr.e_phnum];
        // Counter for how many regions we have mapped
        int mapped = 0;
        for(i = 0; i < hdr.e_phnum; i++) {
                // Is program header type of PT_LOAD
                if(phdrs[i].p_type == PT_LOAD) {
                        /* Round up the memory size to the nearest multiple
                                of 4096 for paging*/
                        int len = ROUND_UP(phdrs[i].p_memsz, 4096);
                        /* Rounding down vaddr so that start of mmap hint
                                matches page size multiples*/
                        uint64_t addr = ROUND_DOWN((uint64_t)(phdrs[i].p_vaddr), 4096);
                        // Region created by mmap located at addr + offset
                        void *region = mmap(
					(void *)addr+offset,
					len,
					PROT_READ | PROT_WRITE | PROT_EXEC,
					MAP_PRIVATE | MAP_ANONYMOUS,
					-1,
					0);
                        /* Did the map fail? Handling if it did by printing
                                mmap failed and exiting with error code 1.*/
			if(region == MAP_FAILED) {
				do_print("mmap failed\n");
				exit(1);
			}
                        /* It didn't fail. Yay! Throw it in array to keep track
                                for munmap*/
			mappedRegions[mapped++] = region;
                        // Seek to the phdr location in the elf file
                        lseek(fd, (int)phdrs[i].p_offset, SEEK_SET);
                        /* Read the information from the elf file into
                                mmap'ed region */
                        read(fd, region, (int)phdrs[i].p_filesz);
                }
        }
        // close the fd
        close(fd);
        // create the function call to the entry point
        void (*f)();
        f = hdr.e_entry + offset;
        // call the entry point!
        f();
        // unmap the mapped regions.
        for(int j = 0; j < mapped; j++) {
                munmap(mappedRegions[j], 4096);
        }
}
/* your code here */

/** 
 * Function that checks the start of the buffer for 'quit'.
 * return 1 if start of buffer is quit, telling main to break from loop and
 *      exit
 * return 0 otherwise
 * 
 * @param *buffer       User entered buffer to see if 'quit' is entered
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
/**
 * Function that handles the command loop
 */
void controller() {
	do_print("Hello, this program simulates a shell\n");
	do_print("The supported commands are wait, hello, ugrep and quit\n");
	while(1) {
                char buf[bufSize];
                /* Small buffer that holds the tiny string to be written to STDOUT
                * to differentiate between user input and echoed output.
                */
                write(1, "> ", 2);
                
                // read a line of input
		do_readline(buf, bufSize); 

		// split it into words
		ARGC = split(ARGV, MAX_ARGS, buf);		

		// exit if first word is quit
		if(checkQuit(ARGV[0])){// if the first word is quit...
			break;
                }

		// load and execute the file named by the first word into memory
		load_file(do_getarg(0));

		// call the loaded programs's entry point
		//for(int i = 0; i < argc; i++) {
       	 	//	do_print(argv[i]);// this should be load into the program
		//}
		// repeat
	}	
}

/**
 * Entry point of this program. Holds important vector table declarations
 *              and jumpstarts the command loop
 * Calls exit when the command loop breaks from user input
 */
void main(void)
{
	vector[0] = do_readline;
	vector[1] = do_print;
	vector[2] = do_getarg;

	controller();
	
	exit(0);
}

