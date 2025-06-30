org 0x7c00
[bits 16]

jmp short stage1_main
nop

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

stage1_main:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov bp, 0x7c00
    mov sp, bp
    mov ss, ax
    sti

    mov si, msg_loading
    call puts

    mov [drive_number], dl

    xor ax, ax
    int 0x12

    jc .memory_error

    mov bx, 0x7c00 + 1024
    mov dl, [drive_number]

    push dx
    mov ah, 0x02

    mov al, 2
    mov ch, 0
    mov dh, 0
    mov cl, 2

    int 0x13

    jc .disk_error

    pop dx
    cmp al, 2

    jne .incomplete_read

    jmp .stage2

.disk_error:
    mov si, msg_error1
    call puts

    jmp .press_key_reboot

.memory_error:
    mov si, msg_error2
    call puts

    jmp .press_key_reboot

.incomplete_read:
    mov si, msg_error3
    call puts

    jmp .press_key_reboot

.press_key_reboot:
    mov si, msg_error4
    call puts

    mov ah, 0
    int 0x16
    int 0x19

.stage2:
    mov bx, 0x07c0
    mov es, bx
    mov bx, 0x200
    push es
    push bx

    retf

    jmp .press_key_reboot


msg_loading db 'Loading...', 0x0D, 0x0A, 0
msg_error1 db 'Disk failed', 0x0D, 0x0A, 0
msg_error2 db 'No pp, small ram :(', 0x0D, 0x0A, 0
msg_error3 db 'Incomplete read', 0x0D, 0x0A, 0
msg_error4: db 'Press any key to reboot...', 0x0D, 0x0A, 0

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

times 510 - ($ - $$) db 0
dw 0xAA55
