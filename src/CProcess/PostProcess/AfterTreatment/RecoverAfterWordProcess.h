#ifndef RECOVER_AFTER_WORD_PROCESS_H
#define RECOVER_AFTER_WORD_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "Common/MsgType.h"
#include "PostProcessor/AfterTreatment/AfterTreatEvent.h"
#include "GeneralOperation/GeneralOperation.h"
#include "Common/f_utility.h"

//STL
#include <string>
#include <iostream>
using namespace std;


class RecoverAfterWordProcess
	: public CProcess
{
public:
	RecoverAfterWordProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~RecoverAfterWordProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(AfterWordRecoverReq * p_recover_req);
	bool package_packet(AfterWordRecoverRes * p_recover_res);
	
};


#endif
