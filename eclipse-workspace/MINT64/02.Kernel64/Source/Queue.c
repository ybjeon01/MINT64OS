#include "Queue.h"
#include "Utility.h"

// initialize Queue
void kInitializeQueue(QUEUE *pstQueue, void *pvQueueBuffer, int iMaxDataCount,
		int iDataSize) {
	pstQueue->iMaxDataCount = iMaxDataCount;
	pstQueue->iDataSize = iDataSize;
	pstQueue->pvQueueArray = pvQueueBuffer;

	pstQueue->iPutIndex = 0;
	pstQueue->iGetIndex = 0;
	pstQueue->bLastOperationPut = FALSE;
}

// check if queue is full
BOOL kIsQueueFull(const QUEUE *pstQueue) {
	if ( ( pstQueue->iGetIndex == pstQueue->iPutIndex ) &&
		  ( pstQueue->bLastOperationPut == TRUE) ) {
		return TRUE;
	}
    return FALSE;
}

// check if queue is empty
BOOL kIsQueueEmpty(const QUEUE *pstQueue) {
	if ( ( pstQueue->iGetIndex == pstQueue->iPutIndex ) &&
		  ( pstQueue->bLastOperationPut == FALSE) ) {
		return TRUE;
	}
    return FALSE;
}

// put a data to queue
BOOL kPutQueue(QUEUE *pstQueue, const void *pvData) {
    if ( kIsQueueFull( pstQueue ) ) {
        return FALSE;
    }
    // copy pvData to queue's iPutIdex pointer
    kMemCpy( (char *) pstQueue->pvQueueArray + ( pstQueue->iDataSize *
    		pstQueue->iPutIndex ), pvData, pstQueue->iDataSize );

    // update iPutIndex and lastOperation
    // this queue is circular queue, so index can not just increased
    pstQueue->iPutIndex = ( pstQueue->iPutIndex + 1) % pstQueue->iMaxDataCount;
    pstQueue->bLastOperationPut = TRUE;
    return TRUE;
}

// get data from queue
BOOL kGetQueue(QUEUE *pstQueue, void *pvData) {
	if ( kIsQueueEmpty(pstQueue) ) {
		return FALSE;
	}
	// copy data in Queue to pvData
	kMemCpy(pvData, (char *) pstQueue->pvQueueArray + ( pstQueue->iDataSize *
			pstQueue->iGetIndex ), pstQueue->iDataSize );

    // update iPutIndex and lastOperation
    // this queue is circular queue, so index can not just increased
	pstQueue->iGetIndex = ( pstQueue->iGetIndex + 1 )% pstQueue->iMaxDataCount;
	pstQueue->bLastOperationPut = FALSE;
	return TRUE;

}
