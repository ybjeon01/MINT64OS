; this file is concatenated with c language code object
; in order to make 32 bit kernel and it is at the head
; of the kernel

[ORG 0x00]
[BITS 16]

SECTION .text

; the code in this file is loaded at address 0x10000 on purpose
; because I decided to use the address from 1 MiB to 0x10000
; for 32 bit kernel
START:
    mov ax, 0x1000
    mov ds, ax
    mov es, ax

    ; A20 gate activation using BIOS service
    mov ax, 0x2401
    int 0x15

    jc .A20GATEERROR
    jmp .A20GATESUCCESS

    ; in case that bios A20 service is not supported,
    ; it trys to use system control port
    .A20GATEERROR:
        in al, 0x92
        or al, 0x02
        and al, 0xFE
        out 0x92, al

    ; Load Global Descriptor table register and jump over
    ; to 32 bit mode
    .A20GATESUCCESS:
	    cli

       ; activate control bit for 32 bit mode
       ; disable paging, cache, internal FPU, Align Check
       ; and enable ProtectedMode
	    lgdt [GDTR]
	    mov eax, 0x4000003B		; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1,
	    							; TS=1, EM=0, MP=1, PE=1
	    mov cr0, eax

        ; jump to 32 bit code
	    jmp dword 0x18: (PROTECTEDMODE - $$ + 0x10000)

[BITS 32]

PROTECTEDMODE:
    ; set which descriptor to use in global descriptor table
    ; below number is offset of global descriptor table
    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ss, ax
    ; use below 1 MiB for stack
    ; the reason for using 0xFFFE instead of 0xFFFF is that
    ;
    mov esp, 0xFFFE
    mov ebp, 0xFFFE

    push (SWITCHSUCCESSMESSAGE - $$ + 0x10000)
    push 2
    push 0
    call PRINTMESSAGE
    add esp, 12

     ; code from 0x10200 is C language code
     ; as you expect, I decided to use 0x10200 address for
     ; c language code
     ; keep in mind that EntryPoint file size must be under 512 bytes
     ; 0x18 is offset for CODEDESCRIPTOR
    jmp dword 0x18:0x10200

PRINTMESSAGE:
    push ebp
    mov ebp, esp
    push esi
    push edi
    push eax
    push ecx
    push edx

    mov eax, dword [ebp + 12]
    mov esi, 160
    mul esi
    mov edi, eax

    mov eax, dword [ebp + 8]
    mov esi, 2
    mul esi
    add edi, eax

    mov esi, dword [ebp + 16]

.MESSAGELOOP:
    mov cl, byte [esi]

    cmp cl, 0
    je .MESSAGEEND

    mov byte [edi + 0xB8000], cl
    add edi, 2
    add esi, 1

    jmp .MESSAGELOOP

.MESSAGEEND:
    pop edx
    pop ecx
    pop eax
    pop edi
    pop esi
    pop ebp
    ret

align 8, db 0

; below 0x0000 is for alignment
dw 0x0000

GDTR:
    dw GDTEND - GDT - 1			; Size of GDT - 1
    dd (GDT - $$ + 0x10000)		; start address of GDT



GDT:
    ; GDT's first descriptor must be null descriptor
    ; my memory says that null descriptor is for preventing program
    ; which has a bug from messing up with table. but I need to check
    ; one more what NULL descriptor is for
    NULLDESCRIPTOR:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00

    ; P = check if the descriptor is present (valid)
    ; DPL = descriptor Prileage Level, 0 means hiehgest level
    ; I do not consider security now
    ; G = Granulairy has segment size extension capability
    ; 1 means multipication of descriptor Limit (which is 20 bit)
    ; by 4KiB. G exists because the biggest segment size is 1 MiB
    ; without G bit
    ; D = Default Operation Size. 1 means that the descriptor
    ; is for segment containing 32 bit operations. 0 is for 16 bit
    ; operations
    ; L = 1 means descriptor for 64 bit segment and 0 means
    ; descriptor for IA32-e's 32 bit compatability mode (32 bit)
    ; For 64 bit descriptor , D must be zero, otherwise computer
    ; reboots in QEMU
    IA_32eCODEDESCRIPTOR:
        dw 0xFFFF			; Limit [15:0]
        dw 0x0000			; Base [15:0]
        db 0x00			; Base [23:16]
        db 0x9A			; P=1, DPL=0, Code Segment, allow Execute/Read
        db 0xAF			; G=1, D=0, L=1, Limit[19:16]
        db 0x00			; Base [31:24]

    IA_32eDATADESCRIPTOR:
        dw 0xFFFF			; Limit [15:0]
        dw 0x0000			; Base [15:0]
        db 0x00			; Base [23:16]
        db 0x92			; P=1, DPL=0, Data Segment, allow Execute/Read
        db 0xAF			; G=1, D=0, L=1, Limit[19:16]
        db 0x00			; Base [31:24]

    CODEDESCRIPTOR:
        dw 0xFFFF			; Limit [15:0]
        dw 0x0000			; Base [15:0]
        db 0x00			; Base [23:16]
        db 0x9A			; P=1, DPL=0, Code Segment, allow Execute/Read
        db 0xCF			; G=1, D=1, L=0, Limit[19:16]
        db 0x00			; Base [31:24]

    DATADESCRIPTOR:
        dw 0xFFFF			; Limit [15:0]
        dw 0x0000			; Base [15:0]
        db 0x00			; Base [23:16]
        db 0x92			; P=1, DPL=0, Data Segment, allow Execute/Read
        db 0xCF			; G=1, D=1, L=0, Limit[19:16]
        db 0x00			; Base [31:24]
GDTEND:

SWITCHSUCCESSMESSAGE: db "Switch To Protected Mode Success~!!", 0

; Since entry point is loaded at 0x10000 and
; the entryPoint code assumes that c language code
; is at 0x10200, entryPoint code size must be
; less or equal to 512 bytes

times 512 - ($ - $$) db 0x00

