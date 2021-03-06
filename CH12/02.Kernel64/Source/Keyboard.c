#include "Types.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"

BOOL sendCommandToKeyboard(BYTE command);

// check if output buffer (0x60) is full by using keyboard controller
BOOL kIsOutputBufferFull(void) {
	// read state register from keyboard controller
	// if bit 0 is set to 1, keyboard sent data
	// to output buffer
	if (kInPortByte(0x64) & 0x01) {
		return TRUE;
	}
	return FALSE;
}

// check if input buffer (0x60) is full by using keyboard controller
// you might think why input buffer and output buffer has the
// same port address. Keyboard controller returns output buffer
// if you read from the address, and saves data from user to
// input buffer if you write to the address. It is how it works
BOOL kIsInputBufferFull(void) {
	// read state register from keyboard controller
	// if bit 1 is set to 1, it means input buffer
	// is full
	if (kInPortByte(0x64) & 0x02) {
		return TRUE;
	}
	return FALSE;
}


// notify keyboard controller that user is going to use
// keyboard and also send command to keyboard to activate
// keyboard
BOOL kActivateKeyboard(void) {
    int i, j;

    // notify keyboard controller that user is going to
    // use keyboard device by sending 0xAE command to
    // control register
    // info: reading from 0x64 returns state register and
    //       writing to 0x64 means writing to control register
    kOutPortByte(0x64, 0xAE);

    // keyboard activation command to keyboard
    return sendCommandToKeyboard(0xF4);

}

// read byte from output buffer
// attention: if there is no data in output buffer
//            computer freeze
BYTE kGetKeyboardScanCode(void) {
    while (!kIsOutputBufferFull());

    return kInPortByte(0x60);
}

// set keyboard LED on/off
BOOL kChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn,
		BOOL bScrollLockOn) {

    BOOL command;
    if (sendCommandToKeyboard(0xED) == FALSE) {
        return FALSE;
    }

    command = (bCapsLockOn << 2 | bNumLockOn << 1 | bScrollLockOn);
    return sendCommandToKeyboard(command);
}

// send command to keyboard
// if keyboard failed to execute the command, it returns
// FALSE.
BOOL sendCommandToKeyboard(BYTE command) {
    int i, j;

    // it is possible that when you want to send data to
    // keyboard, input buffer is already full. so it loops as
    // many as 0xFFFF until input buffer is flushed to keyboard
    // if input buffer is full
    for (i = 0; i < 0xFFFF; i++) {
        if (kIsInputBufferFull() == FALSE) {
            break;
        }
    }
    if (i == 0xFFFF) {
        return FALSE;
    }

    // write keyboard command to input buffer
    kOutPortByte(0x60, command);

    // check if keyboard took command from input buffer
    for (i = 0; i < 0xFFFF; i++) {
        if (kIsInputBufferFull() == FALSE) {
            break;
        }
    }
    if (i == 0xFFFF) {
        return FALSE;
    }

    // wait until ACK (acknowledge) comes back from keyboard
    // keyboard always reply to command
    // just in case keyboard output buffer (0x60) already has
    // data, we accept up to 100 data from output buffer so
    // we can check if keyboard replied to our command
    // since we do not use keyboard before keyboard activation,
    // all data in output buffer and input buffer are junk data.
    for (j = 0; j < 100; j++) {

        // wait for 0xFFFF time until output buffer becomes ready to read
        // because it requires time for controller to fill data to output
        // buffer after user read data from output or keyboard executes
        // command
        for (i = 0; i < 0xFFFF; i++) {
            if (kIsOutputBufferFull()) {
                break;
              }
         }

         // check if keyboard sent ACK (0xFA)
        if (kInPortByte(0x60) == 0xFA) {
            return TRUE;
         }
    }
    return FALSE;
}

// if you set 0xD0 to controller register
// you can read output port buffer from output buffer
// if you set 0xD1 to control register
// you can write output port buffer using input buffer

void kEnableA20Gate(void) {
    BYTE kOutputPortData;
    int i;

    // this code set you can read output port data from
    // output buffer
    kOutPortByte(0x64, 0xD0);

    for (i = 0; i < 0xFFFF; i++) {
        if (kIsOutputBufferFull()) {
            break;
        }
    }

    // read output port data
    kOutputPortData = kInPortByte(0x60);
    // set A20 gate
    kOutputPortData |= 0x01;

    for (i = 0; i < 0xFFFF; i++) {
        if (kIsInputBufferFull() == FALSE) {
            break;
        }
    }

    // write to output port
    // send input buffer to output port
    kOutPortByte(0x64, 0xD1);
    kOutPortByte(0x60, kOutputPortData);
}

