#include "DBConnector.h"
#include "CommonProtocol.h"


bool CDBManager::LoginDataRequest(CPacket* message, ULONG64 characterKey)
{
	//ä���� �ϴ� ������
	//		BYTE	Status				// 0 (���ǿ���) / 1 (����) ...  �ϴ� defines ���
	//
	//		WCHAR	ID[20]				// ����� ID		. null ����
	//		WCHAR	Nickname[20]		// ����� �г���	. null ����

	BYTE Status;
	WCHAR ID[20];
	WCHAR Nickname[20];

	//DBQuery������ ��� Ȯ�� ���ϰ��� ���� Status�� �� �ֱ�
	//�ű⼭ ���� ������ ID�� NickName�� �־��ֱ�
	//����� ��ȸ ���� �� �����ϴ� �������� ����
	Status = en_PACKET_CS_LOGIN_RES_LOGIN::dfLOGIN_STATUS_OK;
	wcscpy_s(ID, L"ID_FIXED");
	wcscpy_s(Nickname, L"NICKNAME_FIXED");

	*message << Status;
	message->PutData((char*)ID, sizeof(WCHAR) * 20);
	message->PutData((char*)Nickname, sizeof(WCHAR) * 20);

	return true;
}