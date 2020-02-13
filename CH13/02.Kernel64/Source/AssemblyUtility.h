#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

// read one byte from port address
BYTE kInPortByte(WORD wPort);
void kOutPortByte(WORD wPort, BYTE bData);

// load GDTR, TSR, ISTR
void kLoadGDTR(QWORD qwGDTRAddress);
void kLoadTR(WORD wTSSSegmentOffset);
void kLoadIDTR(QWORD qwIDTRAddress);

// activate or deactivate interrupt
// read from RFLAGS register
void kEnableInterrupt(void);
void kDisableInterrupt(void);
QWORD kReadRFLAGS(void);

#endif /* __ASSEMBLYUTILITY_H__ */
