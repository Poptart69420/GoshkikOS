#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "../arch/x86_64/arch.h"
#include "../arch/x86_64/asm.h"
#include "../drivers/fs/vfs.h"
#include "spinlock.h"
#include <stdint.h>

#define MAX_FILE_DESCRIPTOR 32

#define FD_READ 0x01
#define FD_WRITE 0x02
#define FD_APPEND 0x04
#define FD_NONBLOCK 0x08

typedef struct {
    vfs_node_t *node;
    uint64_t offset;
    uint64_t present;
    uint64_t flags;
} file_descriptor_t;

typedef struct process_struct_t {
    acontext_t context;
    uint64_t address_space;
    uintptr_t kernel_stack;
    struct fault_frame *syscall_frame;
    pid_t pid;
    struct process_struct_t *snext;
    struct process_struct_t *next;
    struct process_struct_t *prev;
    struct process_struct_t *parent;
    atomic_int flags;
    spinlock_t state_lock;
    file_descriptor_t file_descriptor[MAX_FILE_DESCRIPTOR];
    vfs_node_t *cwd_node;
    char *cwd_path;
    uintptr_t heap_start;
    uintptr_t heap_end;
    list_t *memseg;
    pid_t wait_for;
    long exit_status;
    list_t *child;
    pid_t group;
    pid_t sid;
    uid_t uid;
    uid_t euid;
    uid_t suid;
    gid_t gid;
    gid_t egid;
    gid_t sgid;
    mode_t umask;
    struct process_struct *waker;
} process_t;

#endif // SCHEDULER_H_
