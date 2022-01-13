#include "stdafx.h"
#include "Network.h"

Network::Network(int port, Status* status)
{
	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		status->msg->insert("StartUp ERROR");
	}
	hostent* phst = gethostbyname("local.mutexxd.cn");
	if (phst == nullptr)
	{
		status->msg->insert("DNS ERROR");
	}
	in_addr* iddr = (in_addr*)phst->h_addr;
	const char* ip = inet_ntoa(*iddr);
	status->msg->insert(std::string("Remote IP: ")+ip);

	this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		status->msg->insert("StartUp ERROR");
	}
	serverSockAddress.sin_family = AF_INET;
	serverSockAddress.sin_addr.S_un.S_addr = inet_addr(ip);
	serverSockAddress.sin_port = htons(port);
	this->status = status;
}

void Network::Connect()
{
	status->msg->insert("Connecting to Server...");
	if (connect(this->sock, (struct sockaddr*)&serverSockAddress, sizeof(serverSockAddress)) == SOCKET_ERROR)
	{
		status->msg->insert("Connect Failure");
		closesocket(this->sock);
	}
	status->msg->insert("Server Connected!");
}

void Network::Send(BYTE* data, int dataLen)
{ 
	int dataLenTotal = dataLen + HEADER_SIZE + 4;
	if (dataLenTotal > BUFFER)
	{
		status->msg->insert("Package too big");
		return;
	}
	BYTE dataBuffer[BUFFER];
	dataBuffer[0] = 0xEF; dataBuffer[1] = 0xEF; dataBuffer[2] = 0xEF; dataBuffer[3] = 0xEF;
	dataBuffer[4] = dataLen / 256;
	dataBuffer[5] = dataLen % 256;
	memcpy(dataBuffer + HEADER_SIZE, data, dataLen);
	dataBuffer[dataLen + HEADER_SIZE] = 0xFE; dataBuffer[dataLen + HEADER_SIZE + 1] = 0xFE; dataBuffer[dataLen + HEADER_SIZE + 2] = 0xFE; dataBuffer[dataLen + HEADER_SIZE + 3] = 0xFE;
	send(sock, (char*)dataBuffer, dataLenTotal, 0);
}

void Network::Recv()
{
	int lastPos = 0;
	int dataLen;
	int dataLenTotal;
	int rcvLen;
	int remainSize;
	BYTE rcvBuffer[BUFFER];
	BYTE dataBuffer[BUFFER];
	BYTE frameData[BUFFER];
	memset(rcvBuffer, 0, sizeof rcvBuffer);
	memset(dataBuffer, 0, sizeof dataBuffer);


	rcvLen = (int)recv(this->sock, (char*)rcvBuffer, BUFFER, 0);
	if (rcvLen > 0)
	{
		memcpy(dataBuffer + lastPos, rcvBuffer, rcvLen);
		lastPos += rcvLen;
		//判断消息缓冲区的数据长度大于消息头
		while (lastPos >= HEADER_SIZE)
		{
			//包头做判断，如果包头错误，收到的数据全部清空
			if (dataBuffer[0] == 0xEF && dataBuffer[1] == 0xEF && dataBuffer[2] == 0xEF && dataBuffer[3] == 0xEF)
			{
				dataLen = (int)dataBuffer[5] + 256 * (int)dataBuffer[4];
				dataLenTotal = dataLen + HEADER_SIZE + 4;
				//判断消息缓冲区的数据长度大于消息体
				if (lastPos >= dataLenTotal)
				{
					memcpy(frameData, dataBuffer + HEADER_SIZE, dataLen);
					//处理数据
					this->RecvProcess(frameData, dataLen);
					//剩余未处理消息缓冲区数据的长度
					remainSize = lastPos - dataLenTotal;
					//将未处理的数据前移
					if (remainSize > 0)
					{
						memcpy(dataBuffer, dataBuffer + dataLenTotal,
							remainSize);
						lastPos = remainSize;
					}
				}

				else
				{
					break;
				}
			}
			else      //寻找下一个包头
			{
				bool isFind = false;
				int nFindStart = 0;
				for (int k = 1; k < lastPos; k++)
				{
					if (dataBuffer[k] == 0xEF && dataBuffer[k + 1] == 0xEF && dataBuffer[k + 2] == 0xEF && dataBuffer[k + 3] == 0xEF)
					{
						nFindStart = k;
						isFind = true;
						break;
					}
				}
				if (isFind)
				{
					memcpy(dataBuffer, dataBuffer + nFindStart, lastPos - nFindStart);

					lastPos = lastPos - nFindStart;
				}
				else
				{
					memset(dataBuffer, 0, sizeof(dataBuffer));
					lastPos = 0;
					break;
				}
			}
			break;

		}
	}	
}


