#pragma once

#include "pch.h"
#include "TLSPool.h"
#include "ntPacketDefine.h"

#define HEADER_SIZE 5

extern unsigned long long g_CPacketReleaseCount;
extern unsigned long long g_CPacketAllocCount;

class CPacket
{
	enum en_packet
	{
		eBuffer_default = 1400
	};

public:
	enum class ErrorCode
	{
		SUCCESS = 0, 
		INCOMPLETE_DATA_PACKET,
		INVALID_DATA_PACKET
	};

public:
	int _front;
	int _rear;
	int _bufsize;
	long _usageCount;
	bool _encodeflag;
	long _headerFlag;
	char* _buf;

	char* GetHeaderPtr();

public:
	inline static thread_local TMemoryPool<CPacket> cLocalPool;

	CPacket();
	CPacket(int bufize);
	~CPacket();

	static CPacket* Alloc()
	{
		CPacket* retNode = (CPacket*)cLocalPool.Alloc();
		retNode->Init();

		InterlockedIncrement(&g_CPacketAllocCount);

		return retNode;
	}

	static void ReleaseCPacket(CPacket* target)
	{
		target->Clear();
		cLocalPool.Delete(target);
		InterlockedDecrement(&g_CPacketAllocCount);
	}
	
	void IncrementUseCount();
	void DecrementUseCount();
	//------------------------------------------
	//�Ҹ��� ��� ȣ��Ǵ� �Լ� 
	//------------------------------------------
	void Clear();
	//------------------------------------------
	//������ ��� ȣ��Ǵ� �Լ� 
	//------------------------------------------
	void Init();
	//------------------------------------------
	//�����ִ� ������ ������ ��ȯ 
	//------------------------------------------
	int GetBufferSize();
	//------------------------------------------
	//����ִ� �������� ���� ��ȯ
	//------------------------------------------
	int GetDataSize();
	//------------------------------------------
	// �����͸� ���� ���� ��ȯ���� 
	//------------------------------------------
	char* GetBufferPtr();
	//------------------------------------------
	// �����Ͱ� ����ִ� �κ��� ��ȯ����
	//------------------------------------------
	char* GetDataPtr();
	int MoveFront(int iSize);
	int MoveRear(int iSize);

	bool PutData(char* data, int size);
	//-----------------------------------------
	// front�� 0���� �ű�� �ű⿡ ����� �־��ִ� �Լ�
	//-----------------------------------------
	bool PutHeader(ClientHeader cHeader);
	//-----------------------------------------
	// front�� 3���� �ű�� �ű⿡ ����� �־��ִ� �Լ�
	//-----------------------------------------
	bool PutHeader(ServerHeader sHeader);
	//-----------------------------------------
	// Front���� ������ �̾��ִ� �Լ�
	//-----------------------------------------
	bool PopFrontData(int size, char* out);
	//--------------------------------------------
	// ����ȭ ���ۿ� ����� �ְ� ��ȣȭ ���ִ� �Լ�.
	// Ŭ���̾�Ʈ ����̶� 5����Ʈ �԰� ���
	// �̹� Encode �Ǿ��ٸ� return false
	//--------------------------------------------
	bool _ClientEncodePacket();
	//--------------------------------------------
	// ��ȣȭ �� ����ȭ ���۸� ��ȣȭ ���ִ� �Լ�.
	// Ŭ���̾�Ʈ ����̶� 5����Ʈ �԰� ���
	// üũ�� üũ �� ���� �ٸ��� return false -> ���� ���� �������� ����
	//--------------------------------------------
	int _ClientDecodePacket();

	//������ �����ε�//---------------------------------------
	template<typename type>
		requires std::is_arithmetic_v<type>
	CPacket& operator<<(type iValue) {
		
		(type&)_buf[_rear] = iValue;
		_rear += sizeof(iValue);

		return *this;
	}

	template<typename type>
		requires std::is_arithmetic_v<type>
	CPacket& operator>>(type& iValue) {

		iValue = (type&)_buf[_front];
		_front += sizeof(iValue);

		return *this;
	}
};

