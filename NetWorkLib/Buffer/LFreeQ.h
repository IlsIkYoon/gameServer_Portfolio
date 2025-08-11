#pragma once
#include "pch.h"
#include "LFMemoryPool.h"
#include "TLSPool.h"
#include "SerializeBuf.h"

#define MASKING_VALUE17BIT 0x00007fffffffffff

#define ENQUEUE 0x00
#define DEQUEUE 0x01
#define BIT_64 64
#define UNUSED_BIT 17
#define BIT_OR_VALUE (BIT_64 - UNUSED_BIT)


template <typename T>
class CLFree_Queue
{
public:
	struct Node
	{
		T _data;
		Node* _next;
	};

	Node* _head;
	alignas(64) Node* _tail;
	long _size;
	unsigned long long _bitCount;

public:
	CTLSPool<Node> mPool;

	CLFree_Queue()
	{
		_head = (Node*)mPool.Alloc();
		_head->_next = (Node*)this;
		_head->_data = T{};
		_tail = _head;
		_size = 0;
		_bitCount = 0;
	}

	inline unsigned long long GetSize()
	{
		return _size;
	}
	inline unsigned long long GetBitCount()
	{
		return _bitCount;
	}



	inline void* PackPtr(void* ptr)
	{

		unsigned long long localBitCount = InterlockedIncrement(&_bitCount);
		void* retPtr;

		retPtr = (void*)((ULONG_PTR)ptr | (localBitCount << BIT_OR_VALUE));

		return retPtr;
	}

	static void* UnpackPtr(void* ptr)
	{
		void* retPtr = (void*)((ULONG_PTR)ptr & MASKING_VALUE17BIT);

		return retPtr;
	}
	inline Node* PeekFront()
	{
		return ((Node*)(UnpackPtr(_head)))->_next;
	}


	inline void Enqueue(T pData)
	{
		Node* localTail;
		Node* exchangeNode;
		Node* localTailAddress;
		Node* nextNode;
		Node* newNode;

		newNode = (Node*)_mPool.Alloc();


		newNode->_data = pData;
		newNode->_next = (Node*)this;
		exchangeNode = (Node*)PackPtr(newNode); //상위 17비트는 0일 거라는 가정 하에 진행

		while (1)
		{
			localTail = _tail;
			localTailAddress = (Node*)UnpackPtr(localTail);
			nextNode = localTailAddress->_next;
			
			Node* packTailNext = (Node*)PackPtr(nextNode);

			if (nextNode == nullptr)
			{
				__debugbreak();
				continue;
			}

			if (nextNode != (Node*)this)
			{
				if (localTailAddress == nextNode)
				{
					__debugbreak();
				}
				if (InterlockedCompareExchangePointer((PVOID*)&_tail, packTailNext, localTail) == localTail)
				{

				}

				continue;

			}

			if (localTailAddress == newNode)
			{
				__debugbreak();
			}

			if (InterlockedCompareExchangePointer((PVOID*)&localTailAddress->_next, newNode, (Node*)this) == (Node*)this)
			{
				InterlockedIncrement(&_size);
				if (InterlockedCompareExchangePointer((PVOID*)&_tail, exchangeNode, localTail) == localTail)
				{

				}
				break;
			}
		}
	}

	T Dequeue()
	{
		Node* localHead;
		Node* localTail;
		Node* localHeadAddress;
		Node* localTailAddress;
		Node* nextNode;
		T retval;

		long localSize = InterlockedDecrement(&_size);

		if (localSize < 0)
		{
			InterlockedIncrement(&_size);
			return static_cast<T>(0);
		}

		while (1)
		{
			localHead = _head;
			localTail = _tail;

			localHeadAddress = (Node*)UnpackPtr(localHead);
			localTailAddress = (Node*)UnpackPtr(localTail);
			nextNode = localHeadAddress->_next;
			Node* tailNext = localTailAddress->_next;
			Node* packTailNext = (Node*)PackPtr(tailNext);
			Node* packHeadNext = (Node*)PackPtr(nextNode);
			
			if (localTailAddress->_next == nullptr)
			{
				continue;
			}

			if (tailNext != (Node*)this)
			{
				if (tailNext == localTailAddress)
				{
					__debugbreak();
				}
				if (InterlockedCompareExchangePointer((PVOID*)&_tail, packTailNext, localTail) == localTail)
				{

				}
				continue;
			}


			if (nextNode == (Node*)this || nextNode == nullptr)
				continue;


			retval = nextNode->_data;

			if (InterlockedCompareExchangePointer((PVOID*)&_head, packHeadNext, localHead) == localHead)
			{
				if (InterlockedCompareExchangePointer((PVOID*)&localHeadAddress->_next, (Node*)this, (Node*)this) == (Node*)this)
				{
					__debugbreak();
				}
				
				mPool.Delete(localHeadAddress);
				break;
			}
		}

		return retval;
	}


	void Clear()
	{
		while (1)
		{
			T retNode;
			retNode = Dequeue();
			if (retNode == nullptr)
			{
				break;
			}

			retNode->DecrementUseCount();
		}

		return;
	}

};