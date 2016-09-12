#ifndef PULSE_PROCESS_H
#define PULSE_PROCESS_H

//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "Common/MsgType.h"
#include "TransController/TransController.h"
#include "EventEngine/EventEngine.h"

//STL
#include <string>
#include <iostream>
using namespace std;


class PulseProcess
	: public CProcess
{
public:
	PulseProcess(EventEngine * p_owner, 
				 const CallID & cid, 
				 NetPacket & inpacket, 
				 NetPacket & outpacket);

	~PulseProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet();
	bool package_packet(SlavePulseRes * p_res);

	TransType m_slave_trans_type;
	TransSlaveInfo m_slave_info;

};


#endif //PULSE_PROCESS_H

