#include "stdafx.h"
#include "Status.h"

Status::Status()
{
	msg = new MessageQueue(5);
}
