#pragma once
#include"stdafx.h"
#include"Status.h"
#include"Threading.h"

#define BUFFER 1024
#define HEADER_SIZE 6
#define PULSE 1000


constexpr auto PORT = 62629;

class Network
{
public:
	Network(int port, Status* status);
	void Connect();
	SOCKET sock;

	void Send(BYTE *data, int dataLen);
	void Recv();

	bool isDisconnected = false;
private:
	Status* status = nullptr;
	sockaddr_in serverSockAddress{};
	void RecvProcess(BYTE* frameData, int dataLen);
	void unexpectedPack(BYTE* frameData, int dataLen);
};
void* networkThread(void* data);
struct NetThreadData
{
	int port;
	Status* status;
};

void netJob(Network net);
void* deamonCheckThread(void* data);
void* deamonSetThread(void* data);
struct DeamonThreadData
{
	Network* net;
	Status* status;
};