#ifndef ACCEPTOR_H
#define ACCEPTOR_H

//BOOST
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

//MAIN
#include "NetType.h"
#include "UidGenerator.h"
#include "EventEngine/EventEngine.h"
#include "Connection.h"
#include "NetResponse.h"

//STL
#include <string>
#include <iostream>
#include <set>
#include <map>
using namespace std;

class AcceptorManager;

class Acceptor
	: private boost::noncopyable, 
	  public boost::enable_shared_from_this<Acceptor>
{
public:
	explicit Acceptor(const sock_t sockfd, 
					  const string & ip, 
					  const string & port, 
					  EventEngine * p_owner,
					  ConnectionManager & connection_manager,
					  UidGenerator & uid_generator,
					  boost::asio::io_service & io_service);
	
	void Listen(EventEngine * p_caller, CallID cid);
	void Stop() { m_acceptor.close(); }

	sock_t Sockfd() const { return m_sockfd; }

private:

	sock_t m_sockfd;
	boost::asio::io_service & m_io_service;
	EventEngine * mp_caller;
	EventEngine * mp_owner;
	CallID m_callid;
	UidGenerator & m_uid_generator;
	boost::asio::ip::tcp::acceptor m_acceptor;

	const string & m_listen_ip;
	const string & m_listen_port;

	connection_ptr m_in_connection;

	ConnectionManager & m_connection_manager;

private:
	void start_accept();
	void handle_accept(const boost::system::error_code & err);
};

typedef boost::shared_ptr<Acceptor> acceptor_ptr;

class AcceptorManager
{
public:
	bool Start(acceptor_ptr sp_acceptor);

	bool Stop(const sock_t sockfd);
	acceptor_ptr Find(const sock_t sockfd);

private:
	map<sock_t, acceptor_ptr> m_acceptor_map;
};


#endif //ACCEPTOR_H
