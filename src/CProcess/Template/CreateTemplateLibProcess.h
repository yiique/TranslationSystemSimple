#ifndef CREATE_TEMPLATELIB_PROCESS_H
#define CREATE_TEMPLATELIB_PROCESS_H


#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "Common/MsgType.h"
#include "TemplateLibrary/TemplateProcessor.h"
#include "Common/f_utility.h"


#include <string>
#include <iostream>



class CreateTemplateLibProcess : public CProcess
{
public:
	CreateTemplateLibProcess(EventEngine * p_owner, 
		const CallID & cid, 
		NetPacket & inpacket, 
		NetPacket & outpacket);

	~CreateTemplateLibProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(TemplateLibCreateReq * p_templatelib_create);
	bool package_packet(TemplateLibCreateRes * p_templatelib_create_res);

};

#endif
