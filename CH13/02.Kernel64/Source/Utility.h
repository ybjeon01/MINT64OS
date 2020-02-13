#ifndef __UTILITY_H__
#define __UILITY_H__

#include "Types.h"

// functions
void kMemSet(void *pvDestination, BYTE bData, int iSize);
int kMemCpy(void *pvDestination, const void *pvSource, int iSize);
int kMemCpm(const void *pvDestination, const void *pvSource, int iSize);

#endif /* __UTILITY_H__ */
