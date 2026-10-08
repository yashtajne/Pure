#ifndef LinuxKernel_h
#define LinuxKernel_h

#include "../../Pure.h"
typedef unsigned long linux_ulong_t;

#if !defined(__x86_64__)
#error "The Linux syscall layer currently supports x86-64 only."
#endif


#define LINUX_AT_FDCWD (-100)
#define LINUX_AT_SYMLINK_NOFOLLOW 0x100
#define LINUX_O_RDONLY    0
#define LINUX_O_WRONLY    1
#define LINUX_O_RDWR      2
#define LINUX_O_CREAT     64
#define LINUX_O_EXCL      128
#define LINUX_O_TRUNC     512
#define LINUX_O_APPEND    1024
#define LINUX_O_DIRECTORY 65536
#define LINUX_PROT_READ 1
#define LINUX_PROT_WRITE 2
#define LINUX_MAP_PRIVATE 2
#define LINUX_MAP_ANONYMOUS 32
#define LINUX_SEEK_SET 0
#define LINUX_SEEK_CUR 1
#define LINUX_SEEK_END 2
#define LINUX_EINTR 4
#define LINUX_ENOENT 2
#define LINUX_EISDIR 21
#define LINUX_ENOTDIR 20
#define LINUX_ERANGE 34
#define LINUX_ESRCH 3
#define LINUX_SIGTERM 15
#define LINUX_S_IFMT 0170000
#define LINUX_S_IFREG 0100000
#define LINUX_S_IFDIR 0040000
#define LINUX_WEXITSTATUS(status) (((status) >> 8) & 0xff)
#define LINUX_WIFEXITED(status) (((status) & 0x7f) == 0)
#define LINUX_WIFSIGNALED(status) (((status) & 0x7f) != 0 && ((status) & 0x7f) != 0x7f)
#define LINUX_WTERMSIG(status) ((status) & 0x7f)

#define LINUX_MAP_FAILED ((void*)-1)

struct stat
{
	uint64_t st_dev;
	uint64_t st_ino;
	uint64_t st_nlink;
	uint32_t st_mode;
	uint32_t st_uid;
	uint32_t st_gid;
	uint32_t __pad0;
	uint64_t st_rdev;
	int64_t st_size;
	int64_t st_blksize;
	int64_t st_blocks;
	uint64_t st_atime_sec;
	uint64_t st_atime_nsec;
	uint64_t st_mtime_sec;
	uint64_t st_mtime_nsec;
	uint64_t st_ctime_sec;
	uint64_t st_ctime_nsec;
	int64_t __unused[3];
};

#define S_ISDIR(mode) (((mode) & LINUX_S_IFMT) == LINUX_S_IFDIR)
#define S_ISREG(mode) (((mode) & LINUX_S_IFMT) == LINUX_S_IFREG)

/* linux_dirent64 — layout must exactly match the kernel ABI for getdents64 */
struct linux_dirent64
{
	uint64_t d_ino;       /* inode number              */
	int64_t  d_off;       /* offset to next dirent     */
	uint16_t d_reclen;    /* length of this record     */
	uint8_t  d_type;      /* file type (DT_* constants)*/
	char     d_name[1];   /* filename (null-terminated)*/
};

#define LINUX_DT_UNKNOWN 0
#define LINUX_DT_REG     8
#define LINUX_DT_DIR     4

typedef int64_t linux_ssize_t;
typedef int32_t linux_pid_t;
static int linux_errno;

static inline long linux_syscall0(long number)
{
	long result;
	__asm__ volatile ("syscall" : "=a"(result) : "a"(number) : "rcx", "r11", "memory");
	return result;
}

static inline long linux_syscall1(long number, long arg1)
{
	long result;
	__asm__ volatile ("syscall" : "=a"(result) : "a"(number), "D"(arg1) : "rcx", "r11", "memory");
	return result;
}

static inline long linux_syscall2(long number, long arg1, long arg2)
{
	long result;
	__asm__ volatile ("syscall" : "=a"(result) : "a"(number), "D"(arg1), "S"(arg2) : "rcx", "r11", "memory");
	return result;
}

static inline long linux_syscall3(long number, long arg1, long arg2, long arg3)
{
	long result;
	__asm__ volatile ("syscall" : "=a"(result) : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3) : "rcx", "r11", "memory");
	return result;
}

