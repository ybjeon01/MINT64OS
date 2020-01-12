[BITS 32]

; disclose following code to C language
global kReadCPUID, kSwitchAndExecute64bitKernel

SECTION .text

; CPUID operation provides CPU information
; for mode switch, this function is used if computer cpu supports
; 64 bit
; return CPUID to the address pointed by the params
; PARAM: DWORD dwEAX, DWORD *pdwEAX, *pdwEBX, *pdwECX, *pdwEDX
kReadCPUID:
    ; convention for calling function in 32 bit
    push ebp
    mov ebp, esp
    ; saving previously used registers before using them
    push eax
    push ebx
    push ecx
    push edx
    push esi

    ; dwEAX is parameter for CPUID operation
    mov eax, dword [ebp + 8]
    cpuid

    ; moves returned value to memory address referenced by pointers
    mov esi, dword [ebp + 12]
    mov dword [esi], eax

    mov esi, dword [ebp + 16]
    mov dword [esi], ebx

    mov esi, dword [ebp + 20]
    mov dword [esi], ecx

    mov esi, dword [ebp + 24]
    mov dword [esi], edx

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax

; set necessary bits for 64 bit environment such as PAE bit in cr3,
; PML4 table address in cr4, and set msr bit for switching to
; 64 bit mode
kSwitchAndExecute64bitKernel:
    ; remember that you can not directly set bit to control register
    mov eax, cr4
    or eax, 0x20			; set PAE bit (bit 5) to 1
    mov cr4, eax


    mov eax, 0x100000	; PML table address (1MiB)
    mov cr3, eax
