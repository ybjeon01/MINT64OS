#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

// read one byte from port address
BYTE kInPortByte(WORD wPort);
void kOutPortByte(WORD wPort, BYTE bData);

#endif /* __ASSEMBLYUTILITY_H__ */
