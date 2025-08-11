#pragma once

#include "pch.h"

//-------------------------------------------------------
// ��Ʈ��ũ �������� ���
//-------------------------------------------------------

#define NETWORK_PROTOCOL_CODE 119
#define STICKYKEY 50

#define PAYLOAD_MAX 500


#pragma pack(push, 1)

struct ServerHeader
{
	WORD len;
};

struct ClientHeader //5����Ʈ ¥�� ���
{
	unsigned char _code;
	unsigned short _len;
	unsigned char _randKey;
	unsigned char _checkSum;
};

#pragma pack(pop)