#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"

// singleton console manager
CONSOLEMANAGER gs_stConsoleManager = { 0, };

// should be called once before using console
void kInitializeConsole( int iX, int iY ) {
	// set 0 to CONSOLEMANAGER
	kMenSet( &gs_stConsoleManager, 0, sizeof( gs_stConsoleManager ) );
	// set cursor location
	kSetCursor( iX, iY );
}

// set cursor location and character output location
void kSetCursor( int iX, int iY) {
	int iLinearValue;

	iLinearValue = iY * CONSOLE_WIDTH + iX;

	// select upper cursor location register so we send part of
	// linear value
	kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR );
	kOutportByte (VGA_PORT_DATA, iLinearValue >> 8 );

	// select lower cursor location register so we send part of
	// linear value
	kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR );
	kOutPortByte( VGA_PORT_DATA, iLinearValue & 0xFF );

	// update CONSOLE Manager cursor location
	gs_stConsoleManager.iCurrentPrintOffset = iLinearValue;
}

// get current cursor location
void kGetCursor( int *piX, int *piY ) {
	*piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
	*piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}

// Mint64OS version printf
void kPrintf( const char *pcFormatString, ... ) {
	va_list ap;
	char vcBuffer[ 1024 ];
	int iNextPrintOffset;

	// process with vsprintf() by using va_list
	va_start(ap, pcFormatString );
	kVSPrintf( vcBuffer, pcFormatString, ap );
	va_end(ap);

	// print formatString to screen
	iNextPrintOffset = kConsolePrintString( vcBuffer );
	// update console cursor location
	kSetCursor( iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH );
}

// prints out string with '\n' or '\t' to screen, and return next cursor
// location for printing
int kConsolePrintString( const char *pcBuffer ) {
	CHARACTER *pstScreen = ( CHARACTER *) CONSOLE_VIDEOMEMORYADDRESS;
	int i, j;
	int iLength;
	int iPrintOffset;

	iPrintOffset = gs_stConsoleManager.iCurrentPrintOffset;
	iLength = kStrLen( pcBuffer );
	for (i = 0; i < iLength; i++) {
		if ( pcBuffer[i] == '\n' ) {
			// move cursor location to multiply of 80
			// so iPrintOffset's iX is equal to 0
			iPrintOffset += ( CONSOLE_WIDTH - ( iPrintOffset % CONSOLE_WIDTH ) );
		}
		else if ( pcBuffer[i] == '\t') {
			// move cursor location to multiply of 8
			iPrintOffset += ( 8 - ( iPrintOffset % 8 ) );
		}
		// print generic character
		else {
			pstScreen[iPrintOffset].bCharacter = pcBuffer [i];
			pstScreen[iPrintOffset].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
			iPrintOffset++;
		}

		// when iPrintOffset is out of screen, scroll down a line
		if (iPrintOffset >= ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) ) {
			// except first line of screen, move all lines up by one line
			kMemCpy( CONSOLE_VIDEOMEMORYADDRESS,
					CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof( CHARACTER ),
					( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH * sizeof ( CHARACTER ) );

			// fill last line with blank
			for (j = ( CONSOLE_HEIGHT - 1) * (CONSOLE_WIDTH);
					j < ( CONSOLE_HEIGHT * CONSOLE_WIDTH ); j++) {
				pstScreen[j].bCharacter = ' ';
				pstScreen[j].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
			}

			// set cursor location to first index of last line
			iPrintOffset = (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH;
		}
		return iPrintOffset;
}

// stopped here 20200318
