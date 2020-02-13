#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "Types.h"

#pragma pack(push ,1)

// general queue that can insert item of any data type
typedef struct kQqueueManagerStruct {
	// size of item
	// max number of item
	int iDataSize;
	int iMaxDataCount;

	// index for circular queue
	void *pvQueueArray;
	int iPutIndex;
	int iGetIndex;

	// check if last operation was put or get
	BOOL bLastOperationPut;
} QUEUE;

#pragma pack(pop)

// functions
void kInitializeQueue(QUEUE *pstQueue, void *pvQueueBuffer, int iMaxDataCount,
		int iDataSize);
BOOL kIsQueueFull(const QUEUE *pstQueue);
BOOL kIsQueueEmpty(const QUEUE *pstQeueu);
BOOL kPutQueue(QUEUE *pstQueue, const void *pvData);
BOOL kGetQueue(QUEUE *pstQueue, void *pvData);

#endif /* __QUEUE_H__ */
