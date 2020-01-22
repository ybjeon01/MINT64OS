#include "Types.h"
#include "Keyboard.h"

void kPrintString(int iX, int iY, const char *pcString);

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

