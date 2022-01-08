#include "Status.h"

Status::Status()
{

}

void Status::AliveCheck()
{
	if (isServerAlive)
	{
		isServerAlive = false;
		std::cout << "SERVER OK" << std::endl;
	}

}
