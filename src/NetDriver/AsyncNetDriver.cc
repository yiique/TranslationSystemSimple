#include "AsyncNetDriver.h"


AsyncNetDriver::AsyncNetDriver(void): m_timer(m_io_service)
{
	msp_io_service_worker.reset(new boost::asio::io_service::work(m_io_service));
}


AsyncNetDriver::~AsyncNetDriver(void)
{
}




int AsyncNetDriver::run()
{
	try
	{
		m_timer.expires_from_now(boost::posix_time::seconds(5));
		m_timer.async_wait(boost::bind(&AsyncNetDriver::on_timer, this, boost::asio::placeholders::error)); 

		m_io_service.run();
	}catch(boost::system::system_error err)
	{
		lerr << "Error: " << err.what() << endl;
	}

	return 0;
}


sock_t AsyncNetDriver::Listen(const char * ip,
							  const int port,
							  EventEngine *p_caller,
							  const CallID & cid)
{
	if( !ip || !p_caller )
	{
		lerr << "ERROR: INPUT null pointer. " << endl;
		return -1;
	}

	//生成sockfd
	sock_t sockfd = m_sockfd_generator.Generate();
	if( sockfd < 0 )
	{
		
		return -1;
	}

	//生成NetRequest
	ListenNetReq *p_listen_req = new ListenNetReq(sockfd, ip, num_2_str(port));
	p_listen_req->SetCallID(cid);  //设置callid
	
	//调用父类InputEvent
	Event e(p_listen_req, p_caller);
	this->PostEvent(e);

	return sockfd;
}

sock_t AsyncNetDriver::Connect(const char * ip,
								const int port,
								const char * cliip,
								EventEngine *p_caller,
								const CallID & cid)
{
	if( !ip || !p_caller || !cliip)
	{
		lerr << "ERROR: INPUT null pointer. " << endl;
		return -1;
	}

	///生成sockfd
	sock_t sockfd = m_sockfd_generator.Generate();
	if( sockfd < 0 )
	{
		return -1;
	}

	//生成NetRequest
	ConnNetReq * p_conn_req = new ConnNetReq(sockfd, ip, num_2_str(port));
	p_conn_req->SetCallID(cid); //设置callid
	
	//调用父类InputEvent
	Event e(p_conn_req, p_caller);
	this->PostEvent(e);

	return sockfd;

}

int AsyncNetDriver::Receive(sock_t sockfd,
							NetPacket * p_packet,
							EventEngine *p_caller,
							const CallID & cid)
{
	if( !p_caller || !p_packet)
	{
		lerr << "ERROR: INPUT null pointer. " << endl;
		return -1;
	}

	//生成NetEvent
	RecvNetReq * p_recv_req = new RecvNetReq(sockfd, p_packet);
	p_recv_req->SetCallID(cid); //设置callid

	//调用父类InputEvent
	Event e(p_recv_req, p_caller);
	this->PostEvent(e);

	return 0;
}

int AsyncNetDriver::Send(sock_t sockfd,
						 NetPacket * p_packet,
						 EventEngine * p_caller,
						 const CallID & cid)
{
	if( !p_caller || !p_packet)
	{
		lerr << "ERROR: INPUT null pointer. " << endl;
		return -1;
	}

	if(p_packet->GetSize() == 0)
	{
		lerr << "ERROR: packet's size is 0. " << endl;
		return -1;
	}
	//生成NetEvent
	SendNetReq * p_send_req = new SendNetReq(sockfd, p_packet);
	p_send_req->SetCallID(cid); //设置callid

	//调用父类InputEvent
	Event e(p_send_req, p_caller);
	this->PostEvent(e);

	return 0;
}

sock_t AsyncNetDriver::Shutdown(sock_t sockfd,
						  		EventEngine *p_caller,
								const CallID & cid)
{
	if( !p_caller )
	{
		lerr << "ERROR: INPUT null pointer. " << endl;
		return -1;
	}

	//生成NetEvent
	CloseNetReq * p_close_req = new CloseNetReq(sockfd);
	p_close_req->SetCallID(cid); //设置callid

	//调用父类InputEvent
	Event e(p_close_req, p_caller);
	this->PostEvent(e);

	return 0;
}

int AsyncNetDriver::PostEvent(const Event & e)
{
	EventEngine::PostEvent(e);

	m_io_service.post(boost::bind(&AsyncNetDriver::post_handler, this));

	return 0;
}



void AsyncNetDriver::req_listen(EventData * p_edata, EventEngine * p_caller)
{
	if( !p_edata || !p_caller )
	{
		lerr << "ERROR: INPUT is null. " << endl;
		return;
	}

	ListenNetReq * p_listen_req = dynamic_cast<ListenNetReq*>(p_edata);
	assert(p_listen_req);

	//Step 1 新建acceptor
	sock_t sockfd = p_listen_req->GetSrcSock();
	acceptor_ptr sp_acceptor(new Acceptor(sockfd, p_listen_req->_ip, p_listen_req->_port, this, m_connection_manager, m_sockfd_generator, m_io_service));

	if(false == m_acceptor_manager.Start(sp_acceptor))
	{
		lerr << "Insert acceptor failed. sockfd = " << sockfd << endl;
		return;
	}

	//Step 2 启动监听
	sp_acceptor->Listen(p_caller, p_listen_req->GetCallID());

}

