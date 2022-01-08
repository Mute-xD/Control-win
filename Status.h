#pragma once
#include "stdafx.h"



class Status
{
public:
	Status();
	bool isExit = false;
	bool isServerAlive = true;
	void AliveCheck();
};

