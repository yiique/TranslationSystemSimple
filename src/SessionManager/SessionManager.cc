
#include "SessionManager.h"
#include <assert.h>


SessionManager::SessionManager(AsyncNetDriver * p_net_driver):  m_listen_ip(""),
																m_listen_port(0), 
																m_net_driver(*p_net_driver)
																 
{
}

SessionManager::~SessionManager(void)
{
}

bool SessionManager::SetListen(const char * ip, const int port)
{
	if( !ip || port <=0 || port >=65535 )
	{
		lerr << "ERROR: Input ip or port is illegal." << endl;
		return false;
	}

	m_listen_ip = ip;
	m_listen_port = port;

	return true;
}

bool SessionManager::Start(void)
{
	//lout << " start." << endl;

	//注：这里有个顺序问题，假设给网络引擎发送后，立刻收到了回复事件，回复事件立刻被放入了
	//Session管理器的输入队列里面，但是此时输入管理器并未启动完毕（其处理线程还未生成），
	//但是仍然没有问题，不会因为此时序崩溃，因为输入队列已经实例化（错误的话它会抛出一个异常）。

	//发送侦听请求
	const CallID cid = generate_call_id(); //产生一个callid
	//cout << "cqw debug: listening at " << m_listen_ip.c_str() << ":" << m_listen_port << endl;
	int lissock = m_net_driver.Listen(m_listen_ip.c_str(), m_listen_port, this, cid);
	//ldbg1 << "Listen cid = " << cid << endl;

	if(lissock == -1)
	{
		lwrn << "Listen net driver failed." << endl;
		release_call_id(cid);
		return false;
	}

	//建立侦听session
	CSession lis_session(cid, lissock);
	lis_session.SetState(SSTATE_LISTEN);
	
	//添加到sessionmap中
	m_session_map.clear();
	m_session_map.insert(make_pair(cid, lis_session));

	return EventEngine::Start();
}


void SessionManager::on_event(Event & e)
{
	if(!e._p_edata)
	{
		lerr << "Event's data is null." << endl;
		return;
	}

	//获得CallID 和事件类型
	const CallID cid = e._p_edata->GetCallID();
	const EDType e_type = e._p_edata->GetType();

	//lout << "Get a new Event : cid = " << cid << " e_type = " << e_type << endl;

	//在session中找出对应的session
	session_map_t::iterator iter = m_session_map.find(cid);

	if( iter == m_session_map.end() )
	{
		on_no_session_event(e);
		return;
	}

	//获得session
	CSession & session = iter->second;

	//根据类型调用响应的处理函数
	switch(e_type)
	{

	//网络事件 BEG
	case EDTYPE_LISTEN_NET_RES:
		on_listen_newsock(session, e._p_edata);
		break;
	case EDTYPE_CLOSE_NET_RES:
		on_close_sock(session, e._p_edata);
		break;
	case EDTYPE_RECV_NET_RES:
		on_recv_packet(session, e._p_edata);
		break;
	case EDTYPE_SEND_NET_RES:
		on_send_packet(session, e._p_edata);
		break;
	//网络事件 END

	//其他事件 
	default:
		on_proc_event(session, e._p_edata);
	}


	//销毁EventData
	delete e._p_edata;

}

void SessionManager::on_no_session_event(Event & e)
{
	//处理没有找到session的事件
	//默认处理是丢弃
	lwrn << "WARNING: NO SESSION EVENT.  priority = " << e._priority << "   caller = " << (unsigned long) e._p_caller << endl;

	if(!e._p_edata)
	{
		lwrn << "WARNING: NO SESSION EVENT TYPE = " << e._p_edata->GetType() << endl;
		lwrn << "WARNING: NO SESSION EVENT CALLID = " << e._p_edata->GetCallID() << endl;
		delete e._p_edata;
	}else
	{
		lwrn << "WARNING: NO SESSION EVENT DATA IS NULL." << endl;
	}


}


