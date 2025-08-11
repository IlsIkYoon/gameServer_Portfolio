#pragma once

#pragma comment(lib, "NetWorkLib.lib")

#include "NetWorkLib_Unified.h"

#define SESSION_PER_THREAD 640

#define NetWorkProtocol 0xa9

#define SECTOR_RATIO 100 //100���� 1 ���� 6400���� ������ �������� ���� �ϴ°� ����
#define FrameRate 25
#define FrameSec (1000 / FrameRate)
#define FrameError 20

#define TRUST_MODE 0

//----------------------------------------------------------------
//# ȭ�� �̵�����--------------------------
//----------------------------------------------------------------
#define dfRANGE_MOVE_TOP	0
#define dfRANGE_MOVE_LEFT	0
#define dfRANGE_MOVE_RIGHT	6400
#define dfRANGE_MOVE_BOTTOM	6400

#define MAX_SECTOR_X dfRANGE_MOVE_RIGHT/SECTOR_RATIO
#define MAX_SECTOR_Y dfRANGE_MOVE_BOTTOM/SECTOR_RATIO



//-----------------------------------------------------------------
// �̵� ����üũ ����
//-----------------------------------------------------------------
#define dfERROR_RANGE		50 //���߿� ����


#define PLAYERMAXCOUNT 5000

#define LL 0x00
#define RR 0x04


#define dfERROR_RANGE		50

//// 30�� �̻��� �ǵ��� �ƹ��� �޽��� ���ŵ� ���°�� ���� ����.
#define dfNETWORK_PACKET_RECV_TIMEOUT	30000


//-----------------------------------------------------------------
// ĳ���� �̵� �ӵ�   // 25fps ���� �̵��ӵ�
//-----------------------------------------------------------------
#define dfSPEED_PLAYER_X	6	// 3   50fps
#define dfSPEED_PLAYER_Y	4	// 2   50fps



//-----------------------------------------------------------------
// ĳ���� ���� 
//-----------------------------------------------------------------
#define dfPACKET_MOVE_DIR_LL					0
#define dfPACKET_MOVE_DIR_LU					1
#define dfPACKET_MOVE_DIR_UU					2
#define dfPACKET_MOVE_DIR_RU					3
#define dfPACKET_MOVE_DIR_RR					4
#define dfPACKET_MOVE_DIR_RD					5
#define dfPACKET_MOVE_DIR_DD					6
#define dfPACKET_MOVE_DIR_LD					7



//---------------------------------
// Session Status
//---------------------------------
#define NotAlive 0
#define ReadyToAction 1
#define ProcessInAction 2
#define ReadyToComplete 3
#define ActionComplete 4





#define STRINGCOUNT 20
#define SEND_PROBABILITY 5


#define THREAD_ACTION_INCOMPLETE 0
#define THREAD_ACTION_COMPLETE 1