void kReboot(void) {
    int i;
    for (i = 0; i < 0xFFFF; i++) {
        if (kIsInputBufferFull() == FALSE) {
            break;
        }
    }

    // Write to output port
    // send input buffer to output port
    kOutPortByte(0x64, 0xD1);
    // reset processor
    kOutPortByte(0x60, 0x00);

    while (1);
}

///////////////////////////////////////////////////////////////////////////////
// table that convert scan code to ASCII code and related functions
///////////////////////////////////////////////////////////////////////////////

// keyboard manager that stores states of keyboard
static KEYBOARDMANAGER gs_stKeyboardManager = {0, };

// table for converting scan code to ASCII code
static KEYMAPPINGENTRY gs_vstKeyMappingTable[ KEY_MAPPINGTABLEMAXCOUNT ] =
{
    /*  0   */  {   KEY_NONE        ,   KEY_NONE        },
    /*  1   */  {   KEY_ESC         ,   KEY_ESC         },
    /*  2   */  {   '1'             ,   '!'             },
    /*  3   */  {   '2'             ,   '@'             },
    /*  4   */  {   '3'             ,   '#'             },
    /*  5   */  {   '4'             ,   '$'             },
    /*  6   */  {   '5'             ,   '%'             },
    /*  7   */  {   '6'             ,   '^'             },
    /*  8   */  {   '7'             ,   '&'             },
    /*  9   */  {   '8'             ,   '*'             },
    /*  10  */  {   '9'             ,   '('             },
    /*  11  */  {   '0'             ,   ')'             },
    /*  12  */  {   '-'             ,   '_'             },
    /*  13  */  {   '='             ,   '+'             },
    /*  14  */  {   KEY_BACKSPACE   ,   KEY_BACKSPACE   },
    /*  15  */  {   KEY_TAB         ,   KEY_TAB         },
    /*  16  */  {   'q'             ,   'Q'             },
    /*  17  */  {   'w'             ,   'W'             },
    /*  18  */  {   'e'             ,   'E'             },
    /*  19  */  {   'r'             ,   'R'             },
    /*  20  */  {   't'             ,   'T'             },
    /*  21  */  {   'y'             ,   'Y'             },
    /*  22  */  {   'u'             ,   'U'             },
    /*  23  */  {   'i'             ,   'I'             },
    /*  24  */  {   'o'             ,   'O'             },
    /*  25  */  {   'p'             ,   'P'             },
    /*  26  */  {   '['             ,   '{'             },
    /*  27  */  {   ']'             ,   '}'             },
    /*  28  */  {   '\n'            ,   '\n'            },
    /*  29  */  {   KEY_CTRL        ,   KEY_CTRL        },
    /*  30  */  {   'a'             ,   'A'             },
    /*  31  */  {   's'             ,   'S'             },
    /*  32  */  {   'd'             ,   'D'             },
    /*  33  */  {   'f'             ,   'F'             },
    /*  34  */  {   'g'             ,   'G'             },
    /*  35  */  {   'h'             ,   'H'             },
    /*  36  */  {   'j'             ,   'J'             },
    /*  37  */  {   'k'             ,   'K'             },
    /*  38  */  {   'l'             ,   'L'             },
    /*  39  */  {   ';'             ,   ':'             },
    /*  40  */  {   '\''            ,   '\"'            },
    /*  41  */  {   '`'             ,   '~'             },
    /*  42  */  {   KEY_LSHIFT      ,   KEY_LSHIFT      },
    /*  43  */  {   '\\'            ,   '|'             },
    /*  44  */  {   'z'             ,   'Z'             },
    /*  45  */  {   'x'             ,   'X'             },
    /*  46  */  {   'c'             ,   'C'             },
    /*  47  */  {   'v'             ,   'V'             },
    /*  48  */  {   'b'             ,   'B'             },
    /*  49  */  {   'n'             ,   'N'             },
    /*  50  */  {   'm'             ,   'M'             },
    /*  51  */  {   ','             ,   '<'             },
    /*  52  */  {   '.'             ,   '>'             },
    /*  53  */  {   '/'             ,   '?'             },
    /*  54  */  {   KEY_RSHIFT      ,   KEY_RSHIFT      },
    /*  55  */  {   '*'             ,   '*'             },
    /*  56  */  {   KEY_LALT        ,   KEY_LALT        },
    /*  57  */  {   ' '             ,   ' '             },
    /*  58  */  {   KEY_CAPSLOCK    ,   KEY_CAPSLOCK    },
    /*  59  */  {   KEY_F1          ,   KEY_F1          },
    /*  60  */  {   KEY_F2          ,   KEY_F2          },
    /*  61  */  {   KEY_F3          ,   KEY_F3          },
    /*  62  */  {   KEY_F4          ,   KEY_F4          },
    /*  63  */  {   KEY_F5          ,   KEY_F5          },
    /*  64  */  {   KEY_F6          ,   KEY_F6          },
    /*  65  */  {   KEY_F7          ,   KEY_F7          },
    /*  66  */  {   KEY_F8          ,   KEY_F8          },
    /*  67  */  {   KEY_F9          ,   KEY_F9          },
    /*  68  */  {   KEY_F10         ,   KEY_F10         },
    /*  69  */  {   KEY_NUMLOCK     ,   KEY_NUMLOCK     },
    /*  70  */  {   KEY_SCROLLLOCK  ,   KEY_SCROLLLOCK  },

    /*  71  */  {   KEY_HOME        ,   '7'             },
    /*  72  */  {   KEY_UP          ,   '8'             },
    /*  73  */  {   KEY_PAGEUP      ,   '9'             },
    /*  74  */  {   '-'             ,   '-'             },
    /*  75  */  {   KEY_LEFT        ,   '4'             },
    /*  76  */  {   KEY_CENTER      ,   '5'             },
    /*  77  */  {   KEY_RIGHT       ,   '6'             },
    /*  78  */  {   '+'             ,   '+'             },
    /*  79  */  {   KEY_END         ,   '1'             },
    /*  80  */  {   KEY_DOWN        ,   '2'             },
    /*  81  */  {   KEY_PAGEDOWN    ,   '3'             },
    /*  82  */  {   KEY_INS         ,   '0'             },
    /*  83  */  {   KEY_DEL         ,   '.'             },
    /*  84  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  85  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  86  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  87  */  {   KEY_F11         ,   KEY_F11         },
    /*  88  */  {   KEY_F12         ,   KEY_F12         }
};

