// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef _PWD_H
#define _PWD_H

#include <stdio.h>
#include <sys/types.h>

struct passwd {
  char *pw_name;   // user's login name
  char *pw_passwd; // user's password
  uid_t pw_uid;    // numerical user ID
  gid_t pw_gid;    // numerical group ID
  char *pw_gecos;  // user information
  char *pw_dir;    // initial working directory
  char *pw_shell;  // program to use as shell
};

struct passwd *getpwnam(const char *);
struct passwd *getpwuid(uid_t);
int getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **);
int getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **);
void endpwent(void);
struct passwd *getpwent(void);
void setpwent(void);
int fgetpwent_r(FILE *stream, struct passwd *pwbuf, char *buf, size_t size,
                struct passwd **pwbufp);
struct passwd *fgetpwent(FILE *stream);

#endif
_END_C_HEADER
