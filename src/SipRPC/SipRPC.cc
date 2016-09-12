#include "SipRPC.h"
#include "json/json.h"

using namespace siprpc;

boost::asio::io_service SipRPC::ms_io_service;
boost::thread_group SipRPC::ms_thread_group;
boost::shared_ptr<boost::asio::io_service::work > SipRPC::msp_io_service_worker;

SipRPC::SipRPC(const string & ip, 
			   const string & port, 
			   const string & method,
			   rpc_data_ptr sp_req_data) : m_io_service(ms_io_service),
										   m_resolver(ms_io_service),
										   m_socket(ms_io_service),
										   m_ip(ip),
										   m_port(port),
										   m_method(method),
										   msp_req_data(sp_req_data),
										   msp_res_data(new SipRpcData())

{
}

SipRPC::~SipRPC(void)
{
	//cout << "delete : " << (unsigned long) this << endl;
}

void SipRPC::RUN()
{
	ms_io_service.run();
	cout << "io serviec end." << endl;
}

bool SipRPC::Initialize()
{
	msp_io_service_worker.reset(new boost::asio::io_service::work(ms_io_service));
	//ms_thread_group.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &ms_io_service)));
	ms_thread_group.add_thread(new boost::thread(boost::bind(&SipRPC::RUN)));

	return true;
}

rpc_data_ptr SipRPC::Request(const string & ip, 
							 const string & port, 
							 const string & method, 
							 rpc_data_ptr sp_data)
{

	siprpc_ptr sp_rpc(new SipRPC(ip, port, method, sp_data));

	sp_rpc->start();

	if(0 == sp_rpc->wait_finish())
		return sp_rpc->msp_res_data;
	else
		return rpc_data_ptr();
}

void SipRPC::start()
{
	ms_io_service.post(boost::bind(&SipRPC::start_resolve, shared_from_this()));
}

void SipRPC::start_resolve()
{
	static int cnt = 0;

	//cout << "req cnt : " << ++cnt << endl;

	tcp::resolver::query query(m_ip, m_port);
    m_resolver.async_resolve(query,
							boost::bind(&SipRPC::handle_resolve, shared_from_this(),
										boost::asio::placeholders::error,
										boost::asio::placeholders::iterator));
}

int SipRPC::wait_finish()
{
	return m_wait_deq.PopFront();
}

void SipRPC::handle_resolve(const boost::system::error_code& err, 
							tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
    {
		boost::asio::async_connect(m_socket, endpoint_iterator,
								 boost::bind(&SipRPC::handle_connect, shared_from_this(),
											 boost::asio::placeholders::error));
    }
    else
    {
		handle_error();
    }
}

void SipRPC::handle_connect(const boost::system::error_code& err)
{
	if (!err)
    {
		Json::Value root;

		for(size_t i=0; i<msp_req_data->data_vec.size(); ++i)
		{
			root["data"].append(msp_req_data->data_vec[i]);
		}

		Json::StyledWriter writer;
		m_req_body = writer.write(root);

		msp_send_header.reset(new HttpHeader(HttpHeader::HEAD_REQUEST_POST));
		msp_send_header->Url("/"+m_method);
		msp_send_header->Host(m_ip);
		msp_send_header->ContentLength(m_req_body.size());

		vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(msp_send_header->Data().c_str(), msp_send_header->Data().size()));
		buffers.push_back(boost::asio::buffer(m_req_body.c_str(), m_req_body.size()));

		boost::asio::async_write(m_socket, buffers,
								boost::bind(&SipRPC::handle_write, shared_from_this(),
											boost::asio::placeholders::error));
    }
    else
    {
		handle_error();
    }
}

void SipRPC::handle_write(const boost::system::error_code & err)
{
	if(!err)
	{
		m_socket.async_read_some(boost::asio::buffer(m_buffer), 
								 boost::bind(&SipRPC::handle_read, shared_from_this(),
											 boost::asio::placeholders::error,
											 boost::asio::placeholders::bytes_transferred) );
	}else
	{
		cout << "error write : " << err.message() << endl;
		handle_error();
	}
}

void SipRPC::handle_read(const boost::system::error_code & err, size_t bytes_transferred)
{
	static int cnt = 0;

	//cout << "handle read cnt : " << ++cnt << endl;

	if(!err)
	{
		boost::logic::tribool result = parse_recv_data(m_buffer.data(), bytes_transferred);

		if(result)
		{
			//cout << "handle_read success." << endl;
			boost::system::error_code ignored_ec;
			m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			m_socket.close();

			//wake up user thread
			//cout << "ip/port : " << m_ip << ":" << m_port << endl;
			//cout << "bytes_transferred : " << bytes_transferred << endl;
			//cout << (unsigned long) this << endl;
			m_wait_deq.PushBack(0);

			return ;

		}else if( !result )
		{
			cout << "handle_read failed." << endl;
			boost::system::error_code ignored_ec;
			m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			m_socket.close();
			
			handle_error();
			return;

		}else
		{
			//cout << "handle_read keep." << endl;
			//继续接收
			m_socket.async_read_some(boost::asio::buffer(m_buffer), 
							 boost::bind(&SipRPC::handle_read, shared_from_this(),
										 boost::asio::placeholders::error,
										 boost::asio::placeholders::bytes_transferred) );
		}


	}else
	{
		handle_error();
	}
}

boost::logic::tribool SipRPC::parse_recv_data(const char * data, const size_t len)
{
	assert(data);

	if(!msp_recv_header)
	{
		boost::logic::tribool ret = m_recv_header_stream.Write(data, len);

		//cout << "ret = " << ret << " , len = " << len << " , unhandle = " << m_recv_header_stream.UnhandleSize() << endl;

		if(ret)
		{
			msp_recv_header = m_recv_header_stream.Header();

			if(m_recv_header_stream.UnhandleSize() > 0)
				return handle_recv_data(data + len - m_recv_header_stream.UnhandleSize(), m_recv_header_stream.UnhandleSize());
			else
				return true;
		}else
		{
			return ret;
		}
	}

	return handle_recv_data(data, len);
	
}

boost::logic::tribool SipRPC::handle_recv_data(const char * data, const size_t len)
{
	assert(data);

	for(size_t i=0; i<len; ++i)
	{
		m_res_body.push_back(data[i]);
	}

	if(m_res_body.size() < msp_recv_header->ContentLength())
		return boost::indeterminate;

	//JSON 格式解析
	Json::Reader reader;
	Json::Value root;

	try
	{
		if(false == reader.parse(m_res_body, root))
		{
			cout << "Parse json request failed : root" << endl;
			throw -1;
		}

		const Json::Value & c_root = root;

		//fileUrl
		const Json::Value & data_node = c_root["data"];
		if(data_node.type() != Json::arrayValue)
		{
			cout << "Parse json request failed : data" << endl;
			throw -1;
		}

		Json::Value::const_iterator iter = data_node.begin();
		for(; iter != data_node.end(); ++iter)
		{
			const Json::Value & val = *iter;
			
			if(val.type() != Json::stringValue)
			{
				cout << "Parse json request failed : data elem" << endl;
				throw -1;
			}

			msp_res_data->data_vec.push_back(val.asString());
		}


	}catch(...)
	{
		return false;
	}

	return true;
}

void SipRPC::handle_error()
{
	//wake up user thread
	m_wait_deq.PushBack(-1);

	return ;
}