global context_switch

USER_CODE64 equ 0x1B
USER_DATA64 equ 0x23

context_switch:
  push rbp
  push rbx
  push r12
  push r13
  push r14
  push r15

  cmp rdx, 1
  je .context_ring_3

  mov rcx, 22 ;; 22 qwords (176 bytes) if context_t struct is updated, this must be updated as well
  cld
  rep movsq
  jmp .context_done

.context_ring_3:
  mov rax, [rsi + 0]       ;; r15
  mov [rdi + 0], rax
  mov rax, [rsi + 8]       ;; r14
  mov [rdi + 8], rax
  mov rax, [rsi + 16]      ;; r13
  mov [rdi + 16], rax
  mov rax, [rsi + 24]      ;; r12
  mov [rdi + 24], rax
  mov rax, [rsi + 32]      ;; r11
  mov [rdi + 32], rax
  mov rax, [rsi + 40]      ;; r10
  mov [rdi + 40], rax
  mov rax, [rsi + 48]      ;; r9
  mov [rdi + 48], rax
  mov rax, [rsi + 56]      ;; r8
  mov [rdi + 56], rax
  mov rax, [rsi + 64]      ;; rbp
  mov [rdi + 64], rax
  mov rax, [rsi + 72]      ;; rdi
  mov [rdi + 72], rax
  mov rax, [rsi + 80]      ;; rsi
  mov [rdi + 80], rax
  mov rax, [rsi + 88]      ;; rdx
  mov [rdi + 88], rax
  mov rax, [rsi + 96]      ;; rcx
  mov [rdi + 96], rax
  mov rax, [rsi + 104]     ;; rbx
  mov [rdi + 104], rax
  mov rax, [rsi + 112]     ;; rax
  mov [rdi + 112], rax
  mov rax, [rsi + 136]     ;; rip
  mov [rdi + 136], rax

    ;; cs = USER_CODE64
  mov rax, [rel USER_CODE64]
  mov [rdi + 144], rax

    ;; rflags | 0x200
  mov rax, [rsi + 152]
  or  rax, 0x200
  mov [rdi + 152], rax

    ;; rsp
  mov rax, [rsi + 160]
  mov [rdi + 160], rax

  ;; ss = USER_DATA64
  mov rax, [rel USER_DATA64]
  mov [rdi + 168], rax

.context_done:
  pop r15
  pop r14
  pop r13
  pop r12
  pop rbx
  pop rbp
  ret
