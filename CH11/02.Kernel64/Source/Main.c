#include "Types.h"
#include "Keyboard.h"

void kPrintString(int iX, int iY, const char *pcString);
void kPrintHex(int iX, int iY, DWORD value);
void kClearString(int iX, int iY, int length);

void Main(void) {
	// below four variables are for keyboard typing
	char vcTemp[2] = {0, };
	BYTE bFlags;
	BYTE bTemp;
	int i =0;

    kPrintString(0, 10, "Switch To IA-32e Mode Success~!!");
    kPrintString(0, 11, "IA-32e C Language Kernel Start..............[PASS]");
    kPrintString(0, 12, "Keyboard Activate...........................[    ]");

    // activate keyboard
    // if keyboard input buffer is full of data for 0xFFFF time, then
    // it fails or if keyboard does not respond to activation code with
    // ACK code, then it also fails. there is one condition that it may
    // fails, but it is really rare. for the detail, read code
    if (kActivateKeyboard()) {
    	kPrintString(45, 12, "Pass");
    	// set Num Lock, Caps Lock, Scroll Lock off
    	kChangeKeyboardLED(FALSE, FALSE, FALSE);
    }
    else {
    	kPrintString(45, 12, "Fail");
    	while (1);
    }

   // read keyboard and write to monitor
    while (TRUE) {



    	// if keyboard output buffer (0x60) is full then you can read
    	// scan code
    	if (kIsOutputBufferFull()) {
    		// read from 0x60 port
    		bTemp = kGetKeyboardScanCode();

    		// convert scan code to ASCII code with detail info
    		// for example if user pressed or released key or input is
    		// special key
            if (kConvertScanCodeToASCIICode(bTemp, &(vcTemp[0]), &bFlags)) {
            	if (bFlags & KEY_FLAGS_DOWN) {
					kPrintString(i++, 13, vcTemp);
				}
			}
    	}

    }
}

void kPrintString(int iX, int iY, const char *pcString) {
	CHARACTER *pstScreen = (CHARACTER *) 0xB8000;
    int i;

    pstScreen += iY * 80 + iX;

    for (i = 0; pcString[i] != 0; i++) {
    	pstScreen[i].bCharacter = pcString[i];
    }
}

void kClearString(int iX, int iY, int length) {
	CHARACTER *pstScreen = (CHARACTER *) 0xB8000;
	int i;

    pstScreen += iY * 80 + iX;
    for (i = 0; i < length; i++) {
    	pstScreen[i].bCharacter = 0;
    	pstScreen[i].bAttribute = 0x0A;
    }
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
