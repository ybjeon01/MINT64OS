[BITS 64]

Section .text

extern Main

START:
    mov ax, 0x10					; IA-32e mode's data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; address from 0x600000~0x6FFFFF (1MiB) is for stack
    mov ss, ax
    mov rsp, 0x6FFFF8
    mov rbp, 0x6FFFF8

    call Main

    jmp $							; never be executed
