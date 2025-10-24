global context_switch

  ;;
  ;; Define the registers
  ;;

%define CTX_OFF(n) (512 + (8 * n))

%define CTX_GS     CTX_OFF(0)
%define CTX_FS     CTX_OFF(1)
%define CTX_ES     CTX_OFF(2)
%define CTX_DS     CTX_OFF(3)
%define CTX_CR2    CTX_OFF(4)
%define CTX_RAX    CTX_OFF(6)
%define CTX_RBX    CTX_OFF(7)
%define CTX_RCX    CTX_OFF(8)
%define CTX_RDX    CTX_OFF(9)
%define CTX_RSI    CTX_OFF(10)
%define CTX_RDI    CTX_OFF(11)
%define CTX_RBP    CTX_OFF(12)
%define CTX_R8     CTX_OFF(13)
%define CTX_R9     CTX_OFF(14)
%define CTX_R10    CTX_OFF(15)
%define CTX_R11    CTX_OFF(16)
%define CTX_R12    CTX_OFF(17)
%define CTX_R13    CTX_OFF(18)
%define CTX_R14    CTX_OFF(19)
%define CTX_R15    CTX_OFF(20)
%define CTX_RIP    CTX_OFF(23)
%define CTX_CS     CTX_OFF(24)
%define CTX_RFLAGS CTX_OFF(25)
%define CTX_RSP    CTX_OFF(26)
%define CTX_SS     CTX_OFF(27)

  ;;
  ;; Disable inturrupts before trying to context switch
  ;;

context_switch:
  ;; Save segment registers
  mov [rdi + CTX_GS], gs
  mov [rdi + CTX_FS], fs
  mov [rdi + CTX_ES], es
  mov [rdi + CTX_DS], ds

  ;; Save CR2
  mov rax, cr2
  mov [rdi + CTX_CR2], rax

  ;; Save general purpose registers
  mov [rdi + CTX_RAX], rax
  mov [rdi + CTX_RBX], rbx
  mov [rdi + CTX_RCX], rcx
  mov [rdi + CTX_RDX], rdx
  mov [rdi + CTX_RSI], rsi
  mov [rdi + CTX_RDI], rdi
  mov [rdi + CTX_RBP], rbp
  mov [rdi + CTX_R8], r8
  mov [rdi + CTX_R9], r9
  mov [rdi + CTX_R10], r10
  mov [rdi + CTX_R11], r11
  mov [rdi + CTX_R12], r12
  mov [rdi + CTX_R13], r13
  mov [rdi + CTX_R14], r14
  mov [rdi + CTX_R15], r15

  ;; Save return address (RIP)
  pop rax
  mov [rdi + CTX_RIP], rax

  ;; Save CS, RFLAGS, RSP, SS
  mov [rdi + CTX_CS], cs
  pushfq
  pop rax
  mov [rdi + CTX_RFLAGS], rax
  mov [rdi + CTX_RSP], rsp
  mov [rdi + CTX_SS], ss

  ;; Save FPU/SSE state
  fxsave [rdi]

  ;; Load new context
  fxrstor [rsi]

  ;; Restore registers from new context
  mov rax, [rsi + CTX_CR2]
  mov cr2, rax
  mov rax, [rsi + CTX_RAX]
  mov rbx, [rsi + CTX_RBX]
  mov rcx, [rsi + CTX_RCX]
  mov rdx, [rsi + CTX_RDX]
  mov rdi, [rsi + CTX_RDI]
  mov rbp, [rsi + CTX_RBP]
  mov r8,  [rsi + CTX_R8]
  mov r9,  [rsi + CTX_R9]
  mov r10, [rsi + CTX_R10]
  mov r11, [rsi + CTX_R11]
  mov r12, [rsi + CTX_R12]
  mov r13, [rsi + CTX_R13]
  mov r14, [rsi + CTX_R14]
  mov r15, [rsi + CTX_R15]

  ;; Set stack pointer to new process stack
  mov rsp, [rsi + CTX_RSP]

  ;; Restore segment registers
  mov gs, [rsi + CTX_GS]
  mov fs, [rsi + CTX_FS]
  mov es, [rsi + CTX_ES]
  mov ds, [rsi + CTX_DS]

  ;; Return to new process
  mov rax, [rsi + CTX_RIP]  ; RIP
  mov rcx, [rsi + CTX_CS]   ; CS
  mov rdx, [rsi + CTX_RFLAGS] ; RFLAGS
  mov rbx, [rsi + CTX_SS]   ; SS (if returning to usermode)

  ;; Jump to RIP with CS, RFLAGS, RSP, SS
  iretq
