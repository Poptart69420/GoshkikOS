%macro pushall 0
    
push r15
push r14
push r13
push r12
push r11
push r10
push r9
push r8
push rbp
push rdi
push rsi
push rdx
push rcx
push rbx
push rax

%endmacro

%macro popall 0

pop rax
pop rbx
pop rcx
pop rdx
pop rsi
pop rdi
pop rbp
pop r8
pop r9
pop r10
pop r11
pop r12
pop r13
pop r14
pop r15

%endmacro

extern isr_handler

isr_common:
    pushall
    
    mov rax, cr3
    push rax
    mov rax, cr2
    push rax

    xor rax, rax
    mov ax, ds
    push rax
    mov ax, es
    push rax
    mov ax, fs
    push rax
    mov ax, gs
    push rax

    mov rdi, rsp
    cld
    and rsp, ~0xf

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    
    call isr_handler

    pop rax
    mov gs, ax
    pop rax
    mov fs, ax
    pop rax
    mov es, ax
    pop rax
    mov ds, ax

    add rsp, 16
    
    popall
    add rsp, 16
    iretq

%macro isr 1

global isr%1
isr%1:
    push 0
    push %1
    jmp isr_common

%endmacro

%macro isr_error 1

global isr%1
isr%1:
    push %1
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

section .text
align 8
global isr_stub_table
isr_stub_table:
%assign __vec 0
%rep 256
    dq isr%+__vec
%assign __vec __vec + 1
%endrep