int SessionManager::on_listen_newsock(CSession & session, EventData * p_edata)
{
	assert(p_edata);

	//类型转换
	ListenNetRes * p_listen_res = dynamic_cast<ListenNetRes*>(p_edata);

	assert(p_listen_res);

	//判断结果是否正确
	if( !p_listen_res->GetRes())
	{
		lerr << "Listen Failed." << endl;
		return -1;
	}

	//处理新到连接
	const CallID cid = EventEngine::generate_call_id();
	int clifd = p_listen_res->GetClientSock();
	//lout<< " get a new sock = " << clifd << endl;

	//lout << "New Socket cid = " << cid << endl;

	//在SessionMap中插入
	pair<session_map_t::iterator, bool> res = m_session_map.insert(make_pair(cid, CSession(cid, clifd)));

	if(res.second == false)
	{
		//新连接存在  不可能出现这种情况
		lerr << "ERROR: the client fd is already in the session map. cid = " << clifd << endl;
		lerr << "                                      clisession._state = " << res.first->second.GetState() << endl;
		return -1;
	}

	//获得新建立 的用户Session
	CSession & clisession = res.first->second;
	//lout << "New CSession cid = " << clisession.GetCallID() << endl;

	//准备从用户Session中接收NetPacket
	clisession.SetState(SSTATE_RECVING);
	clisession._recv_packet.Clear();
	m_net_driver.Receive(clifd, &clisession._recv_packet, this, clisession.GetCallID());

	return clifd;
	

}

void SessionManager::on_recv_packet(CSession & session, EventData * p_edata)
{
	assert(p_edata);

	//类型转换
	RecvNetRes * p_recv_res = dynamic_cast<RecvNetRes*>(p_edata);

	if(!p_recv_res)
	{
		lerr << "Dynamic_cast EventData to RecvNetRes failed." << endl;
		return;
	}

	//判别sockfd是否符合
	if(session.GetSock() != p_recv_res->GetSrcSock())
	{
		lerr << "Session's sockfd is not ths same as RecvNetRes's sockfd. session sockfd = " << session.GetSock() << endl;
		lerr << "                                                      RecvNetRes sockfd = " << p_recv_res->GetSrcSock() << endl;
		return;
	}

	//判别当前session是否是接收状态
	if(session.GetState() != SSTATE_RECVING)
	{
		lerr << "Session's state is not recving. CurrState = " << session.GetState() << endl;
		return;
	}

	//检查接收结果
	if(!p_recv_res->GetRes())
	{
		lerr << "Net driver recv return error. socket error = " << p_recv_res->GetSockErrno() << endl;
		lerr << "                               protl error = " << p_recv_res->GetProtlErrno() << endl;

		//关闭socket, 关闭完成后由on_close()来负责清理session
		session.SetState(SSTATE_CLOSING);
		m_net_driver.Shutdown(session.GetSock(), this, session.GetCallID());
		return;
	}

	//接收成功 输出接收结果
	string body;
	string type;
	session._recv_packet.GetMsgType(type);
	session._recv_packet.GetBody(body);
	/*
	lout<< endl;
	lout<< "=========SessionManager recv a NetPacket==========" << endl;
	lout<< "-> socket fd = " << session.GetSock() << endl;
	lout<< "-> NetPacket Type is " << type << endl;
	lout<< "-> NetPacket Body is :" << endl;
	lout<< body << endl;
	lout<< "==================================================" << endl;
	lout<< endl;
	*/


	//开始业务流程
	start_process(session);

}

void SessionManager::on_send_packet(CSession & session, EventData * p_edata)
{
	assert(p_edata);

	//类型转换
	SendNetRes * p_send_res = dynamic_cast<SendNetRes*>(p_edata);

	if(!p_send_res)
	{
		lerr << "Dynamic_cast EventData to SendNetRes failed." << endl;
		return;
	}

	//判别sockfd是否符合
	if(session.GetSock() != p_send_res->GetSrcSock())
	{
		lerr << "Session's sockfd is not ths same as SendNetRes's sockfd. session sockfd = " << session.GetSock() << endl;
		lerr << "                                                      RecvNetRes sockfd = " << p_send_res->GetSrcSock() << endl;
		return;
	}

	//判别当前session是否是发送状态
	if(session.GetState() != SSTATE_SENDING)
	{
		lerr << "Session's state is not sending. CurrState = " << session.GetState() << endl;
		return;
	}

	//检查发送结果
	if(!p_send_res->GetRes())
	{
		lerr << "Net driver send return error. socket error = " << p_send_res->GetSockErrno() << endl;
		lerr << "                               protl error = " << p_send_res->GetProtlErrno() << endl;
	}

	//发送完毕
	//关闭socket, 关闭完成后由on_close()来负责清理session
	session.SetState(SSTATE_CLOSING);
	m_net_driver.Shutdown(session.GetSock(), this, session.GetCallID());
}

