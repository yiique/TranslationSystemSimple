#ifndef CREATE_DICT_PROCESS_H
#define CREATE_DICT_PROCESS_H


//MAIN
#include "Common/BasicType.h"
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "Common/MsgType.h"
#include "DictLibrary/DictProcessor.h"
#include "Common/f_utility.h"

//STL
#include <string>
#include <iostream>
using namespace std;


class CreateDictProcess
	: public CProcess
{
public:
	CreateDictProcess(EventEngine * p_owner, 
					  const CallID & cid, 
					  NetPacket & inpacket, 
					  NetPacket & outpacket);

	~CreateDictProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet(DictCreateReq * p_dict_create);
	bool package_packet(DictCreateRes * p_dict_create_res);
	
};


#endif //CREATE_DICT_PROCESS_H
