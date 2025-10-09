#ifndef CRED_H_
#define CRED_H_

typedef struct
{
  int uid, euid, suid;
  int gid, egid, sgid;
} cred_t;

#endif // CRED_H_
