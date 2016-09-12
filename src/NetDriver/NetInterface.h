#ifndef NET_INTERFACE_H
#define NET_INTERFACE_H

//MAIN
#include "AsyncNetDriver.h"
#include "Log/LogStream.h"

class NetInterface
{
public:

	static bool Initialize();

	static AsyncNetDriver * GetUsrNet();
	static AsyncNetDriver * GetSerNet();

private:

	static AsyncNetDriver ms_usr_net;
	static AsyncNetDriver ms_ser_net;

};



#endif //NET_INTERFACE_H
