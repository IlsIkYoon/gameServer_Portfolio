#pragma once
#include "pch.h"
#include "Buffer/SerializeBuf.h"
#include "Buffer/LFreeQ.h"
#include "Session/Session.h"
#include "WorkProtocol.h"
#include "NetworkManager/Network.h"


struct ThreadJob
{
	BYTE jobType;
	ULONG64 id;
};
struct WorkSession
{
	ULONG64 ID;
	CSession* session;

	bool operator==(const WorkSession& rhs) const noexcept
	{
		return ID == rhs.ID && session == rhs.session;
	}
};


class CWork
{
public:
	CLFree_Queue<ThreadJob> ThreadMessageQ;
	std::list<WorkSession> WorkSessionList;
	CSessionManager* sessionManager;
	CNetwork* networkManager;

public:
	virtual bool WorkInit() = 0;
	virtual bool HandleMessage(CPacket* message, ULONG64 id) = 0; //Session ������� �̾Ƴ�
	virtual bool FrameLogic() = 0; //Player ������� ���ư� ����
	virtual void OnCreateSession(ULONG64 ID) = 0; 
	virtual void OnDeleteSession(ULONG64 ID) = 0; 

	bool CreateSession(ULONG64 ID);
	bool DeleteSession(ULONG64 ID);

	bool HandleCreateSessionMsg(ULONG64 ID);
	bool HandleDeleteSessionMsg(ULONG64 ID);
};

struct WorkArg
{
	CWork* threadWork;
};

class WorkManager
{
	bool RequestMoveToWork(BYTE toWork, ULONG64 ID);
};

unsigned int ThreadWorkFunc(void* param);
bool HandleThreadJob(CWork* myWork, ThreadJob* currentJob);