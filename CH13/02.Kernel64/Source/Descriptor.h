#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include "Types.h"

/*
 * macros that defines each flag in global descriptor
 */

#define GDT_TYPE_CODE					0x0A
#define GDT_TYPE_DATA					0x02
#define GDT_TYPE_TSS						0x09

// s field in entry mean system.
#define GDT_FLAGS_LOWER_S				0x10
#define GDT_FLAGS_LOWER_DPL0			0x00
#define GDT_FLAGS_LOWER_DPL1			0x20
#define GDT_FLAGS_LOWER_DPL2			0x40
#define GDT_FLAGS_LOWER_DPL3			0x60
#define GDT_FLAGS_LOWER_P				0x80

#define GDT_FLAGS_UPPER_L				0x20
// this is D field which decides operation size
#define GDT_FLAGS_UPPER_DB				0x40
#define GDT_FLAGS_UPPER_G				0x80

// macros that we use in Descriptor.c

// lower part of GDT entry
// lower flags contains Code/Data/TSS, DPL0, present for system
// segment except that userland segment set DPL3
//
#define GDT_FLAGS_LOWER_KERNELCODE (GDT_TYPE_CODE | GDT_FLAGS_LOWER_S | \
		GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P)

#define GDT_FLAGS_LOWER_KERNELDATA (GDT_TYPE_DATA | GDT_FLAGS_LOWER_S | \
		GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P)

#define GDT_FLAGS_LOWER_TSS (GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P)

#define GDT_FLAGS_LOWER_USERCODE (GDT_TYPE_CODE | GDT_FLAGS_LOWER_S | \
		GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P)

#define GDT_FLAGS_LOWER_USERDATA (GDT_TYPE_DATA | GDT_FLAGS_LOWER_S | \
		GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P)

// upper part of GDT entry
// higher part set granularity and 64 bit
#define GDT_FLAGS_UPPER_CODE (GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L)
#define GDT_FLAGS_UPPER_DATA (GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L)
#define GDT_FLAGS_UPPER_TSS (GDT_FLAGS_UPPER_G)

// set descriptor offset in GDT
#define GDT_KERNELCODESEGMENT			0x08
#define GDT_KERNELDATASEGMENT			0x10
#define GDT_TSSSEGMENT					0x18

// GDTR start address is 1MB + 264KB page table
#define GDTR_STARTADDRESS				0x142000
// number of 8 byte entry; null descriptor/ kernel code/ kernel data
#define GDT_MAXENTRY8COUNT				3
// number of 16 byte entry; TSS entry
#define GDT_MAXENTRY16COUNT				1
// size of Global Descriptor table
#define GDT_TABLESIZE					( ( sizeof( GDTENTRY8 ) * GDT_MAXENTRY8COUNT ) \
		+ ( sizeof( GDTENTRY16 ) * GDT_MAXENTRY16COUNT ) )
#define TSS_SEGMENTSIZE					( sizeof( TSSSEGMENT ) )

/*
 * macros about Interrupt Descriptor Table
 */

// flags and types of Interrupt Descriptor
#define IDT_TYPE_INTERRUPT				0x0E
#define IDT_TYPE_TRAP					0x0F
#define IDT_FLAGS_DPL0					0x00
#define IDT_FLAGS_DPL1					0x20
#define IDT_FLAGS_DPL2					0x40
#define IDT_FLAGS_DPL3					0x60
#define IDT_FLAGS_P						0x80
#define IDT_FLAGS_IST0					0
#define IDT_FLAGS_IST1					1

// flags that I use for Mint64OS Interrupt Descriptor
#define IDT_FLAGS_KERNEL					( IDT_FLAGS_DPL0 | IDT_FLAGS_P )
#define IDT_FLAGS_USER					( IDT_FLAGS_DPL3 | IDT_FLAGS_P )

// maximum number of IDT entries are 100 for CPU
#define IDT_MAXENTRYCOUNT				100
// start address oF IDTR; arranged after TSS Segment
#define IDTR_STARTADDRESS				( GDTR_STARTADDRESS + sizeof( GDTR ) + \
		GDT_TABLESIZE + TSS_SEGMENTSIZE )
// start address of Interrupt Descriptor Table
#define IDT_STARTADDRESS					( IDTR_STARTADDRESS + sizeof( IDTR ) )
// size of Interrupt Descriptor Table
#define IDT_TABLESIZE					( IDT_MAXENTRYCOUNT * sizeof( IDTENTRY ) )

// IST start address
#define IST_STARTADDRESS					0x700000
// IST size
#define IST_SIZE							0x100000

#pragma pack( push, 1 )

// GDTR and IDTR structure
typedef struct kGDTRStruct {
	WORD wLimit;
	QWORD qwBaseAddress;
	// to align 16 byte address
	WORD wPadding;
	DWORD dwPadding;
} GDTR, IDTR;

// 8 byte GDT entry structure
typedef struct KGDTEntry8Struct {
    WORD wLowerLimit;
    WORD wLowerBaseAddress;
    BYTE bUpperBaseAddress1;
    // 4 bits Type, 1 bit S, 2 bites DPL, 1 bit P
    BYTE bTypeAndLowerFlag;
    // 4 bits Segment Limit, 1 bit AVL, L, D/B, G
    BYTE bUpperLimitAndUpperFlag;
    BYTE bUpperBaseAddress2;
} GDTENTRY8;

typedef struct kGDTEntry16Struct {
    WORD wLowerLimit;
    WORD wLowerBaseAddress;
    BYTE bMiddleBaseAddress1;
    // 4 bits Type, 1 bit 0, 2 bites DPL, 1 bit P
    BYTE bTypeAndLowerFlag;
    // 4 bits Segment Limit, 1 bit AVL, 0, 0, G
    BYTE bUpperLimitAndUpperFlag;
    BYTE bMiddleBaseAddress2;
    DWORD dwUpperBaseAddress;
    DWORD dwReserved;
} GDTENTRY16;

// TSS Data structure
typedef struct kTSSDataStruct {
    DWORD dwReserved1;
    QWORD qwRsp[3];
    QWORD qwReserved2;
    QWORD qwIST[7];
    QWORD qwReserved3;
    WORD wReserved;
    WORD wIOMapBaseAddress;
} TSSSEGMENT;

// IDT gate Descriptor structure
typedef struct kIDTEntryStruct {
	WORD wLowerBaseAddress;
	WORD wSegmentSelector;
	// 3 bits IST, 5 bits 0
	BYTE bIST;
	// 4 bits type, 1 bits 0, 2 bits DPL, 1 bit P
	BYTE bTypeAndFlags;
	WORD wMiddleBaseAddress;
	DWORD dwUpperBaseAddress;
	DWORD dwReserved;
} IDTENTRY;

#pragma pack ( pop )

// functions
void kInitializeGDTTableAndTSS(void);
void kSetGDTEntry8(GDTENTRY8 *pstEntry, DWORD dwBaseAddress, DWORD dwLimit,
		BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType);
void kSetGDTEntry16(GDTENTRY16 *pstEntry, QWORD qwBaseAddress, DWORD dwLimit,
		BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType);
void kInitializeTSSSegment(TSSSEGMENT *pstTSS);

void kInitializeIDTTables(void);
void kSetIDTEntry(IDTENTRY *pstEntry, void *pvHandler, WORD wSelector,
		BYTE bIST, BYTE bFlags, BYTE vType);
void kDummyHandler(void);

#endif








