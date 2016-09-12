#ifndef AFTER_MODIFY_WORD_PROCESS_H
#define AFTER_MODIFY_WORD_PROCESS_H


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


class AfterModifyWordProcess
	: public CProcess
{
public:
	AfterModifyWordProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~AfterModifyWordProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(AfterWordModifyReq * p_modify_req);
	bool package_packet(AfterWordModifyRes * p_modify_res);
	
};

#endif