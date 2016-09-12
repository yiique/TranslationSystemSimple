#ifndef SER_SESS_MANAGER_H
#define SER_SESS_MANAGER_H

//MAIN
#include "SessionManager.h"
#include "Common/MsgType.h"
#include "CProcess/Control/PulseProcess.h"
#include "NetDriver/NetInterface.h"

class SerSessManager
	: public SessionManager
{
public:
	SerSessManager(AsyncNetDriver * p_net_driver);
	~SerSessManager(void);

	static SerSessManager * GetInstance();

	bool Start();

private:
	//��д����create_process����
	CProcess * create_process(CSession & session);
};


#endif //SER_SESS_MANAGER_H

