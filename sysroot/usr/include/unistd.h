// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef UNISTD_H
#define UNISTD_H

#include <fcntl.h>
#include <sys/type.h>
#include <sys/types.h>

// we can't include stdint because of namespace pollution
#ifndef __intptr_t_defined
#define __intptr_t_defined 1
typedef __PTRDIFF_TYPE__ intptr_t;
typedef __SIZE_TYPE__ uintptr_t;
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

int access(const char *pathname, int mode);

off_t lseek(int fd, off_t offset, int whence);

ssize_t read(int fd, const void *buffer, size_t count);
ssize_t write(int fd, const void *buffer, size_t count);

int truncate(const char *path, off_t length);
int ftruncate(int fd, off_t length);

int dup(int oldfd);
int dup2(int oldfd, int newfd);

void *sbrk(intptr_t increment);

void _exit(int status);

int close(int fd);

int unlink(const char *pathname);
int link(const char *oldpath, const char *newpath);
int symlink(const char *target, const char *linkpath);
ssize_t readlink(const char *path, char *buf, size_t bufsize);
int rmdir(const char *pathname);

unsigned int sleep(unsigned int seconds);
int usleep(useconds_t usec);

int pipe(int pipefd[2]);

int execve(const char *pathname, char *const argv[], char *const envp[]);

pid_t fork(void);

int mkdir(const char *pathname, mode_t mode);

// not syscall exec

int execl(const char *pathname, const char *arg, ... /*, (char *) NULL */);
int execlp(const char *file, const char *arg, ... /*, (char *) NULL */);
int execle(const char *pathname, const char *arg,
           ... /*, (char *) NULL, char *const envp[] */);
int execv(const char *pathname, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);

// cwd management

char *getcwd(char *buf, size_t size);
int chdir(const char *path);

int isatty(int fd);

// process management
pid_t getpid();
pid_t tcgetpgrp(int fd);
int tcsetpgrp(int fd, pid_t pgrp);
int setpgid(pid_t pid, pid_t pgid);
pid_t getpgid(pid_t pid);
pid_t getpgrp(void);
int setpgrp(void);

// user management
int setuid(uid_t uid);
int seteuid(uid_t uid);
uid_t getuid(void);
uid_t geteuid(void);
int setgid(gid_t gid);
int setegid(gid_t gid);
gid_t getgid(void);
gid_t getegid(void);
char *getlogin(void);
int getlogin_r(char *buf, size_t bufsize);

long pathconf(const char *pathname, int varcode);
#define _PC_PATH_MAX 0

int getopt(int argc, char *const *argv, const char *optstring);

#ifndef NULL
#define NULL ((void *)0)
#endif

extern char **environ;
extern int optind;
extern int opterr;
extern char *optarg;

#endif

_END_C_HEADER
