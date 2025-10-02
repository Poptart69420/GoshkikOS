#include <scheduling/syscall.h>

void *syscall_table[] = {

};

uint64_t syscall_number = sizeof(syscall_table) / sizeof(void *);

void syscall_handler(fault_frame_t *context) {
    (void)context;
    enable_interrupt();
}
