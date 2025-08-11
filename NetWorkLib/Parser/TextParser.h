#ifndef __PARSER__
#define __PARSER__
#include "pch.h"

// ����
// txParser��� ���� �Ǿ� �ִ� ���� ��ü�� �̿��ؼ� ���
// 
// 1. getdata : �ؽ�Ʈ ������ ��� ���ۿ� �������ִ� �Լ�
// 2. search data : ���ڿ��� �Է��ϸ�(key) �� value�� ä���ִ� �Լ� 
// 3. close data : �ڷḦ �� �о ���������� ���۸� �������ִ� �Լ�
//

class CTextParser {
	char* _dataBuf;
	char* _dataPos;
	char* _strBuf;
	int _bufferSize;

public:
	bool GetData(const char* fileName);

	void SkipNoneCommand() {
		while (1) {
			if (*_dataPos == ' ' || *_dataPos == '\n' || *_dataPos == 0x09 || *_dataPos == 0x08)
			{
				_dataPos++;
			}
			else if (*_dataPos == '/' && *(_dataPos + 1) == '/')
			{
				while (*_dataPos != '\n') {
					_dataPos++;
				}

			}
			else if (*_dataPos == '/' && (*_dataPos + 1) == '*') {
				while ((*_dataPos - 1) != '*' || *_dataPos != '/')
				{
					_dataPos++;

					if (_dataPos >= _dataBuf + _bufferSize - 1) {
						// �ּ� ���� ���ڰ� ���µ� ������ ���� �����ϸ� ������ ����
						printf("�ּ� ó�� ���� \n");
						return;
					}
				}
				_dataPos++;
			}
			else {
				break;
			}
		}
	}

	template <typename T>
	bool SearchData(const char* targetKey, T* targetBuf) {
		//������ ��츦 ����, ���ڿ��� Ư��ȭ�� �߰� ��� �ʿ�
		_dataPos = _dataBuf; // ó������ ã��

		size_t targetLen = strlen(targetKey);
		char* dataKey = new char[targetLen + 1];
		while (1) {
			if (_dataPos >= _dataBuf + _bufferSize - 1) {
				std::cout << "��ȿ�� �����Ͱ� ���ο� �����ϴ�" << std::endl;
				delete[] dataKey;
				return false;
			}
			memset(dataKey, 0, targetLen + 1);

			SkipNoneCommand(); //��ȿ�� ���ڿ����� ����

			int j = 0;
			while (1)
			{
				//���ڿ��� �޾Ƽ� �����ϰ� 
				dataKey[j] = *_dataPos;

				_dataPos++;
				j++;

				if (*_dataPos == ' ' || *_dataPos == '\n' || *_dataPos == 0x09 || *_dataPos == 0x08 ||
					j > targetLen - 1 ||
					(*_dataPos == '/' && *(_dataPos + 1) == '/') || (*_dataPos == '/' && *(_dataPos + 1) == '*'))
				{
					break;
				}
			}
			dataKey[targetLen] = NULL; //�迭 ������ ���� �� ���ڸ� �־��ְ� 
			if (strcmp(dataKey, targetKey) == 0) {
				//�´� ��� (Ʋ���� ���� �ٽ� �ݺ������� ���ư�
				SkipNoneCommand();
				//�� ��ȣ�� �´� ����
				if (*_dataPos == '=') {
					_dataPos++;
					SkipNoneCommand();

					if (*_dataPos >= 0x30 && *_dataPos <= 0x39) //ù ���ڰ� ������ ������ ����ִٸ� ���ڷ� ����
					{
						char numBuf[20];
						int i = 0;
						while (*_dataPos >= 0x30 && *_dataPos <= 0x39)
						{
							numBuf[i] = *_dataPos;
							_dataPos++;
							i++;
						}
						numBuf[i] = NULL;
						*targetBuf = atoi(numBuf);

						delete[] dataKey;
						return true;
					}
					else if (*_dataPos >= 0x21 && *_dataPos <= 0x7E) { // ù ���ڰ� ���ڰ� �ƴ϶�� ���ڿ��� ����
						int i = 0;
						while (*_dataPos >= 0x21 && *_dataPos <= 0x7E) {
							*(targetBuf + i) = *_dataPos;
							_dataPos++;
							i++;
						}
						*(targetBuf + i) = NULL;
						//���� �������� ���� ��å�� ���� �� ������� 

						delete[] dataKey;
						return true;
					}

				}
				//���� ������ �̿� ������Ʈ�� ���� ����
			}
		}
	}

	void CloseData();
};
//��� ������ ����� ���� Ŭ���� ����
extern CTextParser txParser;
#endif