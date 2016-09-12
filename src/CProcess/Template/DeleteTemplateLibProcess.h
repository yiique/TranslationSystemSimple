#ifndef DELETE_TEMPLATELIB_PROCESS_H
#define DELETE_TEMPLATELIB_PROCESS_H


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


class DeleteTemplateLibProcess : public CProcess
{
public:
	DeleteTemplateLibProcess(EventEngine * p_owner, 
		const CallID & cid, 
		NetPacket & inpacket, 
		NetPacket & outpacket);

	~DeleteTemplateLibProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(TemplateLibDeleteReq * p_delete_req);
	bool package_packet(TemplateLibDeleteRes * p_delete_res);

};


#endif 
