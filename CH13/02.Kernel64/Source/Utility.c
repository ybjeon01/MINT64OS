#include "Utility.h"

// fill destination with bData as many as iSize
void kMemSet(void *pvDestination, BYTE bData, int iSize) {
	int i;

	for (i = 0; i < iSize; i++) {
		( (char *) pvDestination )[i] = bData;
	}
}

// copy data in pvSource to pvDestination as many as iSize
int kMemCpy(void *pvDestination, const void *pvSource, int iSize) {
    int i;

    for (i=0; i < iSize; i++) {
    	( (char *) pvDestination)[i] = ( (char *) pvSource)[i];
    }
    return iSize;
}

// compare data in pvSource with data in pvDestination as many as iSize
int kmemCmp (const void *pvDestination, const void *pvSource, int iSize) {
	int i;
	char cTemp;

	for (i = 0; i < iSize; i++) {
		cTemp = ( (char *) pvDestination)[i] - ( (char *) pvSource)[i];
		if (cTemp != 0) {
			return (int) cTemp;
		}
	}
	return 0;
}
