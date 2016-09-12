#include "NetInterface.h"

AsyncNetDriver NetInterface::ms_usr_net;
AsyncNetDriver NetInterface::ms_ser_net;

bool NetInterface::Initialize()
{
	if(false == ms_usr_net.Start())
	{
		lerr << "Usr NetDriver Start Failed." << endl;
		return false;
	}

	if(false == ms_ser_net.Start())
	{
		lerr << "Ser NetDriver Start Failed." << endl;
		return false;
	}

	return true;
}

AsyncNetDriver * NetInterface::GetUsrNet()
{
	return &ms_usr_net;
}

AsyncNetDriver * NetInterface::GetSerNet()
{
	return &ms_ser_net;
}
