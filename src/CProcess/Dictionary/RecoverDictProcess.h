#ifndef RECOVER_DICT_PROCESS_H
#define RECOVER_DICT_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "EventEngine/EventEngine.h"
#include "DictLibrary/DictProcessor.h"
#include "Common/f_utility.h"
#include "Common/MsgType.h"


//STL
#include <string>
#include <iostream>
using namespace std;


class RecoverDictProcess
	: public CProcess
{
public:
	RecoverDictProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~RecoverDictProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(DictRecoverReq * p_recover_req);
	bool package_packet(DictRecoverRes * p_recover_res);
	
};


#endif
