[extern isr_handler]

REGISTER_SIZE:      equ 0x78
QUADWORD_SIZE:      equ 0x08

%macro push_all 0
    push rdi
    push rsi
    push rdx
    push rcx
    push rax
    push r8
    push r9
    push r10
    push r11
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro pop_all 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rcx
    pop rdx
    pop rsi
    pop rdi
%endmacro

%macro call_handler 1
    push_all

    mov rdx, rsp
    mov rdi, [rsp + REGISTER_SIZE]
    mov rsi, [rsp + REGISTER_SIZE + QUADWORD_SIZE]

    call %1

    pop_all
%endmacro

%macro isr_noerror 1
global isr_%1
isr_%1:
    cli

    push qword 0
    push qword %1

    call_handler isr_handler

    add rsp, 0x10

    sti

    iretq
%endmacro

%macro isr_error 1
global isr_%1
isr_%1:
    cli

    push qword %1

    call_handler isr_handler

    sti

    iretq
%endmacro

%macro irq_stub 1
global irq_%1
irq_%1:
    cli

    push qword 0
    push qword (32 + %1)

    call_handler isr_handler

    add rsp, 0x10
    mov al, 0x20
    out 0xA0, al
    out 0x20, al

    sti
    iretq
%endmacro


isr_noerror 0
isr_noerror 1
isr_noerror 2
isr_noerror 3
isr_noerror 4
isr_noerror 5
isr_noerror 6
isr_noerror 7
isr_noerror 8
isr_noerror 9
isr_error 10
isr_error 11
isr_error 12
isr_error 13
isr_error 14
isr_noerror 15
isr_noerror 16
isr_noerror 17
isr_noerror 18
isr_noerror 19
isr_noerror 20
isr_noerror 21
isr_noerror 22
isr_noerror 23
isr_noerror 24
isr_noerror 25
isr_noerror 26
isr_noerror 27
isr_noerror 28
isr_noerror 29
isr_noerror 30
isr_noerror 31

irq_stub 0
irq_stub 1
irq_stub 2
irq_stub 3
irq_stub 4
irq_stub 5
irq_stub 6
irq_stub 7
irq_stub 8
irq_stub 9
irq_stub 10
irq_stub 11
irq_stub 12
irq_stub 13
irq_stub 14
irq_stub 15
