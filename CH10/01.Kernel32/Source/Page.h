#ifndef __PAGE_H__
#define __PAGE_H__

#include "Types.h"

#define PAGE_FLAGS_P     0x00000001   // Present
#define PAGE_FLAGS_RW    0x00000002   // Read/Write
#define PAGE_FLAGS_US    0x00000004   // User/Supervisor
#define PAGE_FLAGS_PWT   0x00000008   // Page Level Write-through
#define PAGE_FLAGS_PCD   0x00000010   // PAGE Level Cache Disable
#define PAGE_FLAGS_A     0x00000020   // Accessed
#define PAGE_FLAGS_D     0x00000040   // Dirty
#define PAGE_FLAGS_PS    0x00000080   // Page Size
#define PAGE_FLAGS_G     0x00000100   // Global
#define PAGE_FLAGS_PAT   0x00001000   // Page Attribute Table Index

// last bit of upper part of page entry
#define PAGE_FLAGS_EXB   0x80000000   // Execute Disable bit

#define PAGE_FLAGS_DEFAULT (PAGE_FLAGS_P | PAGE_FLAGS_RW)
#define PAGE_TABLE_SIZE  0x1000  // each table size
#define PAGE_MAXENTRYCOUNT 512

#define PAGE_DEFAULTSIZE 0x200000

#pragma pack(push, 1)

// Base Page Table Entry structure for PML4 entry, page directory pointer
// entry, page directory entry, and page table entry
typedef struct kPageTableEntryStruct {
	// In case of PML4T and PDPTE,
	// 1 bit P, RW, US, PWT, PCD, A, D, PS, G, 3 bit Avail, 1 bit PAT,
	// 8 bit Reserved, 20 bit Base Address

	// In case OF PDE,
	// 1 bit P, RW, US, PWT, PCD, A, D, PS, G, 3 bit Avail, 1 bit PAT,
	// 8 bit Avail, 11 bit Base Address

	DWORD dwAttributeAndLowerBaseAddress;
	DWORD dwUpperBaseAddressAndEXB;
} PML4ENTRY, PDPTENTRY, PDENTRY, PTENTRY;

#pragma pack(pop)


void kInitializePageTables(void);
void kSetPageEntryData(PTENTRY *pstEntry, DWORD dwUpperBaseAddress,
		DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags);

#endif  /*__PAGE_H__*/
