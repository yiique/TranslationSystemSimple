
#ifndef OLT_TRANS_PROCESS_H
#define OLT_TRANS_PROCESS_H

//MAIN
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "Common/BasicType.h"
#include "TransText/TransText.h"
#include "EventEngine/EventData.h"
#include "Common/MsgType.h"
#include "OltManager/OltTaskManager.h"
#include "Log/LogStream.h"
#include "OltManager/OltSubmitReq.h"
#include "OltManager/OltSubmitRes.h"


class OltTransProcess
	: public CProcess
{
public:
	OltTransProcess(EventEngine * p_owner, const CallID & cid, NetPacket & inpacket, NetPacket & outpacket);
	~OltTransProcess(void);

	//��д����Begin����
	ProcessRes Begin();

	//��д����Work����
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet();
	bool package_packet(OltSubmitRes * p_olt_submit_res);


	UsrID m_usr_id;
	string m_domain_name;
	string m_src_language;
	string m_tgt_language;

	vector<TextID> m_src_textid_vec;//�ļ������ݿ��е�ΨһID��
	
};


#endif //OLT_TRANS_PROCESS_H

