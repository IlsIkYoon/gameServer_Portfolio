
#include "pch.h"
#include "TextParser.h"

bool CTextParser::GetData(const char* fileName) {

	FILE* fpRead;

	if (fopen_s(&fpRead, fileName, "rt")) {
		printf("%s ���� ���� ����\n", fileName);
		return false;
	}

	fseek(fpRead, 0, SEEK_END);
	_bufferSize = ftell(fpRead);
	fseek(fpRead, 0, SEEK_SET);

	_dataBuf = new char[_bufferSize];

	fread(_dataBuf, 1, _bufferSize, fpRead);

	fclose(fpRead);

	_dataPos = _dataBuf;

	return true;
}

void CTextParser::CloseData() 
{
	delete _dataBuf;
}

CTextParser txParser;