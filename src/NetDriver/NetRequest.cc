
#include "NetRequest.h"

NetRequest::NetRequest(void): m_src_sockfd(0)
{
}


NetRequest::NetRequest(sock_t sockfd): m_src_sockfd(sockfd)
{
}

NetRequest::~NetRequest(void)
{
}

ListenNetReq::ListenNetReq(sock_t sockfd, const string & ip, const string & port): NetRequest(sockfd), _ip(ip), _port(port)
{
}

ListenNetReq::~ListenNetReq(void)
{
}

ConnNetReq::ConnNetReq(sock_t sockfd, const string & ip, const string & port): NetRequest(sockfd), _ip(ip), _port(port)
{
}

ConnNetReq::~ConnNetReq(void)
{
}


CloseNetReq::CloseNetReq(sock_t sockfd): NetRequest(sockfd)
{
};

CloseNetReq::~CloseNetReq(void)
{
}


RecvNetReq::RecvNetReq(sock_t sockfd, NetPacket * p_recv_packet): NetRequest(sockfd),
															   mp_recv_packet(p_recv_packet)
{
}

RecvNetReq::~RecvNetReq(void)
{
}

SendNetReq::SendNetReq(sock_t sockfd, NetPacket * p_send_packet): NetRequest(sockfd),
															   mp_send_packet(p_send_packet)
{
}

SendNetReq::~SendNetReq(void)
{
}


const EDType ListenNetReq::GetType() const
{
	return EDTYPE_LISTEN_NET_REQ;
}

const EDType ConnNetReq::GetType() const
{
	return EDTYPE_CONN_NET_REQ;
}

const EDType CloseNetReq::GetType() const
{
	return EDTYPE_CLOSE_NET_REQ;
}


const EDType RecvNetReq::GetType() const
{
	return EDTYPE_RECV_NET_REQ;
}


const EDType SendNetReq::GetType() const
{
	return EDTYPE_SEND_NET_REQ;
}

