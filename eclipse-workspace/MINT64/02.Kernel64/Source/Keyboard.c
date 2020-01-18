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
    // reading from 0x64 returns state register and
    // writing to 0x64 means writing to control register
    kOutPortByte(0x64 0xAE);

    // keyboard activation command to keyboard
    sendCommandToKeyboard(0xF4);

}

// read byte from output buffer
// attention: if there is no data in output buffer
//            computer freeze
BYTE kGetKeyboardScanCode(void) {
    while (kOutputBufferFull());

    return kInPortByte(0x60);
}

// set keyboard LED on/off
BOOL kChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn,
		BOOL bScrollLockOn) {

    BOOL command;
    if (sendCommandToKeyboard(0xED) == FALSE) {
        return FALSE;
    }

    BOOL command = (bCapsLockOn << 2 | bNumLockOn << 1 | bScrollLockOn);
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
    kOutportByte(0x64, 0xD0);

    for (i = 0; i < 0xFFFF; i++) {
        if (kOutputBufferFull()) {
            break;
        }
    }

    // read output port data
    kOutputPortData = kInportByte(0x60);
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
    kOutportByte(0x64, 0xD1);
    // reset processor
    kOutPortByte(0x60, 0x00);

    while (1);
}

///////////////////////////////////////////////////////////////////////////////
// table that convert scan code to ASCII code and related functions
///////////////////////////////////////////////////////////////////////////////










