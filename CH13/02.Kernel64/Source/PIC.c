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
    // ICW3 command: where is slave PIC connected
    // each bit among 8 bits represents pin of master PIC
    // Slave PIC is connected to second pin of master PIC,
    // so 0x04 (second bit)
    kOutPortByte(PIC_MASTER_PORT2, 0x04);

    // ICW4 command: set uPM bit (bit 0) zero
    kOutPortByte(PIC_MASTER_PORT2, 0x01);

    // initialize slave controller
    // ICW1 command
    kOutPortByte(PIC_SLAVE_PORT1, 0x11);

    // ICW2 command, IRQ index ( 0x20 + 8)
    kOutPortByte(PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR + 8);

    // ICW3 command: where is master PIC
    // master PIC index is represented with integer
    // master PIC controller is connected to second pin -> 0x02
    kOutPortByte(PIC_SLAVE_PORT2, 0x02);

    // ICW4 command: set uPM bit (bit 0) zero
    kOutPortByte(PIC_SLAVE_PORT2, 0x01);
}

// mask specific IRQ bit so ignore that IRQ interrupt
void kMaskPICInterrupt(WORD wIRQBitmask) {
    // set IMR to master PIC
	// OCW1 command, mask IRQ 0 ~ IRQ 7
	kOutPortByte(PIC_MASTER_PORT2, (BYTE) wIRQBitmask);

	// set IMR to slave PIC
	// OCW1 command, mask IRQ 8 ~ IRQ 15
	kOutPortByte(PIC_MASTER_PORT2, (BYTE) ( wIRQBitmask >> 8 ));
}

// send PIC signal (EOI) that interrupt is processed
// IRQ0 ~ IRQ7: send EOI to only master PIC
// IRQ8 ~ IRQ 15: send EOI to both of master and slave PIC
void kSendEOIToPIC(int iIRQNumber) {
	// send EOI to master slave
	// OCW2 command: EOI bit (bit 5) = 1
	kOutPortByte(PIC_MASTER_PORT1, 0x20);

	// if IRQ is from IRQ8 to IRQ 15, EOI to slave PIC
	if (iIRQNumber > 7) {
		// OCW2 command: EOI bit (bit 5) = 1
		kOutPortByte(PIC_SLAVE_PORT1, 0x20);
	}
}


