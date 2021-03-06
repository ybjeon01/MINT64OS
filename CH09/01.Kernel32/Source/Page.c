#include "Page.h"

// creates PML4 table, PDP table, PD table, and their entries
void kInitializePageTables(void) {
    PML4ENTRY *pstPML4TEntry;
    PDPTENTRY *pstPDPTEntry;
    PDENTRY *pstPDEntry;
    DWORD dwMappingAddress;
    int i;

    // create PML4 table
    // set all PML4 entry with 0 except first entry
    // for IA-32e mode, we use memory from 2MB to 1MB for
    // page tables, stack, and etc.
    pstPML4TEntry = (PML4ENTRY *) 0x100000;

    // our PML4 table size is 4KB so first PDPTENTRY address
    // is (1MB + 4KB) which is 0x101000 in hex
    kSetPageEntryData(&(pstPML4TEntry[0]), 0x00, 0x101000,
    		PAGE_FLAGS_DEFAULT, 0);

    // Since we support up to 64GB, one PML4T entry is enough
    for (i = 1; i < PAGE_MAXENTRYCOUNT; i++) {
    	kSetPageEntryData(&(pstPML4TEntry[i]), 0, 0, 0, 0);
    }

    // first page directory pointer table address and also
    // first entry of PDP.
    // this one PDP table has 64 entries for 64GB support
    pstPDPTEntry = (PDPTENTRY *) 0x101000;

    for (i = 0; i < 64; i++) {
    	// (1MB + 4KB) + 4KB = 0x102000
        kSetPageEntryData(&(pstPDPTEntry[i]), 0,
        		0x102000 + (i * PAGE_TABLE_SIZE), PAGE_FLAGS_DEFAULT, 0);

    }

    // creates page directories and its entries
    for (i = 64; i < PAGE_MAXENTRYCOUNT; i++) {
        kSetPageEntryData(&(pstPDPTEntry[i]), 0, 0, 0, 0);
    }

    // first page directory table address and also
    // first page entry address
    pstPDEntry = (PDENTRY *) 0x102000;
    dwMappingAddress = 0;
    for (i = 0; i < 64 * PAGE_MAXENTRYCOUNT; i++) {
    	// since we cannot represent 64 bit address in IA-32 mode,
    	// "(i * PAGE_DEFAULTSIZE) >> 32" cannot be used for upper
    	// base address. so I first right-shifted 20 bits and multiplies
    	// by i and then right-shifted 12 bits
        kSetPageEntryData(&(pstPDEntry[i]),
        		(i * (PAGE_DEFAULTSIZE >> 20)) >> 12, dwMappingAddress,
				PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
         dwMappingAddress += PAGE_DEFAULTSIZE;
    }
}

// usable function to create entry in every page table
void kSetPageEntryData(PTENTRY *pstEntry, DWORD dwUpperBaseAddress,
		DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags) {
    pstEntry->dwAttributeAndLowerBaseAddress =
    		dwLowerBaseAddress | dwLowerFlags;
    pstEntry->dwUpperBaseAddressAndEXB =
        (dwUpperBaseAddress & 0xFF) | dwUpperFlags;
}
