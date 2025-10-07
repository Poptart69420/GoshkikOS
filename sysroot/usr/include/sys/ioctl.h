// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef IOCTL_H
#define IOCTL_H

#include <sys/types.h>

struct winsize {
  unsigned short ws_row;    // rows, in characters
  unsigned short ws_col;    // columns, in characters
  unsigned short ws_xpixel; // horizontal size, pixels
  unsigned short ws_ypixel; // vertical size, pixels
};

#define TIOCGETA 19  // get termios struct
#define TIOCSETA 20  // set termios struct
#define TIOCSETAW 21 // drain output, set
#define TIOCSETAF 22 // drn out, fls in, set

#define TIOCGPGRP 119 // get pgrp of tty
#define TIOCSPGRP 118 // set pgrp of tty

#define TIOCGWINSZ 104 // get window size
#define TIOCSWINSZ 103 // set window size

#define I_EJECT 501 // eject a device
#define I_MODEL 502 // get the model of a device (reserve 256 char)

int ioctl(int fd, unsigned long op, void *arg);

#endif

_END_C_HEADER