// check if scan code is alphabet
BOOL kIsAlphabetScanCode(BYTE bScanCode) {
    // check by converting scan code to ASCII code

	if ( ('a' <= gs_vstKeyMappingTable[bScanCode].bNormalCode ) &&
		 (gs_vstKeyMappingTable[bScanCode].bNormalCode <= 'z') ) {
		return TRUE;
	}
	return FALSE;
}

// check if scan code is number of symbol in keyboard
// this function excludes number pad
BOOL kIsNumberOrSymbolScanCode(BYTE bScanCode) {
    if (2 <= bScanCode && bScanCode <= 53 &&
    	 kIsAlphabetScanCode(bScanCode) == FALSE) {
    	return TRUE;
    }
    return FALSE;
}

BOOL kIsNumberPadScanCode(BYTE bScanCode) {
	if (71 <= bScanCode && bScanCode <= 83) {
		return TRUE;
	}
	return FALSE;
}

// check if combined key value should be used
// ex) Shift + a = uppercase a = A
BOOL kShouldUseCombinedCode(BYTE bScanCode) {
	BYTE bDownScanCode;
	BOOL bUseCombinedKey = FALSE;

	// 0x7F = 0b01111111
	// when you release key, keyboard send pressed scan code with
	// 0x100000000
	bDownScanCode = bScanCode & 0x7F;

	// alphabet is affected by shift key and Caps Lock
	if (kIsAlphabetScanCode(bDownScanCode)) {
        // if either one of Shift key or Caps Lock is pressed, then
        // return true
		if (gs_stKeyboardManager.bShiftDown ^
		    gs_stKeyboardManager.bCapsLockOn) {
			return TRUE;
		}
		return FALSE;
	}

	// number and symbols are affected by shift key
	if (kIsNumberOrSymbolScanCode(bDownScanCode)) {
		if (gs_stKeyboardManager.bShiftDown) {
			return TRUE;
		}
		return FALSE;
	}

	// In case that Num Lock is activated, number pad except
	// extended number pad scan code is affected by shift key
	if (kIsNumberPadScanCode(bDownScanCode) &&
		gs_stKeyboardManager.bExtendedCodeIn == FALSE) {
        if (gs_stKeyboardManager.bNumLockOn) {
        	return TRUE;
        }
        else {
            return FALSE;
        }
	}
	// should not hit here. Just in case I made a mistake in code
	return FALSE;
}

