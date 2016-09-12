#ifndef INSERT_CHSEG_WORD_PROCESS_H
#define INSERT_CHSEG_WORD_PROCESS_H


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


class InsertChSegWordProcess
	: public CProcess
{
public:
	InsertChSegWordProcess(EventEngine * p_owner, 
							const CallID & cid, 
							NetPacket & inpacket, 
							NetPacket & outpacket);

	~InsertChSegWordProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(InsertChSegWordReq * p_req);
	bool package_packet(InsertChSegWordRes * p_res);
	
};


#endif //INSERT_CHSEG_WORD_PROCESS_H
