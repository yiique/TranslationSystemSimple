#ifndef MODIFY_TEMPLATELIB_PROCESS_H
#define MODIFY_TEMPLATELIB_PROCESS_H


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


class ModifyTemplateLibProcess : public CProcess
{
public:
	ModifyTemplateLibProcess(EventEngine * p_owner, 
		const CallID & cid, 
		NetPacket & inpacket, 
		NetPacket & outpacket);

	~ModifyTemplateLibProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(TemplateLibModifyReq * p_modify_req);
	bool package_packet(TemplateLibModifyRes * p_modify_res);

};


#endif
