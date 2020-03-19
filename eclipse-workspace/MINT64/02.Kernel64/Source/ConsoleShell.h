#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "Types.h"

// macro
#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT 	300
#define CONSOLESHELL_PROMPTMESSAGE 			"MINT64>"

typedef void (* CommandFunction) (const char *pcParameter);

#pragma pack(push, 1)

// structure for shell command
typedef struct kShellCommandEntryStruct {
	// command string
	char *pcCommand;
	// --help string
	char *pcHelp;
	// function pointer to command
	CommandFunction pfFunction;
} SHELLCOMMANDENTRY;

// structure for processing parameters
typedef struct kParameterListStruct {
	const char *pcBuffer;
	// parameter length
	int iLength;
	// current parameter position to process
	int iCurrentPosition;
} PARAMETERLIST;

#pragma pack(pop)

// functions
void kStartConsoleShell( void );
void kExecuteCommand( const char *pcCommandBuffer );
void kInitialzeParameter( PARAMETERLIST *pstList, const char *pcParameter );
int kGetNextParameter( PARAMETERLIST *pstList, char *pcParameter );


// commands
void kHelp( const char *pcParameterBuffer);
void kCls( const char *pcParameterBuffer );
void kShowTotalRAMSize( const char *pcarameterbuffer );
void kStringToDecimalHexTest( const char *pcParameterbuffer );
void kShutdown (const char *pcParameterBuffer );
#endif /* _CONSOLESHELL_H_ */
