#include "PIC.h"
#include "AssemblyUtility.h"

// initialize PIC
void kInitializePIC(void) {
	// initialize master PIC
	// PIC consider first data sent through PORT1 as ICW1 command and
	// expects second to fourth data will be sent to PORT2.
	// second: ICW2 command, third: ICW3 command, fourth: ICW4 command

	// ICW1 command
	kOutPortByte(PIC_MASTER_PORT1, 0x11);
	// ICW2 command: start index of IRQ in IDT
	kOutPortByte(PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR);

    kOutPortByte(PIC_MASTER_PORT2, 0x04);

}
