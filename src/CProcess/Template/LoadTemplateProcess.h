#ifndef LOAD_TEMPLATE_PROCESS_H
#define LOAD_TEMPLATE_PROCESS_H


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


class LoadTemplateProcess : public CProcess
{
public:
	LoadTemplateProcess(EventEngine * p_owner, 
		const CallID & cid, 
		NetPacket & inpacket, 
		NetPacket & outpacket);

	~LoadTemplateProcess(void);

	//��д����Begin����
	ProcessRes Begin();

	//��д����Work����
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(TemplateLoadReq * p_load_req);
	bool package_packet(TemplateLoadRes * p_load_res);

};


#endif
