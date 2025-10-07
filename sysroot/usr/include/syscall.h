// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef _SYSCALL_H
#define _SYSCALL_H

#ifdef __x86_64__
static inline long __syscall0(long n) {
  long ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(n) : "memory");
  return ret;
}

static inline long __syscall1(long n, long a1) {
  long ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(n), "D"(a1) : "memory");
  return ret;
}

static inline long __syscall2(long n, long a1, long a2) {
  long ret;
  asm volatile("int $0x80" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2) : "memory");
  return ret;
}

static inline long __syscall3(long n, long a1, long a2, long a3) {
  long ret;
  asm volatile("int $0x80"
               : "=a"(ret)
               : "a"(n), "D"(a1), "S"(a2), "d"(a3)
               : "memory");
  return ret;
}

static inline long __syscall4(long n, long a1, long a2, long a3, long a4) {
  long ret;
  asm volatile("int $0x80"
               : "=a"(ret)
               : "a"(n), "D"(a1), "S"(a2), "d"(a3), "c"(a4)
               : "memory");
  return ret;
}

static inline long __syscall5(long n, long a1, long a2, long a3, long a4,
                              long a5) {
  long ret;
  register long r8 __asm__("r8") = a5;
  asm volatile("int $0x80"
               : "=a"(ret)
               : "a"(n), "D"(a1), "S"(a2), "d"(a3), "c"(a4), "r"(r8)
               : "memory");
  return ret;
}

static inline long __syscall6(long n, long a1, long a2, long a3, long a4,
                              long a5, long a6) {
  long ret;
  register long r8 __asm__("r8") = a5;
  register long r9 __asm__("r9") = a6;
  asm volatile("int $0x80"
               : "=a"(ret)
               : "a"(n), "D"(a1), "S"(a2), "d"(a3), "c"(a4), "r"(r8), "r"(r9)
               : "memory");
  return ret;
}
#elif defined(__aarch64__)
static inline long __syscall0(long n) {
  register long ret asm("r0");
  register long num asm("r8") = n;
  asm("svc 0" : "=r"(ret) : "r"(num) : "memory");
  return ret;
}

static inline long __syscall1(long n, long a1) {
  register long ret asm("r0");
  register long num asm("r8") = n;
  register long r0 asm("r0") = a1;
  asm("svc 0" : "=r"(ret) : "r"(num), "r"(r0) : "memory");
  return ret;
}

static inline long __syscall2(long n, long a1, long a2) {
  register long ret asm("r0");
  register long num asm("r8") = n;
  register long r0 asm("r0") = a1;
  register long r1 asm("r1") = a2;
  asm("svc 0" : "=r"(ret) : "r"(num), "r"(r0), "r"(r1) : "memory");
  return ret;
}

static inline long __syscall3(long n, long a1, long a2, long a3) {
  register long ret asm("r0");
  register long num asm("r8") = n;
  register long r0 asm("r0") = a1;
  register long r1 asm("r1") = a2;
  register long r2 asm("r2") = a3;
  asm("svc 0" : "=r"(ret) : "r"(num), "r"(r0), "r"(r1), "r"(r2) : "memory");
  return ret;
}

static inline long __syscall5(long n, long a1, long a2, long a3, long a4,
                              long a5) {
  register long ret asm("r0");
  register long num asm("r8") = n;
  register long r0 asm("r0") = a1;
  register long r1 asm("r1") = a2;
  register long r2 asm("r2") = a3;
  register long r3 asm("r3") = a4;
  register long r4 asm("r4") = a5;
  asm("svc 0"
      : "=r"(ret)
      : "r"(num), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4)
      : "memory");
  return ret;
}

static inline long __syscall6(long n, long a1, long a2, long a3, long a4,
                              long a5, long a6) {
  register long ret asm("r0");
  register long num asm("r8") = n;
  register long r0 asm("r0") = a1;
  register long r1 asm("r1") = a2;
  register long r2 asm("r2") = a3;
  register long r3 asm("r3") = a4;
  register long r4 asm("r4") = a5;
  register long r5 asm("r5") = a6;
  asm("svc 0"
      : "=r"(ret)
      : "r"(num), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
      : "memory");
  return ret;
}
#else
#error unsupported architecture
#endif

#define SYS_exit 0
#define SYS_open 1
#define SYS_close 2
#define SYS_read 3
#define SYS_write 4
#define SYS_seek 5
#define SYS_dup 6
#define SYS_dup2 7
#define SYS_sbrk 8
#define SYS_ioctl 9
#define SYS_nanosleep 10
#define SYS_sleepuntil 11
#define SYS_clock_gettime 12
#define SYS_clock_settime 13
#define SYS_pipe 14
#define SYS_execve 15
#define SYS_fork 16
#define SYS_mkdir 17
#define SYS_unlink 18
#define SYS_rmdir 19
#define SYS_readdir 20
#define SYS_stat 21
#define SYS_fstat 22
#define SYS_getcwd 23
#define SYS_chdir 24
#define SYS_waitpid 25
#define SYS_insmod 26
#define SYS_rmmod 27
#define SYS_isatty 28
#define SYS_openpty 29
#define SYS_poll 30
#define SYS_sigprogmask 31
#define SYS_sigaction 32
#define SYS_sigwait 33
#define SYS_sigsuspend 34
#define SYS_sigpending 35
#define SYS_kill 36
#define SYS_getpid 37
#define SYS_mount 38
#define SYS_umount 39
#define SYS_mmap 40
#define SYS_munmap 41
#define SYS_mprotect 42
#define SYS_msync 43
#define SYS_setuid 44
#define SYS_seteuid 45
#define SYS_getuid 46
#define SYS_geteuid 47
#define SYS_setgid 48
#define SYS_setegid 49
#define SYS_getgid 50
#define SYS_getegid 51
#define SYS_chmod 52
#define SYS_fchmod 53
#define SYS_chown 54
#define SYS_fchown 55
#define SYS_setpgid 56
#define SYS_getpgid 57
#define SYS_fcntl 58
#define SYS_umask 59
#define SYS_access 60
#define SYS_utimes 61
#define SYS_truncate 62
#define SYS_ftruncate 63
#define SYS_link 64
#define SYS_rename 65
#define SYS_lstat 66
#define SYS_lchmod 67
#define SYS_lchown 68
#define SYS_symlink 69
#define SYS_readlink 70

#endif

_END_C_HEADER
