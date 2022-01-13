#pragma once
#include "stdafx.h"
#include "MessageQueue.h"


class Status
{
public:
	Status();
	bool isExit = false;
	bool isServerAlive = true;
	MessageQueue* msg = nullptr;
	
};

