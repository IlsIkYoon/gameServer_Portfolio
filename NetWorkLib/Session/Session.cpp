#include "pch.h"
#include "Session.h"


ULONG64 CSession::SessionIndexUnion::GetID()
{
	ULONG64 retval = 0;
	retval |= (ULONG64)_struct._id._dword;
	retval |= ((ULONG64)_struct._id._word) << 32;

	return retval;
}

void CSession::SessionIndexUnion::SetID(ULONG64 ID)
{
	ULONG64 byte4;
	ULONG64 byte2;

	byte4 = ID & 0x00000000ffffffff;
	byte2 = ID & 0x0000ffff00000000;
	byte2 = byte2 >> 32;

	_struct._id._dword = (DWORD)byte4;
	_struct._id._word = (WORD)byte2;
}

unsigned short CSession::SessionIndexUnion::GetIndex()
{
	return _struct._idex;
}

void CSession::SessionIndexUnion::SetIndex(unsigned short index)
{
	_struct._idex = index;
}
long* CSession::SessionReleaseIOCount::GetReleaseFlagPtr()
{
	return &_struct.releaseFlag;
}
long* CSession::SessionReleaseIOCount::GetIoCountPtr()
{
	return &_struct.ioCount;
}

CSessionManager::CSessionManager(int sessionCount)
{
	_sessionMaxCount = sessionCount;
	_sessionList = new CSession[sessionCount];
	_InitIndexStack();

	_sessionCount = 0;
	_sessionID = 1;
}

void CSessionManager::_InitIndexStack()
{
	for (int i = 19999; i >= 0; i--)
	{
		_indexStack.Delete((unsigned short)i);
	}
}

CSession::CSession()
{
	_socket = 0;
	_ID._ulong64 = 0;
	_releaseIOFlag._all = 0;
	_releaseIOFlag._struct.releaseFlag = 1;
	
	_sendFlag = 0;

	ZeroMemory(&_Addr, sizeof(_Addr));
	ZeroMemory(&_sendOverLapped, sizeof(_sendOverLapped));
	ZeroMemory(&_recvOverLapped, sizeof(_recvOverLapped));

	sendData = 0;
	sendCount = 0;
	_recvBuffer = nullptr;

	_status = static_cast<long>(CSession::Status::Active);
	_type = static_cast<BYTE>(enSessionType::en_IDLE);
	
	currentWork = nullptr;
}

CSession::~CSession()
{
}

CSession& CSessionManager::operator[](int idex)
{
	return _sessionList[idex];
}

void CSessionManager::Delete(unsigned short iDex)
{
	_sessionList[iDex].clear();
	_indexStack.Delete(iDex);
	InterlockedDecrement(&_sessionCount);
}

void CSession::clear()
{
	_socket = 0;
	_ID._ulong64 = 0;

	ZeroMemory(&_Addr, sizeof(_Addr));

	_sendBuffer.Clear();
	jobQ.Clear();
	_recvBuffer->DecrementUseCount();
	_recvBuffer = nullptr;

	sendData = 0;
	sendCount = 0;

	if (_sendBuffer._size != 0)
	{
		__debugbreak();
	}

	InterlockedExchange(&_status, static_cast<long>(CSession::Status::Active));
	_type = static_cast<BYTE>(enSessionType::en_IDLE);
	currentWork = nullptr;
}

void CSession::init()
{
	InterlockedIncrement(_releaseIOFlag.GetIoCountPtr());
	InterlockedExchange(&_releaseIOFlag._struct.releaseFlag, 0);
	InterlockedExchange(&_sendFlag, 0);

	_recvBuffer = CPacket::Alloc();
}

bool CSessionManager::_makeNewSession(unsigned short* outIDex, SOCKET* newSocket, SOCKADDR_IN* clientAddr)
{
	unsigned short iDex = _indexStack.Alloc();
	ULONG64 localID = _sessionID++;
	*outIDex = iDex;
	
	_sessionList[iDex].init();
	_sessionList[iDex]._ID.SetID(localID);
	_sessionList[iDex]._ID.SetIndex(iDex);
	_sessionList[iDex]._socket = *newSocket;
	_sessionList[iDex]._Addr = *clientAddr;
	_sessionList[iDex]._type = static_cast<BYTE>(enSessionType::en_Client); //기본이 클라이언트

	InterlockedIncrement(&_sessionCount);
	InterlockedIncrement(&g_LoginSessionCount);

	return true;
}

CSession& CSessionManager::GetSession(int idex)
{
	return _sessionList[idex];
}

void CSession::SwapRecvBuffer()
{
	CPacket* newBuf = CPacket::Alloc();
	CPacket* oldRecvbuf = _recvBuffer;
	newBuf->PutData(_recvBuffer->GetDataPtr(), _recvBuffer->GetDataSize());
	_recvBuffer = newBuf;
	if (oldRecvbuf->_usageCount != 1)
	{
		__debugbreak();
	}
	oldRecvbuf->DecrementUseCount();
}