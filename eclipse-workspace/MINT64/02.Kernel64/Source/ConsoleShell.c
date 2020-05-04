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
	// stopped
}