void AsyncNetDriver::req_connect(EventData * p_edata, EventEngine * p_caller)
{
	if( !p_edata || !p_caller )
	{
		lerr << "ERROR: INPUT is null. " << endl;
		return;
	}

	ConnNetReq *p_conn_req = dynamic_cast<ConnNetReq*>(p_edata);
	assert(p_conn_req);

	//Step 1 新建Connection
	sock_t sockfd = p_conn_req->GetSrcSock();
	connection_ptr sp_conn(new Connection(sockfd, this, m_io_service));

	if(false == m_connection_manager.Start(sp_conn) )
	{
		lerr << "Insert connection failed. sockfd = " << sockfd << endl;

		//TODO 返回错误
		return;
	}

	//Step 2 启动连接
	sp_conn->Connect(p_conn_req->_ip, p_conn_req->_port, p_caller, p_conn_req->GetCallID());
}

void AsyncNetDriver::req_close(EventData * p_edata, EventEngine * p_caller)
{
	if( !p_edata || !p_caller )
	{
		lerr << "ERROR: INPUT is null. " << endl;
		return;
	}
	
	CloseNetReq * p_close_req = dynamic_cast<CloseNetReq*>(p_edata);
	assert(p_close_req);

	//生产NetResponse类
	CloseNetRes * p_close_res = NULL;
	sock_t sockfd = p_close_req->GetSrcSock();
	

	//查找连接
	if(true == m_connection_manager.Stop(sockfd) 
		|| true == m_acceptor_manager.Stop(sockfd) )
	{
		p_close_res = new CloseNetRes(sockfd, NULL, NULL);
		p_close_res->SetResult(true);
	
		m_sockfd_generator.Recycle(sockfd);
	}else
	{
		//此socket不在表中
		p_close_res = new CloseNetRes(sockfd);
		p_close_res->SetResult(false, 0, PROTL_ERR_NOSOCK);
		
	}

	//返回响应类
	p_close_res->SetCallID(p_close_req->GetCallID()); //设置callid
	Event e(p_close_res, this);
	p_caller->PostEvent(e);

}

void AsyncNetDriver::req_recv(EventData * p_edata, EventEngine* p_caller)
{
	if( !p_edata || !p_caller )
	{
		lerr << "ERROR: INPUT is null. " << endl;
		return;
	}

	RecvNetReq * p_recv_req = dynamic_cast<RecvNetReq*>(p_edata);
	assert(p_recv_req);

	sock_t sockfd = p_recv_req->GetSrcSock();

	connection_ptr sp_conn = m_connection_manager.Find(sockfd);

	if(sp_conn)
	{
		sp_conn->Recv(p_recv_req->GetRecvPakcet(), p_caller, p_recv_req->GetCallID());
	}else
	{
		//操作出错，生产NetResponse
		RecvNetRes * p_recv_res = new RecvNetRes(sockfd, p_recv_req->GetRecvPakcet());
		p_recv_res->SetResult(false, 0, PROTL_ERR_NOSOCK); 
		p_recv_res->SetCallID(p_recv_req->GetCallID());

		//返回响应类
		Event e(p_recv_res, this);
		p_caller->PostEvent(e);
	}

}


void AsyncNetDriver::req_send(EventData * p_edata, EventEngine* p_caller)
{
	if( !p_edata || !p_caller )
	{
		lerr << "ERROR: INPUT is null." << endl;
		return;
	}

	SendNetReq *p_send_req = dynamic_cast<SendNetReq*>(p_edata);
	assert(p_send_req);

	sock_t sockfd = p_send_req->GetSrcSock();

	connection_ptr sp_conn = m_connection_manager.Find(sockfd);

	if(sp_conn)
	{
		sp_conn->Send(p_send_req->GetSendPacket(), p_caller, p_send_req->GetCallID());
	}else
	{
		//操作出错，生产NetResponse
		RecvNetRes * p_recv_res = new RecvNetRes(sockfd, p_send_req->GetSendPacket());
		p_recv_res->SetResult(false, 0, PROTL_ERR_NOSOCK); 
		p_recv_res->SetCallID(p_send_req->GetCallID());

		//返回响应类
		Event e(p_recv_res, this);
		p_caller->PostEvent(e);
	}

	
}

void AsyncNetDriver::post_handler()
{

	//读取输入事件
	vector<Event> event_vec;

	//非阻塞读取
	m_input_blkque.TryPopFront(event_vec);

	//读取事件内容，并作响应处理
	for(size_t i=0; i<event_vec.size(); ++i)
	{
		Event & ev = event_vec[i];
		EventData * p_edata = ev._p_edata;

		if(!p_edata)
		{
			lerr << "EventData is null. loop will continue." << endl;
			continue;
		}
		
		const EDType reqtype = p_edata->GetType();

		switch(reqtype)
		{
		case EDTYPE_LISTEN_NET_REQ :
			req_listen(p_edata, ev._p_caller);
			break;
		case EDTYPE_CONN_NET_REQ :
			req_connect(p_edata, ev._p_caller);
			break;
		case EDTYPE_CLOSE_NET_REQ :
			req_close(p_edata, ev._p_caller);
			break;
		case EDTYPE_RECV_NET_REQ :
			req_recv(p_edata, ev._p_caller);
			break;
		case EDTYPE_SEND_NET_REQ :
			req_send(p_edata, ev._p_caller);	
			break;
		default :
			lerr<< "WARNING: can't recognize this NetReqType = " << reqtype << endl;
			break;
		}//switch restype

		//处理完毕 销毁事件内容
		delete p_edata;
	}//for event_vec

	
}

void AsyncNetDriver::on_timer(const boost::system::error_code& error)
{
	//lout << "ON timer : " << boost::this_thread::get_id() << endl;
	m_timer.expires_from_now(boost::posix_time::seconds(5));
    m_timer.async_wait(boost::bind(&AsyncNetDriver::on_timer, this, boost::asio::placeholders::error));  
}

