%macro pushall 0

push rax
push rbx
push rcx
push rdx
push rbp
push rdi
push rsi
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15

%endmacro

%macro popall 0

pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rsi
pop rdi
pop rbp
pop rdx
pop rcx
pop rbx
pop rax

%endmacro

extern isr_handler

isr_common:
    pushall
    cld
    mov rdi, rsp
    xor rbp, rbp
    call isr_handler
    popall
    add rsp, 24
    iretq

%macro isr 1

global isr%1
isr%1:
    push 0
    push %1
    push fs
    jmp isr_common

%endmacro

%macro isr_error 1

global isr%1
isr%1:
    push 0
    push %1
    push fs
    jmp isr_common

%endmacro

%define is_error(i) (i == 8 || (i >= 10 && i <= 14) || i == 17 || i == 21 || i == 29 || i == 30)

%assign i 0
%rep 256
%if !is_error(i)
    isr i
%else
    isr_error i
%endif
%assign i i + 1
%endrep
