// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef POLL_H
#define POLL_H

struct pollfd
{
  int fd;        // file descriptor
  short events;  // requested events
  short revents; // returned events
};

typedef unsigned int nfds_t;

int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#define POLLWRNORM 0x01 // data on priority band 0 may be written
#define POLLWRBAND 0x02 // data on priority bands greater than 0 may be written
#define POLLRDNORM 0x04 // data on priority band 0 may be read
#define POLLRDBAND 0x08 // data on priority bands greater than 0 may be read
#define POLLIN \
  (POLLRDNORM | POLLRDBAND) // same effect as POLLRDNORM | POLLRDBAND
#define POLLPRI 0x10        // high priority data may be read
#define POLLOUT POLLWRNORM  // same value as POLLWRNORM
#define POLLERR 0x100       // an error has occurred (revents only)
#define POLLHUP 0x200       // device has been disconnected (revents only)
#define POLLNVAL 0x400      // invalid fd member (revents only)

#endif
_END_C_HEADER
