#ifndef MODIFY_WORD_PROCESS_H
#define MODIFY_WORD_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "EventEngine/EventEngine.h"
#include "Common/MsgType.h"
#include "DictLibrary/DictProcessor.h"
#include "Common/f_utility.h"

//STL
#include <string>
#include <iostream>
using namespace std;


class ModifyWordProcess
	: public CProcess
{
public:
	ModifyWordProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~ModifyWordProcess(void);

	//��д����Begin����
	ProcessRes Begin();

	//��д����Work����
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(WordModifyReq * p_modify_req);
	bool package_packet(WordModifyRes * p_modify_res);
	
};


#endif //MODIFY_WORD_PROCESS_H
