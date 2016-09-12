#ifndef AFTER_MODIFY_DICT_PROCESS_H
#define AFTER_MODIFY_DICT_PROCESS_H


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


class AfterModifyDictProcess
	: public CProcess
{
public:
	AfterModifyDictProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~AfterModifyDictProcess(void);

	//��д����Begin����
	ProcessRes Begin();

	//��д����Work����
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(AfterDictModifyReq * p_modify_req);
	bool package_packet(AfterDictModifyRes * p_modify_res);
	
};

#endif
