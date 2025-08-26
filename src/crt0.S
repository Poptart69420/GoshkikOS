.section .text

.global _start
_start:
    # Set up end of the stack frame linked list.
    movq $0, %rbp
    pushq %rbp # rip=0
    pushq %rbp # rbp=0
    movq %rsp, %rbp

    # We need those in a moment when we call main.
    pushq %rsi
    pushq %rdi


    # Run the global constructors.
    call _init

    # Restore argc and argv.
    popq %rdi
    popq %rsi

    # Run main
    call kmain

    # Terminate the process with the exit code.
    movl %eax, %edi
    call exit
.size _start, . - _start
