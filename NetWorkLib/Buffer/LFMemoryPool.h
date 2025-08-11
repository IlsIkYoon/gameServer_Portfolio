#pragma once
#include "pch.h"



#define MASKING_VALUE17BIT 0x00007fffffffffff
#define RETURN_NEXTVALUE 0x0f0f0f0f0f0f0f0f


template <typename T>
class CLfree_MemoryPool
{


	struct Node
	{

		T _data;
		Node* _next;
	};

	Node* _top;
	LONG _msize;
	LONG _madeCount;
	unsigned long long _count;


public:

	CLfree_MemoryPool()
	{
		_top = (Node*)nullptr;
		_msize = 0;
		_madeCount = 0;
		_count = 0;
	}

	void Delete(void* pNode) //������ push����
	{

		Node* exchangeNode;
		Node* localTop;
		Node* newNode;

		unsigned long long localCount;

		newNode = (Node*)pNode;
		localCount = InterlockedIncrement(&_count);
		exchangeNode = (Node*)((ULONG_PTR)newNode | (localCount << (64 - 17))); //���� 17��Ʈ�� 0�� �Ŷ�� ���� �Ͽ� ����


		while (1)
		{
			localTop = _top;
			newNode->_next = localTop;

			if (InterlockedCompareExchangePointer((PVOID*)&_top, exchangeNode, localTop) == localTop)
			{
				InterlockedIncrement(&_msize);
				break;
			}

		}

	}

	void* Alloc() //������ pop ����
	{
		Node* localTop;
		Node* realTopAdd;
		Node* retNode;

		while (1)
		{


			localTop = _top;
			if (localTop == (Node*)nullptr)
			{
				//���� �Ҵ��ؼ� �ְ� return
				Node* newNode = new Node;
				_InterlockedIncrement(&_madeCount);
				return (void*)newNode;
			}

			realTopAdd = (Node*)((ULONG_PTR)localTop & MASKING_VALUE17BIT);


			Node* nextNode = realTopAdd->_next;


			if (InterlockedCompareExchangePointer((PVOID*)&_top, nextNode, localTop) == localTop)
			{
				retNode = (Node*)((ULONG_PTR)localTop & MASKING_VALUE17BIT);
				InterlockedDecrement(&_msize);

				return (void*)retNode;
				break;
			}

		}

	}

};