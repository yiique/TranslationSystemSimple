#ifndef AFTER_DELETE_DICT_PROCESS_H
#define AFTER_DELETE_DICT_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "Common/MsgType.h"
#include "PostProcessor/AfterTreatment/AfterTreatEvent.h"
#include "GeneralOperation/GeneralOperation.h"
#include "Common/f_utility.h"

//STL
#include <string>
#include <iostream>
using namespace std;


class AfterDeleteDictProcess
	: public CProcess
{
public:
	AfterDeleteDictProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~AfterDeleteDictProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(AfterDictDeleteReq * p_delete_req);
	bool package_packet(AfterDictDeleteRes * p_delete_res);
	
};

#endif 
