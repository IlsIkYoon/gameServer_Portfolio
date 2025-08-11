#pragma once
#include "pch.h"
#include "Buffer/LFreeQ.h"
#include "Buffer/RingBuffer.h"
#include "Buffer/SerializeBuf.h"

class CWork;

extern unsigned long long g_LoginSessionCount;

enum class enSessionType
{
	en_IDLE = 0,
	en_Client,
	en_Server
};

class CSession
{
public:
	enum class Status
	{
		Active = 0, MarkForDeletion 
	};

	struct IDByte
	{
#pragma pack(push, 1)
		WORD _word;
		DWORD _dword;
#pragma pack(pop)
	};

	struct IdIndexStruct
	{
#pragma pack(push, 1)
		IDByte _id; //6����Ʈ ����ü
		unsigned short _idex;
#pragma pack(pop)
	};
	union SessionIndexUnion
	{
		IdIndexStruct _struct; //6����Ʈ 2����Ʈ ������ ����ü
		ULONG64 _ulong64;

		ULONG64 GetID();
		void SetID(ULONG64 ID);
		unsigned short GetIndex();
		void SetIndex(unsigned short index);
	};
	union SessionReleaseIOCount
	{
		struct FlagIostruct
		{
			long releaseFlag;
			long ioCount;
		};

		long* GetReleaseFlagPtr();
		long* GetIoCountPtr();

		long long _all;
		FlagIostruct _struct;
	};

	SOCKET _socket;

	SessionIndexUnion _ID;
	SessionReleaseIOCount _releaseIOFlag;

	SOCKADDR_IN _Addr;
	long _sendFlag;

	CLFree_Queue<CPacket*> _sendBuffer;
	CPacket* _recvBuffer;
	CWork* currentWork;

	OVERLAPPED _sendOverLapped;
	OVERLAPPED _recvOverLapped;

	BYTE _type;
	long _status;
	unsigned long sendData;
	unsigned long sendCount;

	CLFree_Queue<CPacket*> jobQ;

	CSession();
	~CSession();
	//----------------------------------
	//Session �Ҹ��ڿ� ���ϴ� �Լ�
	//----------------------------------
	void clear();
	//----------------------------------
	// Session �����ڿ� ���ϴ� �Լ�
	//----------------------------------
	void init();
	//----------------------------------
	// ���ú� ���۸� ���ο� ����ȭ ���۷� ���Ƴ��� �ִ� �Լ�
	//----------------------------------
	void SwapRecvBuffer();
};

class CSessionManager
{
	CSession* _sessionList;
	int _sessionMaxCount;
	LFreeStack<unsigned short> _indexStack;

	ULONG64 _sessionCount;
	ULONG64 _sessionID;
	//----------------------------------
	// IndexPool�� 1~20000������ ���ڸ� �־��ִ� �Լ�
	//----------------------------------
	void _InitIndexStack();

public:
	CSessionManager(int sessionCount);
	bool _makeNewSession(unsigned short* outIDex, SOCKET* newSocket, SOCKADDR_IN* clientAddr);

	CSession& operator[](int idex);
	CSession& GetSession(int idex);
	//----------------------------------
	// ���� delete�Լ�
	//----------------------------------
	void Delete(unsigned short iDex);
};

