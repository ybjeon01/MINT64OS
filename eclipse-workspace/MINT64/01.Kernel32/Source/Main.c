#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString(int iX, int iY, const char *pcString);
BOOL kIsMemoryEnough(void);
BOOL kInitializeKernel64Area(void);
void kPrintHex(int iX, int iY, DWORD value);


void Main(void) {
	DWORD i;
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	char vcVendorString[13] = {0, };


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

    // read processor manufacturer info by reading model specific register
    // at 0x00
    kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    // result of rdmsr is stored at eax, ebx, edx, ecx in order
    *(DWORD *)vcVendorString = dwEBX;
    *((DWORD *)vcVendorString + 1) = dwEDX;
    *((DWORD *)vcVendorString + 2) = dwECX;

    kPrintString(0, 7, "Processor Vendor String"
    		           ".....................[            ]");
    kPrintString(45, 7, vcVendorString);

    // check if CPU supports 64 bit mode
    kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);

    kPrintString(0, 8, "64bit Mode Support Check....................[    ]");
    if (dwEDX & (1 << 29)) {
    	kPrintString(45, 8, "Pass");
    }
    else {
    	kPrintString(45, 8, "Fail");
    	kPrintString(0, 9, "This processor does not support 64 bit mode~!!");
    	while (1);
    }

    // convert to IA-32e mode
    kPrintString(0, 9, "Switch To IA-32e Mode");
    // currently I do not have 64 bit kernel code,
    // so I commented below function
    // stoped at page 293
    // kSwitchAndExecute64BitKernel();

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




