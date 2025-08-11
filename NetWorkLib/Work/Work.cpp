#include "pch.h"
#include "Work.h"
#include "NetworkManager/NetWorkManager.h"


HANDLE WorkThreadArr[128];

unsigned int ThreadWorkFunc(void* param)
{
	WorkArg* workParam;
	CWork* myWork;
	CPacket* currentSessionJob;
	ThreadJob currentThreadJob;
	CSession* currentSession;
	CNetwork* myNetwork;
	
	DWORD prevTime;
	DWORD currentTime;
	DWORD resultTime;

	workParam = (WorkArg*)param;

	myWork = workParam->threadWork;
	myNetwork = myWork->networkManager;
	myWork->WorkInit();

	prevTime = timeGetTime();

	while (1)
	{
		while(myWork->ThreadMessageQ.GetSize() != 0)
		{
			currentThreadJob = myWork->ThreadMessageQ.Dequeue();

			HandleThreadJob(myWork, &currentThreadJob);
		}


		for(std::list<WorkSession>::iterator it = myWork->WorkSessionList.begin(); it != myWork->WorkSessionList.end();)
		{
			std::list<WorkSession>::iterator nextIt;

			nextIt = std::next(it);

			ULONG64 prevID = (*it).ID; 
			currentSession = (*it).session;

			myNetwork->IncrementSessionIoCount(currentSession);

			if ((currentSession)->_releaseIOFlag._struct.releaseFlag == 0x01)
			{
				myNetwork->DecrementSessionIoCount(currentSession);
				it = nextIt;
				continue;
			}
			if (currentSession->_ID._ulong64 != prevID)
			{
				myNetwork->DecrementSessionIoCount(currentSession);
				it = nextIt;
				continue;
			}
		
			while (1)
			{

				if (currentSession->jobQ.GetSize() == 0)
				{
					break;
				}

				currentSessionJob = currentSession->jobQ.Dequeue();

				if (myWork->HandleMessage(currentSessionJob, currentSession->_ID._ulong64) == false)
				{
					break;
				}

				currentSessionJob->DecrementUseCount();
			}

			myNetwork->DecrementSessionIoCount(currentSession);

			it = nextIt;
		}
		
		myWork->FrameLogic();

		currentTime = timeGetTime();
		resultTime = currentTime - prevTime;
		if (resultTime < 40)
		{
			Sleep(40 - resultTime);
		}

		prevTime += 40;
	}

	return 0;
}



bool WorkManager::RequestMoveToWork(BYTE toWork, ULONG64 ID)
{

	return true;
}


bool CWork::CreateSession(ULONG64 ID)
{
	ThreadJob message;
	CSession* currentSession;
	unsigned short sessionIndex;

	sessionIndex = CWanManager::GetIndex(ID);
	currentSession = &(*sessionManager)[sessionIndex];

	networkManager->IncrementSessionIoCount(currentSession); // 이 시점에 이미 들어간 것임 

	if (*(currentSession->_releaseIOFlag.GetReleaseFlagPtr()) == 0x01)
	{
		networkManager->DecrementSessionIoCount(currentSession);
		return false;
	}


	if (currentSession->_ID._ulong64 != ID)
	{
		networkManager->DecrementSessionIoCount(currentSession);
		return false;
	}
	currentSession->currentWork = this;

	message.id = ID;
	message.jobType = static_cast<BYTE>(enNetworkThreadProtocol::en_CreateSession);

	ThreadMessageQ.Enqueue(message);

	return true;
}
bool CWork::DeleteSession(ULONG64 ID)
{
	ThreadJob message;
	message.id = ID;
	message.jobType = static_cast<BYTE>(enNetworkThreadProtocol::en_DeleteSession);
	ThreadMessageQ.Enqueue(message);

	return true;
}

bool CWork::HandleCreateSessionMsg(ULONG64 ID)
{
	CSession* currentSession;
	unsigned short sessionIndex;
	
	sessionIndex = CWanManager::GetIndex(ID);
	currentSession = &(*sessionManager)[sessionIndex];
	WorkSession currentWorkSession;

	if (ID != currentSession->_ID._ulong64)
	{
		__debugbreak();
	}

	currentWorkSession.ID = ID;
	currentWorkSession.session = currentSession;

	for (std::list<WorkSession>::iterator it = WorkSessionList.begin(); it != WorkSessionList.end();it++)
	{
		if ((*it).session == currentSession)
		{
			__debugbreak();
		}
	}

	WorkSessionList.push_back(currentWorkSession);
	
	OnCreateSession(ID);

	networkManager->DecrementSessionIoCount(currentSession);

	return true;
}

bool CWork::HandleDeleteSessionMsg(ULONG64 ID)
{
	CSession* currentSession;
	unsigned short sessionIndex;

	sessionIndex = CWanManager::GetIndex(ID);
	currentSession = &(*sessionManager)[sessionIndex];

	WorkSession currentWorkSession;
	currentWorkSession.ID = ID;
	currentWorkSession.session = currentSession;
	WorkSessionList.remove(currentWorkSession);

	OnDeleteSession(ID);

	return true;
}

bool HandleThreadJob(CWork* myWork, ThreadJob* currentJob)
{
	ULONG64 jobID;
	BYTE jobType;
	jobID = currentJob->id;
	jobType = currentJob->jobType;

	switch (jobType)
	{
	case static_cast<BYTE>(enNetworkThreadProtocol::en_CreateSession) : 
		myWork->HandleCreateSessionMsg(jobID);
		break;
	case static_cast<BYTE>(enNetworkThreadProtocol::en_DeleteSession):
		myWork->HandleDeleteSessionMsg(jobID);
		break;
	default:
		__debugbreak();
		return false;
	}

	return true;;
}