void Network::RecvProcess(BYTE* frameData, int dataLen)
{
	BYTE path = frameData[0];
	BYTE type = frameData[1];
	switch (path)
	{
	case '0':
		// boardcast
		break;
	case '1':
		switch (type)
		{
		case '0':
			// s2u command
		{
			break;
		}
		case '1':
			// s2u status
		{
			if (frameData[2] == '6' && frameData[3] == '6' && frameData[4] == '6')
			{
				status->msg->insert("OK");
				status->isServerAlive = true;
			}
			break;
		}
		case '2':
			// s2u message
		{
			std::string msg;
			status->msg->insert("Message From Server");
			for (auto i = 2; i < dataLen; i++)
			{
				msg.push_back(frameData[i]);
			}
			status->msg->insert(msg);
			break;
		}
		default:
		{
			unexpectedPack(frameData, dataLen);
			break;
		}
		}
	case '6':
		switch (type)
		{
		case '0':
			// d2u command
		{
			break;
		}
		case '1':
			// d2u status
		{
			break;
		}
		case '2':
			// d2u message
		{
			std::string msg;
			status->msg->insert("Message From Downer");
			for (auto i = 2; i < dataLen; i++)
			{
				msg.push_back(frameData[i]);
			}
			status->msg->insert(msg);
			break;
		}
		default:
		{
			unexpectedPack(frameData, dataLen);
			break;
		}
		}
		break;
	default:
		unexpectedPack(frameData, dataLen);
	}
}

void Network::unexpectedPack(BYTE* frameData, int dataLen)
{
	status->msg->insert("WARNING: Unexpected Packet Received!");
	std::cout << "Flag is ->" << frameData[0] << std::endl;
	std::cout << "Type is ->" << frameData[1] << std::endl;
	for (auto i = 2; i < dataLen; i++)
	{
		std::cout << frameData[i];
	}
	std::cout << std::endl;
}

void* networkThread(void* data)
{
	auto* threadData = (struct NetThreadData*)data;

	while (!threadData->status->isExit)
	{
		auto* net = new Network(threadData->port, threadData->status);
		net->Connect();
		threadData->status->msg->insert(std::string("Start Listening on ") + std::to_string(threadData->port));
		net->isDisconnected = false;
		DeamonThreadData deamonData{net, threadData->status};
		std::thread tDeamonSet(deamonSetThread, (void*)&deamonData);
		std::thread tDeamonCheck(deamonCheckThread, (void*)&deamonData);
		while (!threadData->status->isExit && !net->isDisconnected)
		{
			netJob(*net);
		}
		closesocket(net->sock);
		tDeamonSet.join();
		tDeamonCheck.join();
		threadData->status->msg->insert("Disconnected! Reset in 5 Sec");
		Sleep(5000);

	}
	return nullptr;
}

void netJob(Network net)
{
	//BYTE* data = (BYTE*)"Hello World ";
	//net.Send(data, 13);
	net.Recv();
	Sleep(50);
}

void* deamonSetThread(void* data)
{
	auto* threadData = (struct DeamonThreadData*)data;
	while (!threadData->status->isExit && !threadData->net->isDisconnected)
	{
		Sleep(PULSE);
		BYTE* pulsePack = (BYTE*)"31666";
		threadData->net->Send(pulsePack, 6);
		threadData->status->msg->insert("Daemon: Pulse Sent");
	}
	threadData->status->msg->insert("Daemon: Set Exit");
	return nullptr;
}

void* deamonCheckThread(void* data)
{
	auto* threadData = (struct DeamonThreadData*)data;
	while (!threadData->status->isExit && !threadData->net->isDisconnected)
	{
		Sleep(PULSE * 3);
		if (threadData->status->isServerAlive)
		{
			threadData->status->isServerAlive = false;
		}
		else
		{
			threadData->status->msg->insert("Daemon: Remote No Response");
			threadData->net->isDisconnected = true;
		}
	}
	threadData->status->msg->insert("Daemon: Check Exit");
	return nullptr;
}
