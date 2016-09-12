#include "Acceptor.h"
#include "Log/LogStream.h"
#include <boost/bind.hpp>

Acceptor::Acceptor(const sock_t sockfd, 
				   const string & ip, 
				   const string & port, 
				   EventEngine * p_owner, 
				   ConnectionManager & connection_manager,
				   UidGenerator & uid_generator,
				   boost::asio::io_service & io_service): m_sockfd(sockfd),
														  m_listen_ip(ip),
														  m_listen_port(port),
														  mp_owner(p_owner),
														  m_connection_manager(connection_manager),
														  m_uid_generator(uid_generator),
														  m_io_service(io_service),
														  m_acceptor(io_service)
{
	assert(mp_owner);

	lout << "listen ip : " << m_listen_ip << endl;
	lout << "listen port : " << m_listen_port << endl;
	boost::asio::ip::tcp::resolver resolver(m_io_service);
	boost::asio::ip::tcp::resolver::query query(m_listen_ip, m_listen_port);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	m_acceptor.bind(endpoint);
	m_acceptor.listen();	
}

void Acceptor::Listen(EventEngine * p_caller, CallID cid)
{
	assert(p_caller);

	mp_caller = p_caller;
	m_callid = cid;

	start_accept();
}


void Acceptor::start_accept()
{
	sock_t sockfd = m_uid_generator.Generate();

	m_in_connection.reset(new Connection(sockfd, mp_owner, m_io_service));

	m_acceptor.async_accept(m_in_connection->Socket(),
							boost::bind(&Acceptor::handle_accept, this, boost::asio::placeholders::error) );

	
}

void Acceptor::handle_accept(const boost::system::error_code & err)
{
	if(!err)
	{
		m_connection_manager.Start(m_in_connection);

		ListenNetRes * p_listen_res = new ListenNetRes(m_sockfd, m_in_connection->Sockfd());
		p_listen_res->SetCallID(m_callid);

		Event e(p_listen_res, mp_owner);
		mp_caller->PostEvent(e);
	}

	start_accept();
}


bool AcceptorManager::Start(acceptor_ptr sp_acceptor)
{
	return m_acceptor_map.insert(make_pair(sp_acceptor->Sockfd(), sp_acceptor)).second;
}

bool AcceptorManager::Stop(const sock_t sockfd)
{
	map<sock_t, acceptor_ptr>::iterator iter = m_acceptor_map.find(sockfd);

	if(iter == m_acceptor_map.end())
		return false;

	iter->second->Stop();
	m_acceptor_map.erase(iter);
	return true;
}


acceptor_ptr AcceptorManager::Find(const sock_t sockfd)
{
	map<sock_t, acceptor_ptr>::iterator iter = m_acceptor_map.find(sockfd);

	if(iter == m_acceptor_map.end())
		return acceptor_ptr();

	return iter->second;
}



