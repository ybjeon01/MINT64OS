; ISR file defines interrupt handlers that is registered to IDT table

[BITS 64]

SECTION .text

; it is like #include <header>
extern kCommonExceptionHandler, kCommonInterruptHandler, kKeyboardHandler

; ISR for processing exceptions
global kISRDivideError, kISRDebug, kISRNMI, kISRBreakPoint, kISROverflow
global kISRBoundRangeExceeded, kISRInvalidOpcode, kISRDeviceNotAvailable
global kISRDoubleFault
global kISRCoprocessorSegmentOverrun, kISRInvalidTSS, kISRSegmentNotPresent
global kISRStackSegmentFault, kISRGeneralProtection, kISRPageFault, kISR15
global kISRFPUError, kISRAlignmentCheck, kISRMachineCheck, kISRSIMDError
global kISRETCException

; ISR for processing interrupt
global kISRTimer, kISRKeyboard, kISRSlavePIC, kISRSerial2, kISRSerial1, kISRParallel2
global kISRFloppy, kISRParallel1, kISRRTC, kISRReserved, kISRNotUsed1, kISRNotUsed2
global kISRMouse, kISRCoprocessor, kISRHDD1, kISRHDD2, kISRETCInterrupt

; macro that saves context and changes selector
%macro KSAVECONTEXT 0			; KSAVECONTEXT does not accept parameter
    ; push registers and segments to stack
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; cannot directly push ds and es to stack
    mov ax, ds
    push rax
    mov ax, es
    push rax

    push fs
    push gs

    ; change segment selector
    mov ax, 0x10			; 0x10: kernel data segmenet descriptor offset
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
%endmacro

; macro that restores context
%macro KLOADCONTEXT 0			; KLOADCONTEXT does not accept parameter
    ; restore all registers and selectors that was pushed to stack
    pop gs
    pop fs
    pop rax
    mov es, ax
    pop rax
    mov ds, ax
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
 %endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Exception Handlers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; #0, Divide Error ISR
kISRDivideError:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    mov rdi, 0
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #1, Debug ISR
kISRDebug:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    mov rdi, 1
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #2, NMI ISR
kISRNMI:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    mov rdi, 2
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #3, BreakPoint ISR
kISRBreakPoint:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    mov rdi, 4
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #4, Overflow ISR
kISROverflow:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    mov rdi, 4
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #5, Bound Range Exceeded ISR
kISRBoundRangeExceeded:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    mov rdi, 5
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #6, Invalid Opcode ISR
kISRInvalidOpcode:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    mov rdi, 6
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #7, Device not Available ISR
kISRDeviceNotAvailable:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    mov rdi, 7
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #8, Double Fault ISR
kISRDoubleFault:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 8
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    add rsp, 8      ; remove error code from stack
    iretq           ; complete interrupt processing and return to previous running code

; #9, Coprocessor Segment Overrun ISR
kISRCoprocessorSegmentOverrun:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    mov rdi, 9
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #10, Invalid TSS ISR
kISRInvalidTSS:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 10
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    add rsp, 8      ; remove error code from stack
    iretq           ; complete interrupt processing and return to previous running code

; #11, Segment Not Present ISR
kISRSegmentNotPresent:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 11
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    add rsp, 8      ; remove error code from stack
    iretq           ; complete interrupt processing and return to previous running code

; #12, Stack Segment Fault ISR
kISRStackSegmentFault:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 12
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    add rsp, 8      ; remove error code from stack
    iretq           ; complete interrupt processing and return to previous running code

; #13 General Protection ISR
kISRGeneralProtection:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 13
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    add rsp, 8      ; remove error code from stack
    iretq           ; complete interrupt processing and return to previous running code

; #14, Page Fault ISR
kISRPageFault:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 14
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    add rsp, 8      ; remove error code from stack
    iretq           ; complete interrupt processing and return to previous running code

; #15, Reserved ISR
kISR15:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 15
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #16, FPU Error ISR
kISRFPUError:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 16
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #17, Alignment Check ISR
kISRAlignmentCheck:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 17
    mov rsi, qword [rbp + 8]
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    add rsp, 8      ; remove error code from stack
    iretq           ; complete interrupt processing and return to previous running code

; #18, Machine Check ISR
kISRMachineCheck:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 18
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #19, SIMD Floating Point Exception ISR
kISRSIMDError:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 19
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #20 ~ #31, Reserved ISR
kISRETCException:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 20
    call kCommonExceptionHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Interrupt Handlers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; #32, Timer ISR

kISRTimer:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 32
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #33, Keyboard ISR
kISRKeyboard:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 33
    call kKeyboardHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #34, slave PIC ISR
kISRSlavePIC:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 34
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #35, serial port 2 ISR
kISRSerial2:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 35
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #36, serial port 1 ISR
kISRSerial1:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 36
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #37, parallel port 2 ISR
kISRParallel2:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 37
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #38, floppy disk controller ISR
kISRFloppy:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 38
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #39, parallel port 1 ISR
kISRParallel1:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 39
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #40, RTC ISR
kISRRTC:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 40
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #41, reserved interrupt ISR
kISRReserved:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 41
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #42, not used
kISRNotUsed1:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 42
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #43, not used
kISRNotUsed2:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 43
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #44, mouse ISR
kISRMouse:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 44
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #45 coprocessor ISR
kISRCoprocessor:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 45
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #46, hard disk 1 ISR
kISRHDD1:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 46
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; #47, hard disk 2 ISR
kISRHDD2:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 47
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code

; # 48, undefined interrupt ISR
; handles all interrupts that is not defined in ISR
kISRETCInterrupt:
    KSAVECONTEXT    ; save context and change selector to kernel data descriptor
    ; exception number as first parameter
    ; error code as second parameter
    mov rdi, 48
    call kCommonInterruptHandler
    KLOADCONTEXT    ; restore context
    iretq           ; complete interrupt processing and return to previous running code











