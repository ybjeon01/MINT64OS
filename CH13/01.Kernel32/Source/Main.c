#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString(int iX, int iY, const char *pcString);
BOOL kIsMemoryEnough(void);
BOOL kInitializeKernel64Area(void);
void kCopyKernel64ImageTo2Mbyte(void);

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

    // copy IA-32e mode kernel in somewhere under address 1MiB
    // to 0x200000 (2Mbyte)
    kPrintString(0, 9, "Copy IA-32e Kernel To 2M Address............[    ]");
    kCopyKernel64ImageTo2Mbyte();
    kPrintString(45, 9, "Pass");


    // convert to IA-32e mode
    kPrintString(0, 9, "Switch To IA-32e Mode");
    kSwitchAndExecute64bitKernel();

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

// Copy IA-32e mode kernel somewhere under address 1MB
// to 0x20000 (2Mbyte)
void kCopyKernel64ImageTo2Mbyte(void) {
    WORD wKernel32SectorCount, wTotalKernelSectorCount;
    DWORD * pdwSourceAddress, *pdwDestinationAddress;
    int i;
    int iKernel64ByteCount;

    // count of overall sectors except booloader is at
    // 0x7c05 and count of protected mode kernel is at
    // 0x7c07
    wTotalKernelSectorCount = *((WORD *) 0x7c05);
    wKernel32SectorCount = *((WORD *) 0x7c07);

    pdwSourceAddress = (DWORD *) (0x10000 + (wKernel32SectorCount * 512));
    pdwDestinationAddress = (DWORD *) 0x200000;

    iKernel64ByteCount = 512 * (wTotalKernelSectorCount - wKernel32SectorCount);
    for (i = 0; i < iKernel64ByteCount / 4; i++) {
    	// copy 4 bytes at once
    	*pdwDestinationAddress = *pdwSourceAddress;
    	pdwDestinationAddress++;
    	pdwSourceAddress++;
    }
}





