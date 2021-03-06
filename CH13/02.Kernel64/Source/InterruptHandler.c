#include "InterruptHandler.h"
#include "PIC.h"

// common exception handler
void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode) {
    char vcBuffer[3] = {0, };

    // interrupt number as ASCII two-digit number
    vcBuffer[0] = '0' + iVectorNumber / 10;
    vcBuffer[1] = '0' + iVectorNumber % 10;

    kPrintString(0, 0, "==================================================");
    kPrintString(0, 1, "               Exception Occur~!!!!               ");
    kPrintString(0, 2, "                    Vector:                       ");
    kPrintString(27, 2, vcBuffer);
    kPrintString(0, 3, "==================================================");

    while (1);
}

// common Interrupt handler
void kCommonInterruptHandler(int iVectorNumber) {
	char vcBuffer[] = "[INT:  , ]";
	// count how many interrupt occurs
	static int g_iCommonInterruptCount = 0;

	// interrupt number as ASCII two-digit number
	vcBuffer[5] = '0' + iVectorNumber / 10;
	vcBuffer[6] = '0' + iVectorNumber % 10;

	// interrupt count
	vcBuffer[8] = '0' + g_iCommonInterruptCount;
	g_iCommonInterruptCount = (g_iCommonInterruptCount + 1) % 10;
	kPrintString(70, 0, vcBuffer);

	// send EOI to PIC controller
	// reason for iVectorNumber - PIC_IRQSTARTVECTOR is that
	// iVectorNumber is number defined in ISR.
	// iVectorNumber is not IRQ number
	kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
}

// keyboard Interrupt Handler
void kKeyboardHandler(int iVectorNumber) {
    char vcBuffer[] = "[INT:  , ]";
    // count how many interrupt occurs
    static int g_iCommonInterruptCount = 0;

    // interrupt number as ASCII two-digit number
    vcBuffer[5] = '0' + iVectorNumber / 10;
    vcBuffer[6] = '0' + iVectorNumber % 10;

    // interrupt count
    vcBuffer[8] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = (g_iCommonInterruptCount + 1) % 10;
    kPrintString(70, 0, vcBuffer);

    // send EOI to PIC controller
    // reason for iVectorNumber - PIC_IRQSTARTVECTOR is that
    // iVectorNumber is number defined in ISR.
    // iVectorNumber is not IRQ number
    kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
}
