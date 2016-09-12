
#ifndef CSESSION_H
#define CSESSION_H

//MAIN
#include "NetDriver/NetPacket.h"
#include "CProcess/CProcess.h"
#include "Log/LogStream.h"
#include "EventEngine/EventEngine.h"

typedef enum
{
	SSTATE_LISTEN,    //0
	SSTATE_RECVING,   //1
	SSTATE_SENDING,   //2
	SSTATE_CLOSING,   //3
	SSTATE_PROCESS,   //4
	SSTATE_CONNECT    //5
} SessionState;   

class CSession
{
public:
	CSession(const CallID & cid, const int sockfd);
	virtual ~CSession(void);

	const CallID & GetCallID() const
	{
		return m_cid;
	};

	int GetSock() const
	{
		return m_sockfd;
	};

	const SessionState & GetState() const
	{
		return m_state;
	};

	void SetState(const SessionState & state)
	{
		m_state = state;
	};


private:

	CallID m_cid;
	int m_sockfd;
	SessionState m_state;

public:
	NetPacket _recv_packet;
	NetPacket _send_packet;
	CProcess * _p_process;

};

#endif //CSESSION_H
