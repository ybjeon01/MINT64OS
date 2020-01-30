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
}
