#include "SerSessManager.h"

//唯一实例
SerSessManager theSerSessionManager(NetInterface::GetSerNet());

SerSessManager * SerSessManager::GetInstance()
{
	return &theSerSessionManager;
}

SerSessManager::SerSessManager(AsyncNetDriver * p_net_driver): SessionManager(p_net_driver)
{
}

SerSessManager::~SerSessManager(void)
{
}

bool SerSessManager::Start()
{
	if( !SessionManager::Start() )
		return false;

	lout << " SerSessManager this_ptr = " << (unsigned long ) this << endl;
	return true;
}


CProcess * SerSessManager::create_process(CSession & session)
{
	//目前服务器Session只接受心跳包
	if(!session._recv_packet.IsGood())
	{
		lerr << "ERROR: session's recv packet is not good." << endl;
		return NULL;
	}

	string msg_type;
	if(!session._recv_packet.GetMsgType(msg_type))
	{
		lerr << "ERROR: get recv packet's msg type failed." << endl;
		return NULL;
	}

	if( MSG_TYPE_PULSE == msg_type )
	{
		return new PulseProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else
	{
		lerr << "ERROR: Can't find this msg type : " << msg_type <<endl;
		return NULL;
	}
}

