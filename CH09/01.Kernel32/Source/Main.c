#include "Types.h"
#include "Page.h"

void kPrintString(int iX, int iY, const char *pcString);
BOOL kIsMemoryEnough(void);
BOOL kInitializeKernel64Area(void);
void kPrintHex(int iX, int iY, DWORD value);


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

	kPrintString(45, 4, "Pass");
    kPrintString(0, 5, "IA-32e Kernel Area Initialize...............[    ]");

	if (kInitializeKernel64Area() == FALSE) {
		kPrintString(45, 5, "Fail");
		kPrintString(0, 6, "Kernel Area Initialization Fail~!!");
		while (1);
	}

    kPrintString(45, 5, "Pass");
    kPrintString(0, 6, "IA-32e Page Tables initialize...............[    ]");
    kInitializePageTables();
    kPrintString(45, 6, "Pass");
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

// initializes all memory area from 6MB to 1MB to zero
// for 64 bit kernel code and its data area
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

// Since our Kernel requires at least 64MB, it checks
// if computer has 64MB
BOOL kIsMemoryEnough(void) {
	DWORD *pdwCurrentAddress;

	pdwCurrentAddress = (DWORD *) 0x100000;

	while (pdwCurrentAddress < (DWORD *) 0x4000000) {
       // if we writes data to invalid address, we
		// cannot get the same data from the address
		*pdwCurrentAddress = 0x12345678;
       if (*pdwCurrentAddress != 0x12345678) {
           return FALSE;
        }

       // we checks only address divisible by 1 MB
       pdwCurrentAddress += (0x100000 / 4);
	}
	return TRUE;
}

BYTE IntegerToHex(BYTE integer);

void kPrintHex(int iX, int iY, DWORD value) {
    int i = 7;

	while (i >= 0) {
		// moving part of 4 bits to rightmost side
       BYTE ch = value >> (i * 4);
       // we need only lower 4 bits for translating
       //binary to hex it changes all upper bits to 0
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




