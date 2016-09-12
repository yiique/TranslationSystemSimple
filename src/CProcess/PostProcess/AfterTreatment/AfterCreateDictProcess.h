#ifndef AFTER_CREATE_DICT_PROCESS_H
#define AFTER_CREATE_DICT_PROCESS_H


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


class AfterCreateDictProcess
	: public CProcess
{
public:
	AfterCreateDictProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~AfterCreateDictProcess(void);

	//��д����Begin����
	ProcessRes Begin();

	//��д����Work����
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(AfterDictCreateReq * p_dict_create);
	bool package_packet(AfterDictCreateRes * p_dict_create_res);
	
};

#endif
