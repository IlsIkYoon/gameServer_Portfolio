//----------------------------------------------------------------
//ConcurrentThread 1, WorkerThread 1�� Config File�� �о����
//������ ������ �̱� �����带 ������� �����
//----------------------------------------------------------------
#include "Resource/MonitoringServerResource.h"
#include "MonitoringServer.h"
#include "Network/NetworkManager.h"
#include "Contents/ContentsManager.h"
#include "DBConnector/DBConnector.h"

extern CWanServer* g_NetworkManager;
extern CContentsManager* g_ContentsManager;
CPdhManager g_PDH;
#ifndef __MAC__
CDBManager* g_DBManager;
#endif

bool MonitoringServer()
{
	g_NetworkManager = new CWanServer;

	int portNum;
	int sessionCount;
	int ethernetCount;
	unsigned char ntProtocol;
	unsigned char stickyKey;

	txParser.GetData("MonitorServer_Config.ini");
	txParser.SearchData("portNum", &portNum);
	txParser.SearchData("sessionCount", &sessionCount);
	txParser.SearchData("ethernetCount", &ethernetCount);
	txParser.SearchData("ntProtocolCode", &ntProtocol);
	txParser.SearchData("stickyKey", &stickyKey);
	txParser.CloseData();
	
	g_NetworkManager->RegistPortNum(portNum);
	g_NetworkManager->RegistSessionMaxCount(sessionCount);
	g_NetworkManager->RegistWorkerThreadCount(1);
	g_NetworkManager->RegistConcurrentCount(1);
	g_NetworkManager->Regist_NetworkProtocolCode(ntProtocol);
	g_NetworkManager->Regist_PacketStickyKey(stickyKey);
	g_ContentsManager = new CContentsManager(g_NetworkManager);
	g_PDH.RegistEthernetMax(ethernetCount);
	g_PDH.Start();
#ifndef __MAC__
	g_DBManager = new CDBManager;
#endif
	g_NetworkManager->Start();

	CCpuUsage myCpu;

	

	Sleep(INFINITE);
	

	return true;
}

