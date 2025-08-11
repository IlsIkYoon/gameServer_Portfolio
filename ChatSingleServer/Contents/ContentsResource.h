#pragma once


#include "NetWorkLib_Unified.h"
#include <conio.h>

#define __JOBQLOCKFREEQ_

#define SERVER_ID 0xffffffffffffffff

#define NetWorkProtocol 0xa9

#define SECTOR_RATIO 100 //100���� 1 ���� 6400���� ������ �������� ���� �ϴ°� ����
#define FrameRate 25
#define FrameSec (1000 / FrameRate)
#define FrameError 20

//----------------------------------------------------------------
//# ȭ�� �̵�����-------------------------- -
//----------------------------------------------------------------
#define dfRANGE_MOVE_TOP	0
#define dfRANGE_MOVE_LEFT	0
#define dfRANGE_MOVE_RIGHT	6400
#define dfRANGE_MOVE_BOTTOM	6400

//-----------------------------------------------------------------
// �̵� ����üũ ����
//-----------------------------------------------------------------
#define dfERROR_RANGE		50 //���߿� ����

#define PLAYERMAXCOUNT 20000

#define LL 0x00
#define RR 0x04

#define dfERROR_RANGE		50

//// 30�� �̻��� �ǵ��� �ƹ��� �޽��� ���ŵ� ���°�� ���� ����. (��Ȱ��ȭ)
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