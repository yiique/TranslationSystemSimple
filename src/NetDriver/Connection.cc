#include "Connection.h"
#include <boost/bind.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

Connection::Connection(const sock_t sockfd,
					   EventEngine * p_owner, 
					   boost::asio::io_service & io_service): m_sockfd(sockfd),
															  mp_owner(p_owner),
															  m_io_service(io_service),
															  m_resolver(io_service),
															  m_socket(io_service), 
															  mp_recv_packet(NULL),
															  mp_send_packet(NULL)
{
}


Connection::~Connection()
{

}


void Connection::Connect(const string & ip, const string & port, EventEngine * p_caller, CallID cid)
{
	assert(p_caller);

	//TODO 验证状态
	mp_caller = p_caller;
	m_callid = cid;

	boost::asio::ip::tcp::resolver::query query(ip, port);
    m_resolver.async_resolve(query,
							boost::bind(&Connection::handle_resolve, this,
										boost::asio::placeholders::error,
										boost::asio::placeholders::iterator));
										 
}

void Connection::Recv(NetPacket * p_packet, EventEngine * p_caller, CallID cid)
{
	assert(p_packet && p_caller);
	
	//TODO 验证状态

	mp_recv_packet = p_packet;
	mp_caller = p_caller;
	m_callid = cid;

	m_socket.async_read_some(boost::asio::buffer(m_buffer), 
							 boost::bind(&Connection::handle_read, shared_from_this(),
										 boost::asio::placeholders::error,
										 boost::asio::placeholders::bytes_transferred) );
}


void Connection::Send(NetPacket * p_packet, EventEngine * p_caller, CallID cid)
{
	assert(p_packet && p_caller);

	//TODO 验证状态

	mp_send_packet = p_packet;
	mp_caller = p_caller;
	m_callid = cid;

	vector<boost::asio::const_buffer> buffers;

	buffers.push_back(boost::asio::buffer(mp_send_packet->GetData(), mp_send_packet->GetSize()));

	boost::asio::async_write(m_socket, buffers,
							 boost::bind(&Connection::handle_write, shared_from_this(), boost::asio::placeholders::error) );
}

void Connection::Stop()
{
	m_socket.close();
}


void Connection::handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{

	if (!err)
    {
		boost::asio::async_connect(m_socket, endpoint_iterator,
								 boost::bind(&Connection::handle_connect, this,
											 boost::asio::placeholders::error));
    }
    else
    {
		handle_connect_error();
    }
}

void Connection::handle_connect(const boost::system::error_code& err)
{
	if (!err)
    {
		handle_connect_success();
    }
    else
    {
		handle_connect_error();
    }
}


void Connection::handle_read(const boost::system::error_code & e, size_t bytes_transferred)
{
	//cout << "[recv data this time : ] size = " << bytes_transferred << endl;
	//cout << "--------------------------------------" << endl;
	//cout << string(m_buffer.data(), bytes_transferred) << endl;
	//cout << "--------------------------------------" << endl;

	if(!e)
	{
		boost::logic::tribool result = parse_recv_data(m_buffer.data(), bytes_transferred);

		if(result)
		{
			//lout << "recv success." << endl;
			handle_recv_success();

		}else if( !result )
		{
			lout << "recv failed." << endl;
			handle_recv_error();

		}else
		{
			//继续接收
			m_socket.async_read_some(boost::asio::buffer(m_buffer), 
							 boost::bind(&Connection::handle_read, shared_from_this(),
										 boost::asio::placeholders::error,
										 boost::asio::placeholders::bytes_transferred) );
		}

	}else
	{
		lerr << "handle_read error: " << e.message() << endl;
		handle_recv_error();
	}
}

void Connection::handle_write(const boost::system::error_code & e)
{
	if(!e)
	{
		handle_send_success();
	}else
	{
		handle_send_error();
	}

}


boost::logic::tribool Connection::parse_recv_data(const char * data, const size_t len)
{
	assert(data);

	PACKET_STATE_T state = mp_recv_packet->Write(data, len);

	if( state == WRITE_FINISH )
	{
		return true;
	}else if( state == WRITE_NORMAL)
	{
		return boost::indeterminate;
	}else
	{

		lout << "state = " << state << endl;
		return false;
	}

}

void Connection::handle_connect_success()
{
	assert(mp_caller);

	ConnNetRes * p_conn_res = new ConnNetRes(m_sockfd);
	p_conn_res->SetCallID(m_callid);


	//发送给调用者
	Event e(p_conn_res, mp_owner);
	mp_caller->PostEvent(e);
}

void Connection::handle_recv_success()
{
	assert(mp_caller);

	RecvNetRes * p_recv_res = new RecvNetRes(m_sockfd, mp_recv_packet);
	p_recv_res->SetResult(true); 
	p_recv_res->SetCallID(m_callid);

	//返回响应类
	Event e(p_recv_res, mp_owner);
	mp_caller->PostEvent(e);
}

void Connection::handle_send_success()
{
	assert(mp_caller);

	SendNetRes * p_send_res = new SendNetRes(m_sockfd, mp_recv_packet);
	p_send_res->SetResult(true); 
	p_send_res->SetCallID(m_callid);

	//返回响应类
	Event e(p_send_res, mp_owner);
	mp_caller->PostEvent(e);
}

void Connection::handle_connect_error()
{
	assert(mp_caller);

	ConnNetRes * p_conn_res = new ConnNetRes(m_sockfd);
	p_conn_res->SetResult(false); 
	p_conn_res->SetCallID(m_callid);


	//发送给调用者
	Event e(p_conn_res, mp_owner);
	mp_caller->PostEvent(e);
}


void Connection::handle_recv_error()
{
	assert(mp_caller);

	RecvNetRes * p_recv_res = new RecvNetRes(m_sockfd, mp_recv_packet);
	p_recv_res->SetResult(false); 
	p_recv_res->SetCallID(m_callid);

	//返回响应类
	Event e(p_recv_res, mp_owner);
	mp_caller->PostEvent(e);
}

void Connection::handle_send_error()
{
	assert(mp_caller);

	SendNetRes * p_send_res = new SendNetRes(m_sockfd, mp_recv_packet);
	p_send_res->SetResult(false); 
	p_send_res->SetCallID(m_callid);

	//返回响应类
	Event e(p_send_res, mp_owner);
	mp_caller->PostEvent(e);
}

bool ConnectionManager::Start(connection_ptr sp_conn)
{
	return m_connection_map.insert(make_pair(sp_conn->Sockfd(), sp_conn)).second;
}

bool ConnectionManager::Stop(const sock_t sockfd)
{
	map<sock_t, connection_ptr>::iterator iter = m_connection_map.find(sockfd);

	if(iter == m_connection_map.end())
		return false;

	iter->second->Stop();
	m_connection_map.erase(iter);
	return true;
}


connection_ptr ConnectionManager::Find(const sock_t sockfd)
{
	map<sock_t, connection_ptr>::iterator iter = m_connection_map.find(sockfd);

	if(iter == m_connection_map.end())
		return connection_ptr();

	return iter->second;
}
