[BITS 64]

SECTION .text

; read a byte from port
; or write a byte to port
global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR

; read one byte from port
; param: port address
kInPortByte:
    push rdx			; store rdx register that will be temporarily used
    mov rax, 0		; initialize register to zero
    mov rdx, rdi		; move first parameter (port addr) to rax
    in al, dx			; read byte

    pop rdx
    ret


; write one byte to port
; param: port address, data
kOutPortByte:
    push rdx
    push rax

    mov rdx, rdi		; move first parameter (port addr) to rdi
    mov rax, rsi		; move second parameter (data) to rax
    out dx, al		; write byte

    pop rax
    pop rdx
    ret

; set GDT address to GDTR
; param: GDT start address
kLoadGDTR:
    lgdt [rdi]
    ret

; set TSS segment descirptor offset to TR register
; param: TSS segment descriptor offset
kLoadTR:
    ltr di;
    ret

; set IDT table start address to IDTR register
; param: IDT start address
kLoadIDTR:
    lidt [rdi]
    ret