//
void updateCombinationKeyStatusAndLED(BYTE bScanCode) {
    BOOL bDown;
    BYTE bDownScanCode;
    BOOL bLEDStatusChanged = FALSE;

    // check if key is pressed or released by checking the last bit of
    // scan code (0x10000000)
    if (bScanCode & 0x80) {
        bDown = FALSE;
        bDownScanCode = bScanCode & 0x7F;
    }
    else {
    	bDown = TRUE;
    	bDownScanCode = bScanCode;
    }

    // check if the last scan code is shift key or Caps Lock
    // or Num Lock or Scroll Lock
    // Shift Scan code
    if (bDownScanCode == 42 || bDownScanCode == 54) {
    	gs_stKeyboardManager.bShiftDown = bDown;
    }
    // Caps Lock Scan code
    else if (bDownScanCode == 58 && bDown == TRUE) {
        gs_stKeyboardManager.bCapsLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    // Num Lock Scan code
    else if (bDownScanCode == 69 && bDown == TRUE) {
        gs_stKeyboardManager.bNumLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    // Scroll Lock Scan code
    else if (bDownScanCode == 70 && bDown == TRUE) {
    	gs_stKeyboardManager.bScrollLockOn ^= TRUE;
    	bLEDStatusChanged = TRUE;
    }

    // send command to keyboard if LED status is changed
    if (bLEDStatusChanged) {
    	kChangeKeyboardLED(gs_stKeyboardManager.bCapsLockOn,
    			            gs_stKeyboardManager.bNumLockOn,
                          gs_stKeyboardManager.bScrollLockOn);
    }
}

// convert scan code to ASCII code
BOOL kConvertScanCodeToASCIICode(BYTE bScanCode, BYTE *pbASCIICode,
		                         BOOL *pbFlags) {
    BOOL bUseCombinedKey;

    // if pause key was pressed, ignore scan code as many as
    // number of pressed paused key
    if (gs_stKeyboardManager.iSkipCountForPause > 0) {
        gs_stKeyboardManager.iSkipCountForPause--;
        return FALSE;
    }

    // process Pause Scan code
    // pause is extension key and every extension key
    // is consisted of two bytes which starts with 0xE0 or 0xE1
    // only pause key is started with 0xE1
    if (bScanCode == 0xE1) {
        *pbASCIICode = KEY_PAUSE;
        *pbFlags = KEY_FLAGS_DOWN;
        gs_stKeyboardManager.iSkipCountForPause = KEY_SKIPCOUNTFORPAUSE;
        return TRUE;
    }
    // if scan code is 0xE0, then set only flags, you can find
    // extension key by calling this function one more
    else if (bScanCode == 0xE0) {
    	gs_stKeyboardManager.bExtendedCodeIn = TRUE;
    	return FALSE;
    }

    // check if it should return combined key
    bUseCombinedKey = kShouldUseCombinedCode(bScanCode);

    if (bUseCombinedKey) {
    	*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bCombinedCode;
    }
    else {
    	*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bNormalCode;
    }

    // check if extension key
    if (gs_stKeyboardManager.bExtendedCodeIn) {
    	*pbFlags = KEY_FLAGS_EXTENDEDKEY;
    	gs_stKeyboardManager.bExtendedCodeIn = FALSE;
    }
    else {
    	*pbFlags = 0;
    }

    // remember that "==", equal operator precedes "&" bitwise operator
    // if you do not enclose "bScanCode & 0x80" with parentheses,
    // the pbFlags does not include KEY_FLAGS_DOWN
    if ( (bScanCode & 0x80) == 0) {
    	*pbFlags |= KEY_FLAGS_DOWN;
    }

    // update combined key press or release state
    updateCombinationKeyStatusAndLED(bScanCode);

    return TRUE;
}



























