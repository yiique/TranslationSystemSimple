
#ifndef OLT_DOWNLOAD_PROCESS_H
#define OLT_DOWNLOAD_PROCESS_H

//MAIN
#include "CProcess/CProcess.h"
#include "TinyXml/tinyxml.h"
#include "Common/BasicType.h"
#include "EventEngine/EventData.h"
#include "Common/MsgType.h"
#include "Log/LogStream.h"
#include "TextHandler/TextHandler.h"


class OltDownloadProcess
	: public CProcess
{
public:
	OltDownloadProcess(EventEngine * p_owner, const CallID & cid, NetPacket & inpacket, NetPacket & outpacket);
	~OltDownloadProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

private:
	bool parse_packet();
	bool package_packet(TextCreateRes * p_create_res);

	string m_download_type;
	bool m_is_bilingual;
	TextID m_tid;
	
	
};


#endif //OLT_DOWNLOAD_PROCESS_H

