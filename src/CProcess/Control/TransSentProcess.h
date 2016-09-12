
#ifndef TRANS_SENT_PROCESS_H
#define TRANS_SENT_PROCESS_H

//MAIN
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "Common/BasicType.h"
#include "TransController/TransController.h"
#include "EventEngine/EventData.h"
#include "Common/MsgType.h"
#include "Log/LogStream.h"



class TransSentProcess
	: public CProcess
{
public:
	TransSentProcess(EventEngine * p_owner, const CallID & cid, NetPacket & inpacket, NetPacket & outpacket);
	~TransSentProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet();
	bool package_packet(GetTransSentRes * p_res);

	DomainType m_domain;
	UsrID m_usr_id;
	TextID m_text_id;
	size_t m_offset;
	
};


#endif //TRANS_SENT_PROCESS_H

