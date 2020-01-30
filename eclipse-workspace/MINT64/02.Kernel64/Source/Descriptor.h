#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include "Types.h"

/*
 * macros that defines each flag in global descriptor
 */

#define GDT_TYPE_CODE			0x0A
#define GDT_TYPE_DATA			0x02
#define GDT_TYPE_TSS				0x09

// s field in entry mean system.
#define GDT_FLAGS_LOWER_S		0x10
#define GDT_FLAGS_DPL0			0x00
#define GDT_FLAGS_DPL1			0x20
#define GDT_FLAGS_DPL2			0x40
#define GDT_FLAGS_DPL3			0x60
#define GDT_FLAGS_LOWER_P		0x80
#define GDT_FLAGS_UPPER_L		0x20
// this is D field which decides operation
// size
#define GDT_FLAGS_UPPER_DB		0x40
#define GDT_FLAGS_UPPER_G		0x80

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
#define GDT_FLAGS_UPPER_CODE (GDT_FLAGS_UPPER_G | GD_FLAGS_UPPER_L)
#define GDT_FLAGS_UPPER_DATA (GDT_FLAGS_UPPER_G | GD_FLAGS_UPPER_L)
#define GDT_FLAGS_UPPER_TSS (GDT_FLAGS_UPPER_G)

// set descriptor offset in GDT
#define GDT_KERNELCODESEGMENT	0x08
#define GDT_KERNELDATASEGMENT	0x10
#define GDT_TSSSEGMENT			0x18

// stopped here


