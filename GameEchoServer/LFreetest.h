#pragma once

#include "GameEchoServerResource.h"

HANDLE hExitEvent;
HANDLE threadArr[5];


CLFree_Queue<int> testCode;

unsigned int testThreadFunc(void*)
{
	DWORD retval;
	while (1)
	{
		for (int i = 0; i < 5; i++)
		{
			{
				CProfiler p("LFree_Enque");
				testCode.Enqueue(1);
			}
		}
		for (int i = 0; i < 5; i++)
		{
			{
				CProfiler p("LFree_Deque");
				testCode.Dequeue();
			}
		}

		retval = WaitForSingleObject(hExitEvent, 0);
		if (retval == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}



void LockFreeTestCode()
{
	hExitEvent = CreateEvent(NULL, true, false, NULL);
	for (int i = 0; i < 5; i++)
	{
		threadArr[i] = (HANDLE)_beginthreadex(NULL, 0, testThreadFunc, NULL, NULL, NULL);
	}
	for (int i = 0; i < 5; i++)
	{
		ResumeThread(threadArr[i]);
	}

	while (1)
	{
		printf("R : Reset || W : Write || Q : exit\n");

		if (_kbhit())
		{
			char c;
			c = _getch();
			if (c == 'r' || c == 'R')
			{
				ResetAllProfileDate();
			}
			else if (c == 'w' || c == 'W')
			{
				WriteAllProfileData();
			}
			else if (c == 'q' || c == 'Q')
			{
				SetEvent(hExitEvent);
			}
		}
		Sleep(1000);
	}


}