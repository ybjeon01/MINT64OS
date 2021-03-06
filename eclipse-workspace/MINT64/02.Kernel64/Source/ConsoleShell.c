#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "Types.h"

// command table
SHELLCOMMANDENTRY gs_vstCommandTable[] = {
		{ "help", "Show Help", kHelp },
		{ "cls", "Clear Screen", kCls },
		{ "totalram", "Show Total RAM Size", kShowTotalRAMSize },
		{ "shutdown", "Shutdown And Reboot OS", kShutdown },
};

// shell main loop
void kStartConsoleShell( void ) {
	char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
	int iCommandBufferIndex = 0;
	BYTE bKey;
	int iCursorX, iCursorY;

	// print prompt
	kPrintf( CONSOLESHELL_PROMPTMESSAGE );

	while ( TRUE ) {
		bKey = kGetCh();
		// process backspace key
		if ( bKey == KEY_BACKSPACE ) {
			if ( iCommandBufferIndex > 0 ) {
				kGetCursor( &iCursorX, &iCursorY );
				kPrintStringXY( iCursorX - 1, iCursorY, ' ' );
				kSetCursor( iCursorX - 1, iCursorY );
			}
		}
		// process enter key
		else if ( bKey == KEY_ENTER ) {
			kPrintf( "\n" );
			if (iCommandBufferIndex > 0 ) {
				vcCommandBuffer[ iCommandBufferIndex ] = '\0';
				kExecuteCommand( vcCommandBuffer );
			}

			// print prompt message and flush buffer
			kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );
			kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
			iCommandBufferIndex = 0;
		}
		// ignore shift key, caps lock, and scroll lock
		else if ( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
					( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
					( bKey == KEY_SCROLLLOCK ) ) {
			;
		}
		else {
			// convert tab key to a space
			if ( bKey == KEY_TAB ) {
				bKey = ' ';
			}

			// fill buffer only if buffer has enough space
			if ( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ) {
				vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
				kPrintf( "%c",bKey );
			}
		}
	}
}


// compare string given by user to commands in buffer and if they matches,
// execute the command
void kExecuteCommand(const char *pcCommandBuffer) {
	int i, iSpaceIndex;
	int iCommandBufferLength, iCommandLength;
	int iCount;

	// measure length of command length
	iCommandBufferLength = kStrLen( pcCommandBuffer );
	for ( iSpaceIndex = 0; iSpaceIndex < iCommandBufferLength; iSpaceIndex++ ) {
		if ( pcCommandBuffer[ iSpaceIndex ] == ' ' ) {
			break;
		}
	}

	iCount = sizeof( gs_vstCommandTable) / sizeof( SHELLCOMMANDENTRY );
	for ( i = 0; i < iCount; i++ ) {
		iCommandLength = kStrLen( gs_vstCommandTable[i].pcCommand );
		// compare length of command to that of command buffer
		if ( ( iCommandLength == iSpaceIndex ) &&
			  ( kMemCmp( gs_vstCommandTable[i].pcCommand, pcCommandBuffer, iSpaceIndex ) == 0 ) ) {
			gs_vstCommandTable[i].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
			break;
		}
	}

	if ( i>= iCount ) {
		kPrintf(";%s; is not found.\n", pcCommandBuffer );
	}
}


// initialize parameter structure
void kInitializeParameter( PARAMETERLIST *pstList, const char *pcParameter ) {
	pstList->pcBuffer = pcParameter;
	pstList->iLength = kStrLen( pcParameter);
	pstList->iCurrentPosition = 0;
}


// return parameter content and length of it
int kGetNextParameter( PARAMETERLIST *pstList, char *pcParameter ) {
	int i;
	int iLength;

	if ( pstList->iLength <= pstList->iCurrentPosition ) {
		return 0;
	}

	for ( i = pstList->iCurrentPosition; i < pstList->iLength; i++ ) {
		if ( pstList->pcBuffer[i] == ' ' ) {
			break;
		}
	}

	kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
	iLength = i - pstList->iCurrentPosition;
	pcParameter[iLength] = '\0';

	// update parameter current position
	pstList->iCurrentPosition += iLength + 1;
	return iLength;
}

//========================================================================
// command code
//========================================================================

// print shell help
void kHelp(const char *pcCommandBuffer) {
	int i ;
	int iCount;
	int iCursorX, iCursorY;
	int iLength;
	int iMaxCommandLength = 0;

	kPrintf("=========================================================\n");
	kPrintf("                    MINT 64 Shell Help                   \n");
	kPrintf("=========================================================\n");

	iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

	// find longest length of commands
	for ( i = 0; i < iCount; i++ ) {
		iLength = kStrLen( gs_vstCommandTable[i].pcCommand );
		if ( iLength > iMaxCommandLength ) {
			iMaxCommandLength = iLength;
		}
	}

	// print help string
	for ( i = 0; i < iCount; i++ ) {
		kPrintf( "%s", gs_vstCommandTable[i].pcCommand );
		kGetCursor( &iCursorX, &iCursorY );
		kSetCursor( iMaxCommandLength, iCursorY );
		kPrintf("  -  %s\n", gs_vstCommandTable[i].pcHelp );
	}
}


// clear all screen
void kCls( const char *pcParameterBuffer ) {
	// since first line is used for debugging, cursor move to line 1
	kClearScreen();
	kSetCursor( 0, 1 );
}


// show total ram size
void kShowTotalRamSize( const char *pcParameterBufer ) {
	kPrintf( "Total Ram Size = %d MB\n", kGetTotalRamSize() );
}


// convert number encoded in ASCII to integer and print the integer
void kStringToDecimalHexTest( const char *pcParameterBuffer ) {
	char vcParameter[100];
	int iLength;
	PARAMETERLIST stList;
	int iCount = 0;
	long lValue;

	// initialize parameter
	kInitializeParameter( &stList, pcParameterBuffer );

	while ( TRUE ) {
		iLength = kGetNextParameter( &stList, vcParameter );
		// when no parameter exists
		if ( iLength == 0 ) {
			break;
		}

		// print parameter and decide the parameter is hex or decimal
		kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1,
				vcParameter, iLength );

		if ( kMemCmp(pcParameterBuffer, "0x", 2 ) == 0 ) {
			lValue = kAToI( vcParameter + 2, 16 );
			kPrintf( "HEX Value = %q\n", lValue );
		}
		else {
			lValue = kAToI( vcParameter, 10 );
			kPrintf( "DecimalValue = %d\n", lValue );
		}
		iCount++;
	}
}


// reboot PC
void kShutdown( const char *pcParameterBuffer ) {
	kPrintf( "System Shutdown Start...\n" );

	// reboot PC by using keyboard controller
	kPrintf( "Press Any Key To Reboot PC..." );
	kGetCh();
	kReboot();
}
