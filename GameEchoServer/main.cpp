#include "GameEchoServer.h"
#include "LFreetest.h"

int main()
{
	if (GameEchoServer() == false)
	{
		printf("Server Failed !!!\n");
		__debugbreak();
	}

	return 0;
}