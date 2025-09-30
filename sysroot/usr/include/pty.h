//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef PTY_H
#define PTY_H

#include <termios.h>
#include <sys/ioctl.h>

int openpty(int *amaster, int *aslave, char *name,const struct termios *termp,const struct winsize *winp);

#endif
_END_C_HEADER
