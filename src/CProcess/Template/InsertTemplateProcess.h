#ifndef INSERT_TEMPLATE_PROCESS_H
#define INSERT_TEMPLATE_PROCESS_H


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


class InsertTemplateProcess : public CProcess
{
public:
	InsertTemplateProcess(EventEngine * p_owner, 
		const CallID & cid, 
		NetPacket & inpacket, 
		NetPacket & outpacket);

	~InsertTemplateProcess(void);

	//��д����Begin����
	ProcessRes Begin();

	//��д����Work����
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(TemplateInsertReq * p_insert_req);
	bool package_packet(TemplateInsertRes * p_insert_res);

};


#endif
