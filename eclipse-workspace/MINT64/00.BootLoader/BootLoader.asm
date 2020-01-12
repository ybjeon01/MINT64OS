[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:START

; Total Sector Count is modified after compiling and linking.
; Utility Tool, ImageMaker measure the size of the overall binary file
; and change the TOTALSECTORCOUNT based on the size
TOTALSECTORCOUNT: dw 0

START:
    mov ax, 0x07C0
    mov ds, ax
    mov ax, 0xB800
    mov es, ax

    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFE
    mov bp, 0xFFFE

mov si, 0
.SCREANCLEARLOOP:
    mov byte [es:si], 0
    mov byte [es:si + 1], 0x0A
    add si, 2
    cmp si, 80 * 25 * 2

    jl .SCREANCLEARLOOP


push MESSAGE1
push 0
push 0
call PRINTMESSAGE
add sp, 6

push IMAGELOADINGMESSAGE
push 1
push 0
call PRINTMESSAGE
add sp, 6

RESETDISK:
    mov ah, 0
    mov dl, 0
    int 0x13
    jc HANDLEDISKERROR

    ; set 0x1000 for data segment and 0x0 for destination
    ; of data to be loaded so that final destination is 1 MiB
    ; 1MiB address is for 32 bit mode kernel code
    mov si, 0x1000
    mov es, si
    mov bx, 0x0000

mov di, word [TOTALSECTORCOUNT]
READDATA:
    cmp di, 0
    je READEND
    sub di, 0x1

    mov ah, 0x2
    mov al, 0x1
    mov ch, byte [TRACKNUMBER]
    mov cl, byte [SECTORNUMBER]
    mov dh, byte [HEADNUMBER]
    mov dl, 0x00
    int 0x13
    jc HANDLEDISKERROR

    add si, 0x0020
    mov es, si

    mov al, [SECTORNUMBER]
    add al, 0x01;
    mov byte [SECTORNUMBER], al

    cmp ax, 37
    jl READDATA

    xor byte [HEADNUMBER], 0x01
    mov byte [SECTORNUMBER], 0x01

    cmp byte [HEADNUMBER], 0x00
    jne READDATA

    add byte [TRACKNUMBER], 0x01
    jmp READDATA

READEND:
    push LOADINGCOMPLETEMESSAGE
    push 1
    push 20
    call PRINTMESSAGE
    add sp, 6

    jmp 0x1000:0x0000


HANDLEDISKERROR:
    push DISKERRORMESSAGE
    push 1
    push 20
    jmp $


PRINTMESSAGE:
    push bp
    mov bp, sp

    push es
    push si
    push di
    push ax
    push cx
    push dx

    mov ax, 0xB800
    mov es, ax

    mov ax, word [bp + 6]
    mov si, 160
    mul si
    mov di, ax

    mov ax, word [bp + 4]
    mov si, 2
    mul si
    add di, ax

    mov si, word [bp + 8]

.MESSAGELOOP:
    mov cl, byte [si]
    cmp cl, 0
    je .MESSAGEEND

    mov byte [es:di], cl
    add si, 1
    add di, 2

    jmp .MESSAGELOOP

.MESSAGEEND:
    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret

MESSAGE1: db "MINT64 OS Boot Loader Start~!!", 0
DISKERRORMESSAGE: db 'DISK Error~!!', 0
IMAGELOADINGMESSAGE: db 'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db 'Complete~!!', 0

TRACKNUMBER: db 0x00
HEADNUMBER: db 0x00
SECTORNUMBER: db 0x02

times 510 - ($ - $$) db 0x00

db 0x55
db 0xAA