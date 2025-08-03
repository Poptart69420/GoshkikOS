#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>

char *__env[1] = { 0 };
char **environ = __env;

void _exit()
{
  while (1) __asm__ volatile("cli; hlt");
}

int close(int file)
{
  return -1;
}

int execve(char *name, char **argv, char **env)
{
  errno = ENOMEM;
  return -1;
}

int fork(void)
{
  errno = EAGAIN;
  return -1;
}

int fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int getpid(void)
{
  return 1;
}

int isatty(int file)
{
  return 1;
}
