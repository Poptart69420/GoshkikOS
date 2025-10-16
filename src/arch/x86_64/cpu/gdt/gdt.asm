extern gdt_ptr

global gdt_reload
gdt_reload:
    lgdt [rel gdt_ptr]
    push 8
    lea rax, [rel .cleanup]
    push rax
    retfq
.cleanup:
    mov eax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

global tss_reload
tss_reload:
    mov ax, 0x28
    ltr ax
    ret
