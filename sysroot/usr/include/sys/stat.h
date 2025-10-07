// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef STAT_H
#define STAT_H

#include <sys/type.h>
#include <sys/types.h>

struct stat
{
  dev_t st_dev;         // ID of device containing file
  ino_t st_ino;         // file serial number
  mode_t st_mode;       // mode of file (see below)
  nlink_t st_nlink;     // number of links to the file
  uid_t st_uid;         // user ID of file
  gid_t st_gid;         // group ID of file
  dev_t st_rdev;        // device ID (if file is character or block special)
  off_t st_size;        // file size in bytes (if file is a regular file)
  time_t st_atime;      // time of last access
  time_t st_mtime;      // time of last data modification
  time_t st_ctime;      // time of last status change
  blksize_t st_blksize; // a filesystem-specific preferred I/O block size for
  blkcnt_t st_blocks;   // number of blocks allocated for this object
};

#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IXUSR 0100
#define S_IRWXU S_IRUSR | S_IWUSR | S_IXUSR

#define S_IRGRP 040
#define S_IWGRP 020
#define S_IXGRP 010
#define S_IRWXG S_IRGRP | S_IWGRP | S_IXGRP

#define S_IROTH 04
#define S_IWOTH 02
#define S_IXOTH 01
#define S_IRWXO S_IROTH | S_IWOTH | S_IXOTH

#define S_ISVTX 01000 // restriction deletion bit
#define S_ISUID 02000 // set uid bit
#define S_ISGID 04000 // set gid bit

// files types
#define S_IFBLK 0010000 // block device
#define S_IFCHR 0020000 // character device
#define S_IFIFO 0040000 // FIFO (Firt In First Out)
#define S_IFREG 0100000 // regular file
#define S_IFDIR 0200000 // directory
#define S_IFLNK 0400000 // symbolic link

#define S_IFMT 0770000

// types test

#define S_ISBLK(m) ((m & S_IFMT) == S_IFBLK) // Test for a block special file.
#define S_ISCHR(m) \
  ((m & S_IFMT) == S_IFCHR)                  // Test for a character special file.
#define S_ISDIR(m) ((m & S_IFMT) == S_IFDIR) // Test for a directory.
#define S_ISFIFO(m) \
  ((m & S_IFMT) == S_IFIFO)                  // Test for a pipe or FIFO special file.
#define S_ISREG(m) ((m & S_IFMT) == S_IFREG) // Test for a regular file.
#define S_ISLNK(m) ((m & S_IFMT) == S_IFLNK) // Test for a symbolic link.

// functions
int stat(const char *pathname, struct stat *st);
int fstat(int fd, struct stat *st);
int lstat(const char *pathname, struct stat *st);
int chmod(const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);
int lchmod(const char *pathname, mode_t mode);
int chown(const char *pathname, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int lchown(const char *pathname, uid_t owner, gid_t group);
mode_t umask(mode_t mask);

#endif
_END_C_HEADER
