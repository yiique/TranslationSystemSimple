#ifndef SIP_RPC_H
#define SIP_RPC_H

//MAIN
#include "HttpHeader.h"
#include "BlockingDeque.h"

//BOOST
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

//STL
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using boost::asio::ip::tcp;

namespace siprpc
{

class SipRpcData
	: public boost::enable_shared_from_this<SipRpcData> 
{
public:
	vector<string> data_vec;
};

typedef boost::shared_ptr<SipRpcData> rpc_data_ptr;



class SipRPC
	: public boost::enable_shared_from_this<SipRPC>, 
	  public boost::noncopyable
{
public:
	static void RUN();
	static bool Initialize();
	static rpc_data_ptr Request(const string & ip, const string & port, const string & method, rpc_data_ptr sp_data);

private:
	static boost::asio::io_service ms_io_service;
	static boost::thread_group ms_thread_group;
	static boost::shared_ptr<boost::asio::io_service::work > msp_io_service_worker;

private:
	explicit SipRPC(const string & ip, 
					const string & port, 
					const string & method,
					rpc_data_ptr sp_req_data);
public:
	~SipRPC(void);

private:

	HttpHeaderStream m_recv_header_stream;
	http_header_ptr msp_send_header;
	http_header_ptr msp_recv_header;

	boost::asio::io_service & m_io_service;
	tcp::resolver m_resolver;
	tcp::socket m_socket;		

	boost::array<char, 8192> m_buffer; 

	string m_ip;
	string m_port;
	string m_method;

	rpc_data_ptr msp_req_data;
	rpc_data_ptr msp_res_data;

	string m_req_body;
	string m_res_body;

	BlockingDeque<int> m_wait_deq;

private:

	void start();
	void start_resolve();
	void handle_error();
	void handle_resolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);
	void handle_connect(const boost::system::error_code& err);
	void handle_write(const boost::system::error_code & err);
	void handle_read(const boost::system::error_code & err, size_t bytes_transferred);
	boost::logic::tribool handle_recv_data(const char * data, const size_t len);
	boost::logic::tribool parse_recv_data(const char * data, const size_t len);

	int wait_finish();
	
};

typedef boost::shared_ptr<SipRPC> siprpc_ptr;

}

#endif //SIP_RPC_H
