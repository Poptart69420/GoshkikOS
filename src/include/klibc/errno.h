#ifndef ERRNO_H_
#define ERRNO_H_

#define EPERM 1             /* Not super-user */
#define ENOENT 2            /* No such file or directory */
#define ESRCH 3             /* No such process */
#define EINTR 4             /* Interrupted system call */
#define EIO 5               /* I/O error */
#define ENXIO 6             /* No such device or address */
#define E2BIG 7             /* Arg list too long */
#define ENOEXEC 8           /* Exec format error */
#define EBADF 9             /* Bad file number */
#define ECHILD 10           /* No children */
#define EAGAIN 11           /* No more processes */
#define ENOMEM 12           /* Not enough core */
#define EACCES 13           /* Permission denied */
#define EFAULT 14           /* Bad address */
#define ENOTBLK 15          /* Block device required */
#define EBUSY 16            /* Mount device busy */
#define EEXIST 17           /* File exists */
#define EXDEV 18            /* Cross-device link */
#define ENODEV 19           /* No such device */
#define ENOTDIR 20          /* Not a directory */
#define EISDIR 21           /* Is a directory */
#define EINVAL 22           /* Invalid argument */
#define ENFILE 23           /* Too many open files in system */
#define EMFILE 24           /* Too many open files */
#define ENOTTY 25           /* Not a typewriter */
#define ETXTBSY 26          /* Text file busy */
#define EFBIG 27            /* File too large */
#define ENOSPC 28           /* No space left on device */
#define ESPIPE 29           /* Illegal seek */
#define EROFS 30            /* Read only file system */
#define EMLINK 31           /* Too many links */
#define EPIPE 32            /* Broken pipe */
#define EDOM 33             /* Math arg out of domain of func */
#define ERANGE 34           /* Math result not representable */
#define ENOMSG 35           /* No message of desired type */
#define EIDRM 36            /* Identifier removed */
#define ECHRNG 37           /* Channel number out of range */
#define EL2NSYNC 38         /* Level 2 not synchronized */
#define EL3HLT 39           /* Level 3 halted */
#define EL3RST 40           /* Level 3 reset */
#define ELNRNG 41           /* Link number out of range */
#define EUNATCH 42          /* Protocol driver not attached */
#define ENOCSI 43           /* No CSI structure available */
#define EL2HLT 44           /* Level 2 halted */
#define EDEADLK 45          /* Deadlock condition */
#define ENOLCK 46           /* No record locks available */
#define EBADE 50            /* Invalid exchange */
#define EBADR 51            /* Invalid request descriptor */
#define EXFULL 52           /* Exchange full */
#define ENOANO 53           /* No anode */
#define EBADRQC 54          /* Invalid request code */
#define EBADSLT 55          /* Invalid slot */
#define EDEADLOCK 56        /* File locking deadlock error */
#define EBFONT 57           /* Bad font file fmt */
#define ENOSTR 60           /* Device not a stream */
#define ENODATA 61          /* No data (for no delay io) */
#define ETIME 62            /* Timer expired */
#define ENOSR 63            /* Out of streams resources */
#define ENONET 64           /* Machine is not on the network */
#define ENOPKG 65           /* Package not installed */
#define EREMOTE 66          /* The object is remote */
#define ENOLINK 67          /* The link has been severed */
#define EADV 68             /* Advertise error */
#define ESRMNT 69           /* Srmount error */
#define ECOMM 70            /* Communication error on send */
#define EPROTO 71           /* Protocol error */
#define EMULTIHOP 74        /* Multihop attempted */
#define ELBIN 75            /* Inode is remote (not really error) */
#define EDOTDOT 76          /* Cross mount point (not really error) */
#define EBADMSG 77          /* Trying to read unreadable message */
#define EFTYPE 79           /* Inappropriate file type or format */
#define ENOTUNIQ 80         /* Given log. name not unique */
#define EBADFD 81           /* f.d. invalid for this operation */
#define EREMCHG 82          /* Remote address changed */
#define ELIBACC 83          /* Can't access a needed shared lib */
#define ELIBBAD 84          /* Accessing a corrupted shared lib */
#define ELIBSCN 85          /* .lib section in a.out corrupted */
#define ELIBMAX 86          /* Attempting to link in too many libs */
#define ELIBEXEC 87         /* Attempting to exec a shared library */
#define ENOSYS 88           /* Function not implemented */
#define ENOTEMPTY 90        /* Directory not empty */
#define ENAMETOOLONG 91     /* File or path name too long */
#define ELOOP 92            /* Too many symbolic links */
#define EOPNOTSUPP 95       /* Operation not supported on transport endpoint */
#define EPFNOSUPPORT 96     /* Protocol family not supported */
#define ECONNRESET 104      /* Connection reset by peer */
#define ENOBUFS 105         /* No buffer space available */
#define EAFNOSUPPORT 106    /* Address family not supported by protocol family */
#define EPROTOTYPE 107      /* Protocol wrong type for socket */
#define ENOTSOCK 108        /* Socket operation on non-socket */
#define ENOPROTOOPT 109     /* Protocol not available */
#define ESHUTDOWN 110       /* Can't send after socket shutdown */
#define ECONNREFUSED 111    /* Connection refused */
#define EADDRINUSE 112      /* Address already in use */
#define ECONNABORTED 113    /* Connection aborted */
#define ENETUNREACH 114     /* Network is unreachable */
#define ENETDOWN 115        /* Network interface is not configured */
#define ETIMEDOUT 116       /* Connection timed out */
#define EHOSTDOWN 117       /* Host is down */
#define EHOSTUNREACH 118    /* Host is unreachable */
#define EINPROGRESS 119     /* Connection already in progress */
#define EALREADY 120        /* Socket already connected */
#define EDESTADDRREQ 121    /* Destination address required */
#define EMSGSIZE 122        /* Message too long */
#define EPROTONOSUPPORT 123 /* Unknown protocol */
#define ESOCKTNOSUPPORT 124 /* Socket type not supported */
#define EADDRNOTAVAIL 125   /* Address not available */
#define ENETRESET 126
#define EISCONN 127  /* Socket is already connected */
#define ENOTCONN 128 /* Socket is not connected */
#define ETOOMANYREFS 129
#define EPROCLIM 130
#define EUSERS 131
#define EDQUOT 132
#define ESTALE 133
#define ENOTSUP 134 /* Not supported */
#define EILSEQ 138
#define EOVERFLOW 139       /* Value too large for defined data type */
#define ECANCELED 140       /* Operation canceled */
#define ENOTRECOVERABLE 141 /* State not recoverable */
#define EOWNERDEAD 142      /* Previous owner died */
#define ESTRPIPE 143        /* Streams pipe error */
#define EWOULDBLOCK EAGAIN  /* Operation would block */

