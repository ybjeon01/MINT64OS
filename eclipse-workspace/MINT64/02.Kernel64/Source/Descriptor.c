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
