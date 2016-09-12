#ifndef RECOVER_TEMPLATE_PROCESS_H
#define RECOVER_TEMPLATE_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "EventEngine/EventEngine.h"
#include "Common/MsgType.h"
#include "TemplateLibrary/TemplateProcessor.h"
#include "Common/f_utility.h"

//STL
#include <string>
#include <iostream>
using namespace std;


class RecoverTemplateProcess
	: public CProcess
{
public:
	RecoverTemplateProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~RecoverTemplateProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(TemplateRecoverReq * p_recover_req);
	bool package_packet(TemplateRecoverRes * p_recover_res);
	
};


#endif
