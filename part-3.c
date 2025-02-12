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

/* ---------- */

/* write these 
*/
int read(int fd, void *ptr, int len);
int write(int fd, void *ptr, int len);
void exit(int err);
int open(char *path, int flags);
int close(int fd);
int lseek(int fd, int offset, int flag);
void *mmap(void *addr, int len, int prot, int flags, int fd, int offset);
int munmap(void *addr, int len);

/* ---------- */

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

/* ---------- */

/* write these new functions */
void do_yield12(void);
void do_yield21(void);
void do_uexit(void);

void do_yield12(void) {

}
void do_yield21(void) {

}

void do_uexit(void) {

}

/* ---------- */

void main(void)
{
	vector[1] = do_print;

	vector[3] = do_yield12;
	vector[4] = do_yield21;
	vector[5] = do_uexit;

	/* your code here */

	do_print("done\n");
	exit(0);
}
