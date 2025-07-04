org 0x8000
[bits 16]

jmp short stage2_main
nop

gdt32_start:

gdt32_null:
    dd 0x00000000
    dd 0x00000000

gdt32_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10011010
    db 0b11001111
    db 0x00

gdt32_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10010010
    db 0b11001111
    db 0x00

gdt32_end:

gdt32_descriptor:
    dw gdt32_end - gdt32_start - 1
    dd gdt32_start

gdt64_start:

gdt64_null:
    dd 0x00000000
    dd 0x00000000

gdt64_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10011010
    db 0b10101111
    db 0x00

gdt64_data:
    dw 0x0000
    dw 0x0000
    db 0x00
    db 0b10010010
    db 0b10100000
    db 0x00

gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1
    dd gdt64_start

CODE_SEG32:         equ gdt32_code - gdt32_start
DATA_SEG32:         equ gdt32_data - gdt32_start
CODE_SEG64:         equ gdt64_code - gdt64_start
DATA_SEG64:         equ gdt64_data - gdt64_start

KERNEL_BUFFER       equ 0x10000
NUMBER_OF_SECTORS   equ 50

stage2_main:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov bp, 0x7c00
    mov sp, bp
    mov ss, ax
    sti

    mov [drive_number], dl

    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov dh, NUMBER_OF_SECTORS
    mov dl, [drive_number]

    push dx
    mov ah, 0x02

    mov al, dh
    mov ch, 0
    mov dh, 0
    mov cl, 4

    int 0x13

    jc floppy_error

    pop dx
    cmp dh, al

    jne incomplete_read

.setup_gdt:
    cli
    lgdt [gdt32_descriptor]

    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax

    jmp CODE_SEG32:init_pm

    ;;
    ;; Random shit for 16 bit real mode
    ;;

msg_error1 db 'Disk failed to load kernel', 0x0D, 0x0A, 0
msg_error2 db 'Failed to read from disk', 0x0D, 0x0A, 0
msg_error3: db 'Press any key to reboot...', 0x0D, 0x0A, 0

oem_name:               db "MSWIN4.1"
bytes_per_sector:       dw 512
sectors_per_cluster:    db 1
reserved_sectors:       dw 1
number_of_fats:         db 2
root_directory_entries: dw 224
total_sectors:          dw 2880
media_descriptor:       db 0xF0
sectors_per_fat:        dw 9
sectors_per_track:      dw 18
number_of_heads:        dw 2
hidden_sectors:         dd 0
large_total_sectors:    dd 0

    ;; Extended Boot Record
drive_number:           db 0x0
reserved:               db 0
boot_signature:         db 0x29
volume_id:              dd 0
volume_label:           db "SHIT OS    "
file_system_type:       db "FAT12   "

incomplete_read:
    mov si, msg_error1
    call puts
    jmp press_key_reboot

floppy_error:
    mov si, msg_error2
    call puts
    jmp press_key_reboot

press_key_reboot:
    mov si, msg_error3
    call puts

    mov ah, 0
    int 0x16
    int 0x19

puts:
    pusha

.print_loop:
    lodsb
    or al, al
    jz .print_loop_done
    mov ah, 0EH
    int 0x10
    jmp .print_loop
.print_loop_done:
    popa
    ret

    ;;
    ;; 32 bit protected mode
    ;;

[bits 32]

vga_start:          equ 0x000B8000
vga_extent:         equ 80 * 25 * 2
style_wb:           equ 0x0F

msg_protected_mode: dw 'Entered 32 bit protected mode', 0

init_pm:
    mov ax, DATA_SEG32
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x90000
    mov esp, ebp

    call init_page_table
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64_descriptor]
    jmp 0x08:init_lm

init_page_table:
    mov eax, cr0
    and eax, 01111111111111111111111111111111b
    mov cr0, eax

    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096
    rep stosd

    mov edi, cr3

    mov dword[edi], 0x2003
    add edi, 0x1000
    mov dword[edi], 0x3003
    add edi, 0x1000
    mov dword[edi], 0x4003

    add edi, 0x1000
    mov ebx, 0x00000083
    mov ecx, 512

.add_page_entry:
    mov dword[edi], ebx
    add ebx, 0x1000
    add edi, 8
    loop .add_page_entry

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ret

pputs:
    pushad
    mov edx, vga_start

.pprint_loop:
    cmp byte[esi], 0
    je .pprint_loop_done

    mov al, byte[esi]
    mov ah, style_wb

    mov word[edx], ax

    add esi, 1
    add edx, 2

    jmp .pprint_loop

.pprint_loop_done:
    popad
    ret

    ;;
    ;; 64 bit long mode
    ;;

[bits 64]

space_char: equ ` `
style_blue: equ 0x1F

msg_long_mode: dw 'Entered 64 bit long mode. Starting shitOS...', 0

init_lm:
    mov ax, DATA_SEG64
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rdi, style_blue
    push rdi
    push rax
    push rcx

    shl rdi, 8
    mov rax, rdi
    mov al, space_char

    mov rdi, vga_start
    mov rcx, vga_extent / 2

    rep stosw

    pop rcx
    pop rax
    pop rdi

    mov rsi, msg_long_mode
    call lputs

    call KERNEL_BUFFER

    cli
    hlt

lputs:
    push rax
    push rdx
    push rdi
    push rsi

    mov rdx, vga_start
    shl rdi, 8

.lprint_loop:
    cmp byte[rsi], 0
    je .lprint_loop_done

    mov rax, rdi
    mov al, byte[rsi]
    mov word[rdx], ax

    add rsi, 1
    add rdx, 2

    jmp .lprint_loop

.lprint_loop_done:
    pop rsi
    pop rdi
    pop rdx
    pop rax

    ret
