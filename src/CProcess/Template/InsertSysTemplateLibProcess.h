#ifndef INSERT_SYS_TEMPLATELIB_PROCESS_H
#define INSERT_SYS_TEMPLATELIB_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "Common/MsgType.h"
#include "TemplateLibrary/TemplateProcessor.h"
#include "Common/f_utility.h"
#include "TemplateLibrary/TemplateDef.h"

//STL
#include <string>
#include <iostream>
using namespace std;


class InsertSysTemplateLibProcess
	: public CProcess
{
public:
	InsertSysTemplateLibProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~InsertSysTemplateLibProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(ToSysTemplateLibReq * p_templatelib_tosys_req);
	bool package_packet(ToSysTemplateLibRes * p_templatelib_tosys_res);
	
};


#endif 
