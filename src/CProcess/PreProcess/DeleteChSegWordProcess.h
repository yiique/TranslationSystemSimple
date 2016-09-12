#ifndef DELETE_CHSEG_WORD_PROCESS_H
#define DELETE_CHSEG_WORD_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "EventEngine/EventEngine.h"
#include "Common/MsgType.h"
#include "GeneralOperation/GeneralOperation.h"
#include "Common/f_utility.h"

//STL
#include <string>
#include <iostream>
using namespace std;


class DeleteChSegWordProcess
	: public CProcess
{
public:
	DeleteChSegWordProcess(EventEngine * p_owner, 
							const CallID & cid, 
							NetPacket & inpacket, 
							NetPacket & outpacket);

	~DeleteChSegWordProcess(void);

	//��д����Begin����
	ProcessRes Begin();

	//��д����Work����
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(DeleteChSegWordReq * p_req);
	bool package_packet(DeleteChSegWordRes * p_res);
	
};


#endif //DELETE_CHSEG_WORD_PROCESS_H
