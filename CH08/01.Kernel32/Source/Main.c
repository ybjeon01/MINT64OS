#include "Types.h"

void kPrintString(int iX, int iY, const char *pcString);
BOOL kIsMemoryEnough(void);
BOOL kInitializeKernel64Area(void);
void kPrintHex(int iX, int iY, DWORD value);
BYTE IntegerToHex(BYTE integer);

void Main(void) {
	kPrintString(0, 3, "C Language Kernel Started~!!!...............[Pass]");

	// check if system satisfies minimum memory requirement
	kPrintString(0, 4, "Minimum Memory Size Check...................[    ]");
	if (kIsMemoryEnough() == FALSE) {
        kPrintString(45, 4, "Fail");
        kPrintString(0, 5, "Not Enough Memory~!! MINT64 OS Requires Over "
        		"64Mbyte Memory~!!");
        while (1);
	}
	else {
		kPrintString(45, 4, "Pass");
	}

	if (kInitializeKernel64Area() == FALSE) {
		kPrintString(45, 5, "Fail");
		kPrintString(0, 6, "Kernel Area Initialization Fail~!!");
		while (1);
	}
	else {
		kPrintString(45, 5, "Pass");
	}

	while (1);

}

void kPrintString(int iX, int iY, const char *pcString) {
	CHARACTER *pstScreen = (CHARACTER *) 0xB8000;
    int i;

    pstScreen += iY * 80 + iX;

    for (i = 0; pcString[i] != 0; i++) {
    	pstScreen[i].bCharacter = pcString[i];
    }
}

BOOL kInitializeKernel64Area(void) {
	DWORD *pdwCurrentAddress;
	pdwCurrentAddress = (DWORD *) 0x100000;

	while ((DWORD) pdwCurrentAddress < 0x600000) {
        *pdwCurrentAddress = 0x00;
        if (*pdwCurrentAddress != 0) {
        	return FALSE;
        }
        pdwCurrentAddress++;
	}
	return TRUE;
}

BOOL kIsMemoryEnough(void) {
	DWORD *pdwCurrentAddress;

	pdwCurrentAddress = (DWORD *) 0x100000;

	while (pdwCurrentAddress < (DWORD *) 0x4000000) {
       *pdwCurrentAddress = 0x12345678;
       if (*pdwCurrentAddress != 0x12345678) {
           return FALSE;
        }

       pdwCurrentAddress += (0x100000 / 4);
	}
	return TRUE;
}

void kPrintHex(int iX, int iY, DWORD value) {
    int i = 7;

	while (i >= 0) {
       BYTE ch = value >> (i * 4);
       ch = ch & 0x0F;
		ch = IntegerToHex(ch);
       kPrintString(iX, iY, (const char *) &ch);
       iX++;
       i--;
	}
}

BYTE IntegerToHex(BYTE integer) {
    if (integer >= 10) {
    	return integer + 55;
    }
    else {
    	return integer + 48;
    }
}




