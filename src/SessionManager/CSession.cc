#include "CSession.h"

CSession::CSession(const CallID & cid, const int sockfd): m_cid(cid),
													m_sockfd(sockfd),
													_recv_packet(sockfd),
													_send_packet(sockfd),
													_p_process(NULL)
{
}

CSession::~CSession(void)
{
	if(_p_process)
		lerr << "_process is not null when CSession Destructe" << endl;
}

