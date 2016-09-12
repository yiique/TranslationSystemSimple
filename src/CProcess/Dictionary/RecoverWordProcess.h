#ifndef RECOVER_WORD_PROCESS_H
#define RECOVER_WORD_PROCESS_H


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


class RecoverWordProcess
	: public CProcess
{
public:
	RecoverWordProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~RecoverWordProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(WordRecoverReq * p_recover_req);
	bool package_packet(WordRecoverRes * p_recover_res);
	
};


#endif
