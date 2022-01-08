#include"stdafx.h"
#include"network.h"


int main()
{
	Status status;
	NetThreadData netData{62629, &status};

	std::thread net(networkThread, (void*)&netData);
	net.join();

	WSACleanup();
	return 0;
}