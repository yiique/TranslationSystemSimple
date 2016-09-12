#ifndef CONNECTION_H
#define CONNECTION_H

//BOOST
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/function.hpp>

//MAIN
#include "EventEngine/EventEngine.h"
#include "NetPacket.h"
#include "NetType.h"
#include "Log/LogStream.h"
#include "NetResponse.h"

//STL
#include <map>
#include <string>
#include <iostream>
using namespace std;

class Connection
	: public boost::enable_shared_from_this<Connection>, 
	  private boost::noncopyable
{
public:
	explicit Connection(const sock_t sockfd, 
						EventEngine * p_owner, 
						boost::asio::io_service & io_service);
	~Connection();

	boost::asio::ip::tcp::socket & Socket() { return m_socket; }

	sock_t Sockfd() const { return m_sockfd; }
	void Connect(const string & ip, const string & port, EventEngine * p_caller, CallID cid);
	void Recv(NetPacket * p_packet, EventEngine * p_caller, CallID cid);
	void Send(NetPacket * p_packet, EventEngine * p_caller, CallID cid);
	void Stop();

private:
	boost::asio::io_service & m_io_service;

	boost::asio::ip::tcp::socket m_socket;
	boost::asio::ip::tcp::resolver m_resolver;
	boost::array<char, 18192> m_buffer;  

	sock_t m_sockfd;
	EventEngine * mp_caller;
	EventEngine * mp_owner;
	CallID m_callid;

	NetPacket * mp_recv_packet;
	NetPacket * mp_send_packet;

private:
	string m_endpoint;

private:
	

	void handle_resolve(const boost::system::error_code& err,  boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
	void handle_connect(const boost::system::error_code& err);
	void handle_read(const boost::system::error_code & e, size_t bytes_transferred);
	void handle_write(const boost::system::error_code & e);
	boost::logic::tribool parse_recv_data(const char * data, const size_t len);

	void handle_connect_success();
	void handle_recv_success();
	void handle_send_success();

	void handle_connect_error();
	void handle_recv_error();
	void handle_send_error();

};

typedef boost::shared_ptr<Connection> connection_ptr;

class ConnectionManager
	: private boost::noncopyable
{
public:
	bool Start(connection_ptr sp_conn);

	bool Stop(const sock_t sockfd);

	connection_ptr Find(const sock_t sockfd);

private:
	map<sock_t, connection_ptr> m_connection_map;
};




#endif //CONNECTION_H