static inline long linux_syscall4(long number, long arg1, long arg2, long arg3, long arg4)
{
	long result;
	register long r10 __asm__("r10") = arg4;
	__asm__ volatile ("syscall" : "=a"(result) : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10) : "rcx", "r11", "memory");
	return result;
}

static inline long linux_syscall5(long number, long arg1, long arg2, long arg3, long arg4, long arg5)
{
	long result;
	register long r10 __asm__("r10") = arg4;
	register long r8 __asm__("r8") = arg5;
	__asm__ volatile ("syscall" : "=a"(result) : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10), "r"(r8) : "rcx", "r11", "memory");
	return result;
}

static inline long linux_syscall6(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6)
{
	long result;
	register long r10 __asm__("r10") = arg4;
	register long r8 __asm__("r8") = arg5;
	register long r9 __asm__("r9") = arg6;
	__asm__ volatile ("syscall" : "=a"(result) : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10), "r"(r8), "r"(r9) : "rcx", "r11", "memory");
	return result;
}

static inline long linux_result(long result)
{
	linux_errno = result < 0 && result >= -4095 ? (int)-result : 0;
	return linux_errno ? -1 : result;
}

static inline long linux_error(long result)
{
	(void)result;
	return linux_errno;
}

static inline void* linux_mmap(void* address, uint64_t size, int protection, int flags, int descriptor, int64_t offset)
{
	long result = linux_syscall6(9, (long)address, (long)size, protection, flags, descriptor, offset);
	return result < 0 && result >= -4095 ? LINUX_MAP_FAILED : (void*)result;
}

static inline long linux_munmap(void* address, uint64_t size)
{
	return linux_result(linux_syscall2(11, (long)address, (long)size));
}

static inline int linux_open(char* pathname, int flags, int mode)
{
	return (int)linux_result(linux_syscall4(257, LINUX_AT_FDCWD, (long)pathname, flags, mode));
}

static inline int linux_close(int descriptor)
{
	return (int)linux_result(linux_syscall1(3, descriptor));
}

static inline linux_ssize_t linux_read(int descriptor, void* buffer, uint64_t count)
{
	return (linux_ssize_t)linux_result(linux_syscall3(0, descriptor, (long)buffer, (long)count));
}

static inline linux_ssize_t linux_write(int descriptor, const void* buffer, uint64_t count)
{
	return (linux_ssize_t)linux_result(linux_syscall3(1, descriptor, (long)buffer, (long)count));
}

static inline int64_t linux_lseek(int descriptor, int64_t offset, int direction)
{
	return (int64_t)linux_result(linux_syscall3(8, descriptor, offset, direction));
}

static inline int linux_mkdir(char* pathname, uint32_t mode)
{
	return (int)linux_result(linux_syscall2(83, (long)pathname, mode));
}

static inline int linux_rmdir(char* pathname)
{
	return (int)linux_result(linux_syscall1(84, (long)pathname));
}

static inline int linux_open_dir(char* pathname)
{
	return (int)linux_result(linux_syscall4(257, LINUX_AT_FDCWD, (long)pathname, LINUX_O_RDONLY | LINUX_O_DIRECTORY, 0));
}

static inline long linux_getdents64(int descriptor, struct linux_dirent64* buffer, uint64_t count)
{
	return linux_result(linux_syscall3(217, descriptor, (long)buffer, (long)count));
}

static inline int linux_unlink(char* pathname)
{
	return (int)linux_result(linux_syscall1(87, (long)pathname));
}

static inline int linux_stat(char* pathname, struct stat* status)
{
	return (int)linux_result(linux_syscall4(262, LINUX_AT_FDCWD, (long)pathname, (long)status, 0));
}

static inline long linux_getcwd(char* buffer, uint64_t size)
{
	return linux_result(linux_syscall2(79, (long)buffer, (long)size));
}

static inline linux_pid_t linux_fork(void)
{
	return (linux_pid_t)linux_result(linux_syscall0(57));
}

static inline int linux_execve(char* pathname, char* const arguments[], char* const environment[])
{
	return (int)linux_result(linux_syscall3(59, (long)pathname, (long)arguments, (long)environment));
}

static inline linux_pid_t linux_waitpid(linux_pid_t process_id, int* status)
{
	return (linux_pid_t)linux_result(linux_syscall4(61, process_id, (long)status, 0, 0));
}

static inline int linux_kill(linux_pid_t process_id, int signal)
{
	return (int)linux_result(linux_syscall2(62, process_id, signal));
}

static inline void linux_exit(int status)
{
	linux_syscall1(60, status);
	for (;;) { }
}

#endif