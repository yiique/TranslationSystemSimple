#ifndef CPROCESS_H
#define CPROCESS_H

//MAIN
#include "NetDriver/NetPacket.h"
#include "EventEngine/EventData.h"
#include "EventEngine/EventEngine.h"
#include "Log/LogStream.h"
#include "TinyXml/tinyxml.h"
#include "Common/f_utility.h"
#include "Common/ErrorCode.h"

//STL
#include <iostream>
#include <string>
using namespace std;


typedef enum
{
	PROCESS_KEEP,
	PROCESS_FINISH
} ProcessRes;

typedef enum
{
	PROCESS_TYPE_NORMAL,
	PROCESS_TYPE_RTT_SEG,
	PROCESS_TYPE_RTT
} ProcessType;

class CProcess
{
public:
	CProcess(EventEngine * p_owner, const CallID & cid, NetPacket & inpacket, NetPacket & outpacket);
	virtual ~CProcess(void);

	//���̿�ʼ  
	virtual ProcessRes Begin();

	virtual ProcessType GetType() const;

	//����event_data 
	virtual ProcessRes Work(EventData * p_edata);

protected:

	EventEngine * mp_owner;
	CallID m_cid; //callid ��ʾsession����
	NetPacket & m_input_packet;
	NetPacket & m_output_packet;

	virtual ProcessRes on_error(int err_code);  //���������

};


#endif //CPROCESS_H