extern int errno;
static inline int __set_errno(int ret)
{
  if (ret < 0)
  {
    errno = -ret;
  }
  return ret;
}

static inline char *__get_err_code(int err)
{
  switch (err)
  {
  case EPERM:
    return "Operation not permitted";
  case ENOENT:
    return "No such file or directory";
  case ESRCH:
    return "No such process";
  case EINTR:
    return "Interrupted system call";
  case EIO:
    return "I/O error";
  case ENXIO:
    return "No such device or address";
  case E2BIG:
    return "Argument list too long";
  case ENOEXEC:
    return "Exec format error";
  case EBADF:
    return "Bad file descriptor";
  case ECHILD:
    return "No child processes";
  case EAGAIN:
    return "Resource temporarily unavailable";
  case ENOMEM:
    return "Not enough memory";
  case EACCES:
    return "Permission denied";
  case EFAULT:
    return "Bad address";
  case ENOTBLK:
    return "Block device required";
  case EBUSY:
    return "Resource busy";
  case EEXIST:
    return "File exists";
  case EXDEV:
    return "Cross-device link";
  case ENODEV:
    return "No such device";
  case ENOTDIR:
    return "Not a directory";
  case EISDIR:
    return "Is a directory";
  case EINVAL:
    return "Invalid argument";
  case ENFILE:
    return "Too many open files in system";
  case EMFILE:
    return "Too many open files";
  case ENOTTY:
    return "Not a typewriter";
  case ETXTBSY:
    return "Text file busy";
  case EFBIG:
    return "File too large";
  case ENOSPC:
    return "No space left on device";
  case ESPIPE:
    return "Illegal seek";
  case EROFS:
    return "Read-only file system";
  case EMLINK:
    return "Too many links";
  case EPIPE:
    return "Broken pipe";
  case EDOM:
    return "Math argument out of domain of func";
  case ERANGE:
    return "Math result not representable";
  case ENOMSG:
    return "No message of desired type";
  case EIDRM:
    return "Identifier removed";
  case ECHRNG:
    return "Channel number out of range";
  case EL2NSYNC:
    return "Level 2 not synchronized";
  case EL3HLT:
    return "Level 3 halted";
  case EL3RST:
    return "Level 3 reset";
  case ELNRNG:
    return "Link number out of range";
  case EUNATCH:
    return "Protocol driver not attached";
  case ENOCSI:
    return "No CSI structure available";
  case EL2HLT:
    return "Level 2 halted";
  case EDEADLK:
    return "Deadlock condition";
  case ENOLCK:
    return "No record locks available";
  case EBADE:
    return "Invalid exchange";
  case EBADR:
    return "Invalid request descriptor";
  case EXFULL:
    return "Exchange full";
  case ENOANO:
    return "No anode";
  case EBADRQC:
    return "Invalid request code";
  case EBADSLT:
    return "Invalid slot";
  case EDEADLOCK:
    return "File locking deadlock error";
  case EBFONT:
    return "Bad font file format";
  case ENOSTR:
    return "Device not a stream";
  case ENODATA:
    return "No data (for no delay IO)";
  case ETIME:
    return "Timer expired";
  case ENOSR:
    return "Out of streams resources";
  case ENONET:
    return "Machine is not on the network";
  case ENOPKG:
    return "Package not installed";
  case EREMOTE:
    return "Object is remote";
  case ENOLINK:
    return "Link has been severed";
  case EADV:
    return "Advertise error";
  case ESRMNT:
    return "Srmount error";
  case ECOMM:
    return "Communication error on send";
  case EPROTO:
    return "Protocol error";
  case EMULTIHOP:
    return "Multihop attempted";
  case ELBIN:
    return "Inode is remote";
  case EDOTDOT:
    return "Cross mount point";
  case EBADMSG:
    return "Unreadable message";
  case EFTYPE:
    return "Inappropriate file type or format";
  case ENOTUNIQ:
    return "Given log. name not unique";
  case EBADFD:
    return "Invalid file descriptor";
  case EREMCHG:
    return "Remote address changed";
  case ELIBACC:
    return "Can't access needed shared lib";
  case ELIBBAD:
    return "Corrupted shared lib";
  case ELIBSCN:
    return ".lib section in a.out corrupted";
  case ELIBMAX:
    return "Too many libs linked";
  case ELIBEXEC:
    return "Exec shared library failed";
  case ENOSYS:
    return "Function not implemented";
  case ENOTEMPTY:
    return "Directory not empty";
  case ENAMETOOLONG:
    return "File or path name too long";
  case ELOOP:
    return "Too many symbolic links";
  case EOPNOTSUPP:
    return "Operation not supported";
  case EPFNOSUPPORT:
    return "Protocol family not supported";
  case ECONNRESET:
    return "Connection reset by peer";
  case ENOBUFS:
    return "No buffer space available";
  case EAFNOSUPPORT:
    return "Address family not supported";
  case EPROTOTYPE:
    return "Protocol wrong type for socket";
  case ENOTSOCK:
    return "Socket operation on non-socket";
  case ENOPROTOOPT:
    return "Protocol not available";
  case ESHUTDOWN:
    return "Can't send after socket shutdown";
  case ECONNREFUSED:
    return "Connection refused";
  case EADDRINUSE:
    return "Address already in use";
  case ECONNABORTED:
    return "Connection aborted";
  case ENETUNREACH:
    return "Network is unreachable";
  case ENETDOWN:
    return "Network interface not configured";
  case ETIMEDOUT:
    return "Connection timed out";
  case EHOSTDOWN:
    return "Host is down";
  case EHOSTUNREACH:
    return "Host unreachable";
  case EINPROGRESS:
    return "Connection already in progress";
  case EALREADY:
    return "Socket already connected";
  case EDESTADDRREQ:
    return "Destination address required";
  case EMSGSIZE:
    return "Message too long";
  case EPROTONOSUPPORT:
    return "Unknown protocol";
  case ESOCKTNOSUPPORT:
    return "Socket type not supported";
  case EADDRNOTAVAIL:
    return "Address not available";
  case ENETRESET:
    return "Network reset";
  case EISCONN:
    return "Socket is already connected";
  case ENOTCONN:
    return "Socket not connected";
  case ETOOMANYREFS:
    return "Too many references";
  case EPROCLIM:
    return "Process limit reached";
  case EUSERS:
    return "Too many users";
  case EDQUOT:
    return "Quota exceeded";
  case ESTALE:
    return "Stale file handle";
  case EILSEQ:
    return "Illegal byte sequence";
  case EOVERFLOW:
    return "Value too large for type";
  case ECANCELED:
    return "Operation canceled";
  case ENOTRECOVERABLE:
    return "State not recoverable";
  case EOWNERDEAD:
    return "Previous owner died";
  case ESTRPIPE:
    return "Streams pipe error";
  default:
    return "Unknown error";
  }
}

#endif // ERRNO_H_
