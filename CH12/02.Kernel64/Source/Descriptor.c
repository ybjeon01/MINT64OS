// In chapter 11, I implemented keyboard controller code
// However, it uses 100 % of CPU since the code use
// while loop without interrupt. Here I implement Task state
// segment and Interrupt descriptor table for interrupt-based
// keyboard control
//
// Since GDT table becomes bigger, I make new global descriptor
// table in 1MB ~ 2MB area


#include "Descriptor.h"
#include "Utility.h"

// initialize Global Descriptor table which includes
// Task state segment descriptor
void kInitializeGDTTableAndTSS(void) {
    GDTR *pstGDTR;
    GDTENTRY8 *pstEntry;
    TSSSEGMENT *pstTSS;
    int i;

    // set GDTR
    pstGDTR = (GDTR *) GDTR_STARTADDRESS;
    pstEntry = (GDTENTRY8 *) (GDTR_STARTADDRESS + sizeof(GDTR));
    pstGDTR->wLimit = GDT_TABLESIZE - 1;
    pstGDTR->qwBaseAddress = (QWORD) pstEntry;

    // TSS segment
    pstTSS = (TSSSEGMENT *) ((QWORD) pstEntry + GDT_TABLESIZE);

    // NULL, 64 bits, Code/Data, TSS segments
    kSetGDTEntry8( &(pstEntry[0]), 0, 0, 0, 0, 0);

    kSetGDTEntry8( &(pstEntry[1]), 0, 0xFFFFF, GDT_FLAGS_UPPER_CODE,
    		GDT_FLAGS_LOWER_KERNELCODE, GDT_TYPE_CODE);

    kSetGDTEntry8 ( &(pstEntry[1]), 0, 0xFFFFF, GDT_FLAGS_UPPER_DATA,
    		GDT_FLAGS_LOWER_KERNELDATA, GDT_TYPE_CODE);

    kSetGDTEntry16 ( (GDTENTRY16 *) &(pstEntry[3]), (QWORD) pstTSS,
    		sizeof(TSSSEGMENT) - 1, GDT_FLAGS_UPPER_TSS, GDT_FLAGS_LOWER_TSS,
			GDT_TYPE_TSS);

    // initialize TSS segment
    kInitializeTSSSegment(pstTSS);
}

// set GDT entry of 8 bytes.
// this function can be used for setting data and code segments
void kSetGDTEntry8(GDTENTRY8 *pstEntry, DWORD dwBaseAddress, DWORD dwLimit,
		BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType) {
    pstEntry->wLowerLimit = dwLimit & 0xFFFF;
    pstEntry->wLowerBaseAddress = dwBaseAddress & 0xFFFF;
    pstEntry->bUpperBaseAddress1 = (dwBaseAddress >> 16) & 0xFF;
    pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
    pstEntry->bUpperLimitAndUpperFlag = ( (dwLimit >> 16) & 0x0F ) |
    		bUpperFlags;
    pstEntry->bUpperBaseAddress2 = ( dwBaseAddress >> 24 ) & 0xFF;
}

// set GDT entry of 16 bytes
// this function is used for setting TSS descriptor
void kSetGDTEntry16(GDTENTRY16 *pstEntry, QWORD qwBaseAddress, DWORD dwLimit,
		BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType) {
    pstEntry->wLowerLimit = dwLimit & 0xFFFF;
    pstEntry->wLowerBaseAddress = qwBaseAddress & 0xFFFF;
    pstEntry->bMiddleBaseAddress1 = (qwBaseAddress >> 16) & 0xFF;
    pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
    pstEntry->bUpperLimitAndUpperFlag = ( ( dwLimit >> 16 ) & 0xFF ) |
    		bUpperFlags;
    pstEntry->bMiddleBaseAddress2 = ( qwBaseAddress >> 24 ) & 0xFF;
    pstEntry->dwUpperBaseAddress = qwBaseAddress >> 32;
    pstEntry->dwReserved = 0;
}

// initialize Task State Segment
void kInitializeTSSSegment(TSSSEGMENT *pstTSS) {
	kMemSet(pstTSS, 0, sizeof(TSSSEGMENT));
	pstTSS->qwIST[0] = IST_STARTADDRESS + IST_SIZE;
	// by setting IO bigger than TSS limit, IO Mapping is not used
	pstTSS->wIOMapBaseAddress = 0xFFFF;
}

/*
 * Interrupt Descriptor Table
 */

// initialize Interrupt Descriptor Table
void kInitializeIDTTables(void) {
    IDTR *pstIDTR;
    IDTENTRY *pstEntry;
    int i;

    // start address of IDTR
    pstIDTR = (IDTR *) IDTR_STARTADDRESS;

    // create Interrupt Descriptor Table
    pstEntry = (IDTENTRY *) (IDTR_STARTADDRESS + sizeof(IDTR));
    pstIDTR->qwBaseAddress = (QWORD) pstEntry;
    pstIDTR->wLimit = IDT_TABLESIZE - 1;

    // From 0 to 99 in the table, set DummyHandler
    for (i = 0; i < IDT_MAXENTRYCOUNT; i++) {
    	kSetIDTEntry( &(pstEntry[i]), kDummyHandler, 0x08, IDT_FLAGS_IST1,
    			IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    }
}

// set IDT Gate Descriptor
void kSetIDTEntry( IDTENTRY *pstEntry, void *pvHandler, WORD wSelector,
		BYTE bIST, BYTE bFlags, BYTE bType) {
    pstEntry->wLowerBaseAddress = (QWORD) pvHandler & 0xFFFF;
    pstEntry->wSegmentSelector = wSelector;
    // check below two codes in detail
    pstEntry->bIST = bIST & 0x3;
    pstEntry->bTypeAndFlags = bType | bFlags;
    pstEntry->wMiddleBaseAddress = ( (QWORD) pvHandler >> 16 ) & 0xFFFF;
    pstEntry->dwUpperBaseAddress = (QWORD) pvHandler >> 32;
    pstEntry->dwReserved = 0;
}

// temporary handler for interrupt test
void kDummyHandler(void) {
    kPrintString(0, 17, "==================================================");
    kPrintString(0, 18, "       Dummy Interrupt Handler Execute~!!!        ");
    kPrintString(0, 19, "        Interrupt or Exception Occur~!!!!         ");

    while (1);
}