void SessionManager::on_close_sock(CSession & session, EventData * p_edata)
{
	assert(p_edata);

	//类型转换
	CloseNetRes * p_close_res = dynamic_cast<CloseNetRes*>(p_edata);

	if(!p_close_res)
	{
		lerr << "Dynamic_cast EventData to CloseNetRes failed." << endl;
		return;
	}

	//判别sockfd是否符合
	if(session.GetSock() != p_close_res->GetSrcSock())
	{
		lerr << "Session's sockfd is not ths same as CloseNetRes's sockfd. session sockfd = " << session.GetSock() << endl;
		lerr << "                                                       RecvNetRes sockfd = " << p_close_res->GetSrcSock() << endl;
		return;
	}

	//判别当前session是否是关闭中状态
	if(session.GetState() != SSTATE_CLOSING)
	{
		lerr << "Session's state is not closing. CurrState = " << session.GetState() << endl;
		return;
	}

	//检查接收结果
	if(!p_close_res->GetRes())
	{
		lerr << "Net driver close return error. socket error = " << p_close_res->GetSockErrno() << endl;
		lerr << "                                protl error = " << p_close_res->GetProtlErrno() << endl;
	}

	//清理Session
	EventEngine::release_call_id(session.GetCallID());  //回收callid
	m_session_map.erase(session.GetCallID());
}

void SessionManager::start_process(CSession & session)
{
	//设置session状态
	session.SetState(SSTATE_PROCESS);

	//根据MSG-TYPE生成对应process
	NetPacket & packet = session._recv_packet;
	if(!packet.IsGood())
	{
		lerr << "The session's recv packet is not recv good. session call id is : " << session.GetCallID() << endl;
		return;
	}
	
	session._p_process = create_process(session);
	if( !session._p_process )
	{
		//packet类型错误，设置错误报文
		session._send_packet.Clear();
		string type;
		session._recv_packet.GetMsgType(type);
		string reBody = "<Msg><ResCode>";
		reBody += num_2_str(ERR_USR_UNKNOW_MSG_TYPE);
		reBody += "</ResCode><Content /></Msg>";
		session._send_packet.Write(HEAD_RESPONSE, type, reBody);

		//结束处理流程
		end_process(session);
		return;
	}

	//流程处理开始
	ProcessRes res = session._p_process->Begin();

	if(PROCESS_FINISH == res)
	{
		
		end_process(session);
	}

}

void SessionManager::end_process(CSession & session)
{
	//销毁process
	delete session._p_process;
	session._p_process = NULL;

	session.SetState(SSTATE_SENDING);
	m_net_driver.Send(session.GetSock(), &session._send_packet, this, session.GetCallID());
}

void SessionManager::on_proc_event(CSession & session, EventData * p_edata)
{
	assert(p_edata);
	
	//交给CProcess处理
	CProcess * p_proc = session._p_process;

	if(!p_proc)
	{
		
		lerr << "session's process is null. callid = " << session.GetCallID() << endl;
		lerr << "                     SessionState = " << session.GetState() << endl;
		lerr << "                        EventType = " << p_edata->GetType() << endl;

		return;
	}

	//执行处理
	ProcessRes res = p_proc->Work(p_edata);

	//判断是否流程结束  如果结束调用end_process
	if( PROCESS_FINISH == res )
	{
		//流程结束
		end_process(session);
	}

}

CProcess * SessionManager::create_process(CSession & session)
{
	//基类直接返回默认处理（默认为echo服务）
	return new CProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
}

