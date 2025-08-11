#pragma once
#include "pch.h"

#define DefaultBufSize 4 * 1024 



class RingBuffer
{

public:
	char* _buf;
	int _rear;
	int _front;
	int _bufSize;

public:
	RingBuffer();
	RingBuffer(int size);
	~RingBuffer();

	bool Enqueue(char* src, unsigned int dataSize, unsigned int* enqueResult);
	bool Dequeue(char* dest, unsigned int dataSize, unsigned int* dequeResult);
	bool Peek(char* dest, unsigned int dataSize, unsigned int* dequeResult);

	bool IsEmpty();
	bool IsFull();

	unsigned int GetSizeTotal();
	unsigned int GetSizeFree();
	unsigned int GetSizeUsed();
	unsigned int GetSizeUsed(int* outLocalRear, int* outLocalFront);

	unsigned int GetDirectEnqueSize();
	unsigned int GetDirectDequeSize();
	unsigned int GetDirectDequeSize(int* localRear, int* localFront);

	void MoveRear(int val);
	void MoveFront(int val);
	char* GetRear();
	char* GetFront();
	char* GetBufferStart();

	void BufferClear();
};


