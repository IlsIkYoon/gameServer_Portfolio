#pragma once
#include "pch.h"

#include "Buffer/RingBuffer.h"
#include "Buffer/SerializeBuf.h"
#include "Log/Log.h"
#include "Buffer/LFreeQ.h"
#include "Log/Profiler.h"
#include "Session/Session.h"
#include "Network.h"

//-----------------------------------------------
// Ŭ���̾�Ʈ ��� ���� ��Ʈ��ũ �Ŵ���
//-----------------------------------------------
class CLanManager : public CNetwork
{
public:
	//--------------------------------------------
	//network Session ������ ���� ����
	//--------------------------------------------
	SOCKADDR_IN _serverAddr;

	unsigned short _portNum;

	CSessionManager* _sessionList;
	unsigned long _sendInProgress;

	int _sessionMaxCount;
	long _sessionLoginCount;
	//--------------------------------------------
	// IOCP ����
	//--------------------------------------------
	int _concurrentCount;
	int _workerThreadCount;
	HANDLE _hIOCP;

	//---------------------------------------------
	//������ ������ ���� ����
	//---------------------------------------------
	std::thread* _workerThreadArr;
	std::thread _acceptThread;

	HANDLE _exitThreadEvent;
	//---------------------------------------------
	// �α� ������ �ϴ� �Ŵ���
	// ���ο��� �α� ������ ���� �� ť�� �ִ� �α׸� ���Ͽ� ����ϴ� ����
	//---------------------------------------------
	static CLogManager _log;

	SOCKET _listenSocket;

public:
	CLanManager();
	~CLanManager();

private:
	bool _NetworkInit();
	void _MakeNetWorkMainThread();
	//--------------------------------------------
	// ��Ŀ ������ ���� �� 
	// ���ŷ _listensocket�� �̿��� accept���� �����ϴ� �Լ�
	// accept�� �����ϸ�  _sessionCount++;
	// accept�� �����ص� Ű ���� �̹� ���ο� �־��ٸ� ���� ���з� ����
	// ���Ŀ� OnAccept �ڵ鷯 �Լ� ȣ��
	//--------------------------------------------
	void AcceptThread();

	void IOCP_WorkerThread();
	//--------------------------------------------
	// WsaRecv�Լ� ����. ���� �ڵ忡 ���� ����ó��, �л� ���� ���� �ൿ ����
	//--------------------------------------------
	bool _RecvPost(CSession* _session);
	//--------------------------------------------
	// WsaSend�Լ� ����. ���� �ڵ忡 ���� ����ó��, �л� ���� ���� �ൿ ����
	// ����ִ� ������ ���� Send
	//--------------------------------------------
	bool _SendPost(CSession* _session);

	bool _TrySendPost(CSession* _session);
	//--------------------------------------------
	// ���Ǹ���Ʈ�� sendInProgressFlag�� Ȯ���ϰ� Flag�� ���� ������ ���� �迭�� ���鼭 SendPost�� ȣ�����ִ� �Լ�
	//--------------------------------------------
	bool SendToAllSessions();
	//--------------------------------------------
	// Session�� recvBuf(rBuf)���� ��Ʈ��ũ ��� ���̿� �´� ���̷ε带 �̾Ƽ�
	// sBuf�� �����ϰ� ����.
	//--------------------------------------------
	bool _DequePacket(CPacket* sBuf, CSession* _session);
	//--------------------------------------------
	// _session�� recv Buffer�� �ִ� �����͸� ���� �� �� _packet�� _packetSize��ŭ
	// �����ϰ� �� �����͸� �ٽ� ���ۿ� �־���
	//--------------------------------------------
	void _RecvBufRestorePacket(CSession* _session, char* _packet, int _packetSize);
	//--------------------------------------------
	// ������ ������ �����ϴ� �Լ�
	// close socket, ���� delete, list���� erase�۾� ����
	//--------------------------------------------
	void _DisconnectSession(ULONG64 sessionID);
	void _DisconnectSession(CSession* _session);

	//--------------------------------------------
	// ������� ���� ó���� ���ο��� ��
	// ���� �߻��� true ��ȯ
	//--------------------------------------------
	bool CheckGQCSError(bool retval, DWORD* recvdbytes, ULONG_PTR recvdkey, OVERLAPPED* overlapped, DWORD errorno);

	void InitSessionList(int SessionCount);
	//--------------------------------------------
	// CancelIo�� ȣ�� ���Ѽ� ���� ��ƾ�� Ÿ�� ����� �Լ�
	//--------------------------------------------
	bool RequestSessionAbort(ULONG64 playerID);

	virtual bool IncrementSessionIoCount(CSession* _session) override final;
	//--------------------------------------------
	// IOCount�� 0�̸� ���� �� false ��ȯ
	//--------------------------------------------
	virtual bool DecrementSessionIoCount(CSession* _session) override final;

	bool SendCompletionRoutine(CSession* _session);
	bool RecvCompletionRoutine(CSession* _session);

public:
	/////////////������ ���� �Լ���///////////////////////////////
	bool Start();
	int GetSessionMaxCount();
	//---------------------------------------------
	// ULONG64 �������� ID���� �ش��ϴ� 6����Ʈ�� �����ϴ� �Լ�
	//---------------------------------------------
	static ULONG64 GetID(ULONG64 target);
	//---------------------------------------------
	// ULONG64 �������� Index���� �ش��ϴ� 2����Ʈ�� �����ϴ� �Լ�
	//---------------------------------------------
	static unsigned short GetIndex(ULONG64 target);
	//---------------------------------------------
	// ���������� �ٸ� ���ǿ� �޼����� ������ �� �Լ�
	//---------------------------------------------
	bool SendPacket(ULONG64 playerId, CPacket* buf);
	//---------------------------------------------
	// ���������� ���� ���� ��û �Լ�
	//---------------------------------------------
	bool DisconnectSession(ULONG64 sessionID);

	void EnqueLog(const char* string);
	void EnqueLog(std::string& string);
	//--------------------------------------------
	// �ڵ鷯 �Լ�
	// �����Լ��� ��ӹ޾Ƽ� �����ؼ� ���
	//--------------------------------------------
	virtual void _OnMessage(CPacket* SBuf, ULONG64 ID) = 0;
	virtual void _OnAccept(ULONG64 ID) = 0;
	virtual void _OnDisConnect(ULONG64 ID) = 0;
	virtual void _OnSend(ULONG64 ID) = 0;

	void ExitNetWorkManager();
	//--------------------------------------------
	// NetWorkLibrary�� PQCS�� SendAllSession�� ��û�ϴ� �Լ�
	//--------------------------------------------
	void EnqueSendRequest();
	//--------------------------------------------
	// ��� ���� ���� ����(CancelIoȣ��) ��Ű�� �Լ�
	//--------------------------------------------
	void DisconnectAllSessions();

	void RegistConcurrentCount(int pCount);
	void RegistSessionMaxCount(int pCount);
	void RegistWorkerThreadCount(int pCount);
	void RegistPortNum(int pPortNum);

	bool ConnectServer(std::wstring ip, unsigned short portNum, ULONG64* outSessionID);

};

