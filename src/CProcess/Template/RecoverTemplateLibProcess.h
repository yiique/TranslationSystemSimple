#ifndef RECOVER_TEMPLATELIB_PROCESS_H
#define RECOVER_TEMPLATELIB_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "EventEngine/EventEngine.h"
#include "TemplateLibrary/TemplateProcessor.h"
#include "Common/f_utility.h"
#include "Common/MsgType.h"


//STL
#include <string>
#include <iostream>
using namespace std;


class RecoverTemplateLibProcess
	: public CProcess
{
public:
	RecoverTemplateLibProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~RecoverTemplateLibProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(TemplateLibRecoverReq * p_recover_req);
	bool package_packet(TemplateLibRecoverRes * p_recover_res);
	
};


#endif
