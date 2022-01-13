#include"stdafx.h"
#include"network.h"
#include"Gameworld.h"
#include"MessageQueue.h"

int main(int argc, char* argv[])
{
	Status status;
	NetThreadData netData{62629, &status};
	std::thread net(networkThread, (void*)&netData);
	Gameworld world(&status);
	
	net.join();

	WSACleanup();

	return 0;
}