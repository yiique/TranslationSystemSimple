#ifndef RECOVERY_CHSEG_WORD_PROCESS_H
#define RECOVERY_CHSEG_WORD_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "EventEngine/EventEngine.h"
#include "Common/MsgType.h"
#include "GeneralOperation/GeneralOperation.h"
#include "Common/f_utility.h"

//STL
#include <string>
#include <iostream>
using namespace std;


class RecoveryChSegWordProcess
	: public CProcess
{
public:
	RecoveryChSegWordProcess(EventEngine * p_owner, 
							const CallID & cid, 
							NetPacket & inpacket, 
							NetPacket & outpacket);

	~RecoveryChSegWordProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(RecoveryChSegWordReq * p_req);
	bool package_packet(RecoveryChSegWordRes * p_res);
	
};


#endif //RECOVERY_CHSEG_WORD_PROCESS_H
