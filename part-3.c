/*
 * file:        part-3.c
 * description: part 3, CS5600 Project 1, 2025 SP
 */

/* NO OTHER INCLUDE FILES */
#include "elf64.h"
#include "sysdefs.h"

extern void *vector[];
extern void switch_to(void **location_for_old_sp, void *new_value);
extern void *setup_stack0(void *_stack, void *func);


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


/* copy from Part 2 */
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
  * The guts of part 2
  * 
  * This is where we read in the elf header if valid file found.
  *     Then we loop through the sections, if the section type is PT_LOAD:
  *     We create a mmap region and read from the file into this region
  *     We add the offset to the entry point return the pointer
  * 
  * @param *filename    The file that we want to open, load into memory
  *                             and execute.
  * @param offset       The offset that we specify as to where to load
  *                             the programs
  */
 void* load_file(char *filename, int offset) {
        int fd;
        // Trying to open file, printing error and returning if file not found
        if((fd = open(filename, O_RDONLY)) < 0) {
                do_print("Unable to open file specified.\n");
                return NULL;
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
        // Counter for how many regions we have mapped
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
        return hdr.e_entry + offset;
}

/* ---------- */
/* Global variables used for context switching */
void *STACK1 = NULL;
void *STACK2 = NULL;
void *STACK1_PTR = NULL;
void *STACK2_PTR = NULL;
void *MAIN_PTR = NULL;

/**
 * Function that calls the switch_to function that switches the execution
 * stack from one process to another. From process 1 to process 2
 */
void do_yield12(void) {
	switch_to(&STACK1_PTR, STACK2_PTR);
}
/**
 * Function that calls the switch_to function that switches the execution
 * stack from one process to another. From process 2 to process 1
 */
void do_yield21(void) {
	switch_to(&STACK2_PTR, STACK1_PTR);
}
/**
 * Function that is called by a process. "exiting" the process.
 * Switches the execution stack back to the "OS" stack, or in
 * this case, the part-3.c stack.
 */
void do_uexit(void) {
	switch_to(NULL, MAIN_PTR);
}

/* ---------- */

/**
 * Controller function such that we don't throw everything in main.
 * 
 * Memory allocation with mmap, setting up the stack for the two processes
 * through load_file() and jump starting process 1 happens here
 */
void controller() {
        STACK1 = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	STACK2 = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	STACK1_PTR = setup_stack0(STACK1+4096, load_file("process1", 0x800000));
	STACK2_PTR = setup_stack0(STACK2+4096, load_file("process2", 0x805000));
        switch_to(&MAIN_PTR, STACK1_PTR);
}

/**
 * Entry point of this program. Holds important vector table declarations
 *              and jumpstarts the command loop
 * Prints "done" and Calls exit when the command loop breaks from user input
 */
void main(void)
{
	vector[1] = do_print;
	vector[3] = do_yield12;
	vector[4] = do_yield21;
	vector[5] = do_uexit;
	
	controller();

	do_print("done\n");
	exit(0);
}
