global syscall
global syscall_entry
extern syscall_handler

  ;;
  ;; To-do: Setup MSRs to use syscall instruction instead of int 0x80
  ;;

section .text
; syscall(num, arg1, arg2, arg3, arg4, arg5, arg6)
; rdi, rsi, rdx, rcx, r8, r9 = first six C arguments

syscall:
  mov     rax, rdi        ; syscall number
  mov     rdi, rsi        ; arg1
  mov     rsi, rdx        ; arg2
  mov     rdx, rcx        ; arg3
  mov     r10, r8         ; arg4
  mov     r8,  r9         ; arg5
  mov     r9,  [rsp+8]    ; arg6

  int 0x80                 ; perform syscall
  ret                     ; result in rax

syscall_entry:
  push    rbx
  push    rcx
  push    rdx
  push    rsi
  push    rdi
  push    rbp
  push    r8
  push    r9
  push    r10
  push    r11
  push    r12
  push    r13
  push    r14
  push    r15

  mov     rbx, rdi        ; arg1
  mov     r11, rsi        ; arg2
  mov     r12, rdx        ; arg3
  mov     r13, r10        ; arg4
  mov     r14, r8         ; arg5
  mov     r15, r9         ; arg6

  mov     rdi, rax        ; syscall number
  mov     rsi, rbx        ; arg1
  mov     rdx, r11        ; arg2
  mov     rcx, r12        ; arg3
  mov     r8,  r13        ; arg4
  mov     r9,  r14        ; arg5
  push    r15             ; arg6

  call    syscall_handler
  add     rsp, 8          ; pop arg6

  pop     r15
  pop     r14
  pop     r13
  pop     r12
  pop     r11
  pop     r10
  pop     r9
  pop     r8
  pop     rbp
  pop     rdi
  pop     rsi
  pop     rdx
  pop     rcx
  pop     rbx

  iretq
