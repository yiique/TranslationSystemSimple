#include "TransController.h"
#include "DataBase/DBOperation.h"

#ifdef ENABLE_SOFT_PROTECT
#include "SoftProtect/SoftProtectHandle.h"
#endif

TransController TransController::ms_rtt_trans_controller(NetInterface::GetSerNet(), TRANS_TYPE_RTT); 
TransController TransController::ms_olt_trans_controller(NetInterface::GetSerNet(), TRANS_TYPE_OLT); 

TransController * TransController::GetRttInstance()
{
	return &ms_rtt_trans_controller;
}

TransController * TransController::GetOltInstance()
{
	return &ms_olt_trans_controller;
}


TransController::TransController(AsyncNetDriver * p_net_driver, const TransType trans_type): m_net_driver(*p_net_driver) , m_trans_type(trans_type)
{
	m_is_report_state = (TRANS_TYPE_OLT == m_trans_type) ? true : false;

	m_langtype.first = "all";
	m_langtype.second = "chinese";
	m_domaintype.first = "all";
	m_domaintype.second = m_langtype;
}

TransController::~TransController(void)
{
}

bool TransController::Start(const Configurator & config, const int timeout_sec)
{
    m_config = config;
	m_connect_ip = config._local_ip;

	if(timeout_sec < 0)
	{
		lerr << "Input timeout sec can't be less than zero." << endl;
		return false;
	}

	m_timeout_sec = timeout_sec;
	
	//直接生成对应领域 
	if(!init_trans_cores())
	{
		lerr << "ERROR: init_trans_cores failed." << endl;
		return false;
	}


	if( !EventEngine::Start() )
		return false;

	lout << " TransController this_ptr = " << (unsigned long) this << endl;

	return true;
}

bool TransController::init_trans_cores()
{

    for(set<DomainType>::iterator iter = m_config._domain.begin(); iter != m_config._domain.end(); ++iter)
    {   
		lout << "TransController::init_trans_cores " << (DomainType)(*iter) << endl;
        m_trans_core_map.insert(make_pair(*iter, trans_core_share_ptr(new TransCore(*iter, m_is_report_state, m_timeout_sec))));
    }

	return true;
}

void TransController::on_event(Event & e)
{

	if(!e._p_edata)
	{
		lerr << "Event's data is null. Ignore this event." << endl;
		return ;
	}

	const EDType e_type = e._p_edata->GetType();

	switch(e_type)
	{

	//网络事件 BEG
	case EDTYPE_LISTEN_NET_RES:
		on_net(e);
		break;
	case EDTYPE_CONN_NET_RES:
		on_net(e);
		break;
	case EDTYPE_CLOSE_NET_RES:
		on_net(e);
		break;
	case EDTYPE_RECV_NET_RES:
		on_net(e);
		break;
	case EDTYPE_SEND_NET_RES:
		on_net(e);
		break;
	case EDTYPE_UNDEF_NET_RES:
		on_net(e);
		break;
	//网络事件 END

	//心跳包
	case EDTYPE_SLAVE_PULSE_REQ:
		on_slave_pluse(e);
		break;

	//翻译任务事件
	case EDTYPE_TRANS_CONTROL_SUBMIT_REQ:
		on_trans_submit(e);
		break;
	case EDTYPE_TRANS_CONTROL_SUSPEND_REQ:
		on_trans_suspend(e);
		break;
	case EDTYPE_TRANS_CONTROL_RERUN_REQ:
		on_trans_rerun(e);
		break;
	case EDTYPE_TRANS_CONTROL_CANCEL_REQ:
		on_trans_cancel(e);
		break;
	case EDTYPE_GET_TRANS_SENT_REQ:
		on_get_trans_sent(e);
		break;

	//定时器事件
	case EDTYPE_TIMER:
		on_timer(e);
		break;
	
	//其他事件 
	default:
		lerr << "Recv a unknow type event , type = " << e_type << endl;
	}

	//销毁EventData
	delete e._p_edata;
}

void TransController::on_slave_pluse(Event & e)
{

	SlavePulseReq *p_req = dynamic_cast<SlavePulseReq*>(e._p_edata);

	assert(p_req && e._p_caller);

	//根据领域查找所属的TransCore
	const TransSlaveInfo & slave_info = p_req->GetSlaveInfo();
	int ret = SUCCESS;

	//lout << "TransController::on_slave_pluse()" << slave_info.domain << endl;
	trans_core_map_t::iterator iter = m_trans_core_map.find(slave_info.domain);

	if(m_trans_core_map.end() != iter)
	{
		m_domaintype = slave_info.domain;

		trans_core_share_ptr sp_trans_core = iter->second;

		sp_trans_core->PluseSlave(slave_info);

		vector<trans_task_share_ptr> task_vec = sp_trans_core->GetReadyTask();
		
		call_slave_for_trans(task_vec);

	}else
	{
		lout << "TransController::on_slave_pluse() ERR:" << slave_info.domain << endl;
		ret = ERR_TRANS_CONTROL_DOMAIN_NOT_FIND;
	}

	//生成响应
	SlavePulseRes *p_res = new SlavePulseRes(p_req->GetCallID());
	p_res->SetResult(ret);

	Event re(p_res, this);
	e._p_caller->PostEvent(re);
}


void TransController::on_trans_submit(Event & e)
{
	TransSubmitReq *p_req = dynamic_cast<TransSubmitReq*>(e._p_edata);
	assert(p_req);

	//获得text
	TransText * p_trans_text = p_req->GetTransText();

	assert(p_trans_text);

	//lout << "Submit text id = " << p_trans_text->GetID() << " , domain = " << p_trans_text->GetDomain() << endl;

	int ret = SUCCESS;

	lout << "TransController::on_trans_submit()" << p_trans_text->GetDomain() << endl;
	//trans_core_map_t::iterator iter = m_trans_core_map.find(p_trans_text->GetDomain());
	trans_core_map_t::iterator iter = m_trans_core_map.find(m_domaintype);

	if(m_trans_core_map.end() != iter)
	{
		trans_core_share_ptr sp_trans_core = iter->second;

		if(false == sp_trans_core->CheckAlive())
		{
			ret = ERR_DOMAIN_NO_ALIVE; 
		}else
		{
			ret = sp_trans_core->SubmitTranslation(p_trans_text);

			if(SUCCESS == ret)
			{
				if(m_is_report_state)
				{
					DBOperation::ReportTransStateAndPercent(p_trans_text->GetID(), TRANS_STATE_RUNNING, 0);
				}

				vector<trans_task_share_ptr> task_vec = sp_trans_core->GetReadyTask();

				//lout << "Read task number = " << task_vec.size() << endl;

				call_slave_for_trans(task_vec);

			}
		}

	}else
	{
		lout << "TransController::on_trans_submit() ERR:" << p_trans_text->GetDomain() << endl;
		ret = ERR_TRANS_CONTROL_DOMAIN_NOT_FIND;
	}
	

	if(SUCCESS != ret)
	{
		finish_translation(ret, p_trans_text);
	}
}

void TransController::on_trans_suspend(Event & e)
{
	TransSuspendReq *p_req = dynamic_cast<TransSuspendReq*>(e._p_edata);

	assert(p_req && e._p_caller);

	//根据领域查找所属的TransCore
	int ret = SUCCESS;
	lout << "TransController::on_trans_suspend()" << p_req->GetDomain() << endl;
	//trans_core_map_t::iterator iter = m_trans_core_map.find(p_req->GetDomain());
	trans_core_map_t::iterator iter = m_trans_core_map.find(m_domaintype);

	if(m_trans_core_map.end() != iter)
	{
		trans_core_share_ptr sp_trans_core = iter->second;

		ret = sp_trans_core->SuspendTranslation(p_req->GetUsrID(), p_req->GetTextID());

		if(SUCCESS == ret)
		{
			if(m_is_report_state)
			{
				DBOperation::ReportTransState(p_req->GetTextID(), TRANS_STATE_SUSPEND);
			}

			vector<trans_task_share_ptr> task_vec = sp_trans_core->GetReadyTask();

			call_slave_for_trans(task_vec);
		}

	}else
	{
		lout << "TransController::on_trans_suspend() ERR:" << p_req->GetDomain() << endl;
		ret = ERR_TRANS_CONTROL_DOMAIN_NOT_FIND;
	}

	//生成响应
	TransSuspendRes *p_res = new TransSuspendRes(p_req->GetCallID());
	p_res->SetResult(ret);

	Event re(p_res, this);
	e._p_caller->PostEvent(re);
}

void TransController::on_trans_rerun(Event & e)
{
	TransRerunReq *p_req = dynamic_cast<TransRerunReq*>(e._p_edata);

	assert(p_req && e._p_caller);

	//根据领域查找所属的TransCore
	int ret = SUCCESS;
	lout << "TransController::on_trans_rerun()" << p_req->GetDomain() << endl;
	//trans_core_map_t::iterator iter = m_trans_core_map.find(p_req->GetDomain());
	trans_core_map_t::iterator iter = m_trans_core_map.find(m_domaintype);

	if(m_trans_core_map.end() != iter)
	{
		trans_core_share_ptr sp_trans_core = iter->second;

		ret = sp_trans_core->RerunTranslation(p_req->GetUsrID(), p_req->GetTextID());

		if(SUCCESS == ret)
		{
			if(m_is_report_state)
			{
				DBOperation::ReportTransState(p_req->GetTextID(), TRANS_STATE_RUNNING);
			}

			vector<trans_task_share_ptr> task_vec = sp_trans_core->GetReadyTask();

			call_slave_for_trans(task_vec);
		}

	}else
	{
		lout << "TransController::on_trans_rerun() ERR:" << p_req->GetDomain() << endl;
		ret = ERR_TRANS_CONTROL_DOMAIN_NOT_FIND;
	}

	//生成响应
	TransRerunRes *p_res = new TransRerunRes(p_req->GetCallID());
	p_res->SetResult(ret);

	Event re(p_res, this);
	e._p_caller->PostEvent(re);
}


void TransController::on_get_trans_sent(Event & e)
{
	GetTransSentReq *p_req = dynamic_cast<GetTransSentReq*>(e._p_edata);
	assert(p_req && e._p_caller);

	GetTransSentRes * p_res = new GetTransSentRes(p_req->GetCallID());

	//根据领域查找所属的TransCore
	lout << "TransController::on_get_trans_sent()" << p_req->GetDomain() << endl;
	//trans_core_map_t::iterator iter = m_trans_core_map.find(p_req->GetDomain());
	trans_core_map_t::iterator iter = m_trans_core_map.find(m_domaintype);

	if(m_trans_core_map.end() != iter)
	{
		trans_core_share_ptr sp_trans_core = iter->second;

		TransText * p_text = sp_trans_core->GetTransTextCopy(p_req->GetUsrID(), p_req->GetTextID());

		if(p_text)
		{
			p_res->is_exist = true;
			p_res->total_sent = p_text->GetTransSize();

			p_text->GetFinishSent(p_req->Offset(), p_res->sent_vec);


		}else
		{
			p_res->is_exist = false;
		}

	}else
	{
		p_res->is_exist = false;
	}

	//生成响应
	Event re(p_res, this);
	e._p_caller->PostEvent(re);
}

void TransController::on_trans_cancel(Event & e)
{
	TransCancelReq *p_req = dynamic_cast<TransCancelReq*>(e._p_edata);

	assert(p_req && e._p_caller);

	//根据领域查找所属的TransCore
	int ret = SUCCESS;
	lout << "TransController::on_trans_cancel()" << p_req->GetDomain() << endl;
	//trans_core_map_t::iterator iter = m_trans_core_map.find(p_req->GetDomain());
	trans_core_map_t::iterator iter = m_trans_core_map.find(m_domaintype);

	if(m_trans_core_map.end() != iter)
	{
		trans_core_share_ptr sp_trans_core = iter->second;

		pair<int, TransText*> res = sp_trans_core->CancelTranslation(p_req->GetUsrID(), p_req->GetTextID());

		ret = res.first;

		if(SUCCESS == ret)
		{
			if(m_is_report_state)
			{
				DBOperation::ReportTransState(p_req->GetTextID(), TRANS_STATE_CANCEL);
			}

			finish_translation(ERR_TRANS_CONTROL_USR_CANCEL, res.second);

			vector<trans_task_share_ptr> task_vec = sp_trans_core->GetReadyTask();

			call_slave_for_trans(task_vec);
		}

	}else
	{
		lout << "TransController::on_trans_cancel() ERR:" << p_req->GetDomain() << endl;
		ret = ERR_TRANS_CONTROL_DOMAIN_NOT_FIND;
	}

	//生成响应
	TransCancelRes *p_res = new TransCancelRes(p_req->GetCallID());
	p_res->SetResult(ret);

	Event re(p_res, this);
	e._p_caller->PostEvent(re);
}

void TransController::call_slave_for_trans(const vector<trans_task_share_ptr> & task_ptr_vec)
{
	for(size_t i=0; i<task_ptr_vec.size(); ++i)
	{
		trans_task_share_ptr sp_task = task_ptr_vec[i];

#ifdef ENABLE_SOFT_PROTECT
		
		if( 0 != softprotect::SoftProtectHandle::AddUseCnt(1))
		{
			handle_max_use_cnt(sp_task);
			continue;
		}
#endif //ENABLE_SOFT_PROTECT

		//向网络模块发起连接请求
		const CallID cid = this->generate_call_id(); //生成call id
		int sockfd = -1;
		
		while(true) //TODO 如果connect失败，必然是本地错误，等待若干秒重试
		{
			sockfd = m_net_driver.Connect(sp_task->sp_slave->GetInfo().ip.c_str(), 
										  sp_task->sp_slave->GetInfo().port,
										  m_connect_ip.c_str(), 
										  this, 
										  cid);

			if(sockfd < 0)
			{
				//等待若干秒后重试 //TODO
				lwrn << "Net Connect retrun error. Wait and retry. idx = " << i << endl;
				boost::this_thread::sleep(boost::posix_time::seconds(3));
				continue;
			}else
			{
				//本地连接成功
				break;
			}
		} // END WHILE

		//建立TransSession
		pair<trans_sess_map_t::iterator, bool> res = m_trans_sess_map.insert( make_pair(cid, TransSession(cid, sockfd, sp_task) ) );

		if(!res.second)
		{
			lerr << "ERROR: Session already in the map. idx = " << i << endl;
			continue;
		}

		//加入成功
		res.first->second.SetState(SSTATE_CONNECT);  //设置为连接状态

	} // END TASK_PTR_VEC FOR
}


void TransController::on_net(Event & e)
{
	if(!e._p_edata)
	{
		lerr << "Event's data is null." << endl;
		return;
	}

	//获得网络事件类型与callid
	const CallID cid = e._p_edata->GetCallID();
	const EDType e_type = e._p_edata->GetType();

	//lout << " Get A new event. call_id = " << cid << " e_type = " << e_type << endl;


	//在翻译session表中找出对应的session
	trans_sess_map_t::iterator iter = m_trans_sess_map.find(cid);

	if( iter == m_trans_sess_map.end() )
	{
		//没有找到
		lerr << "Can't find callid in session_map. call_id = " << cid << endl;
		lerr << "                               event_type = " << e_type << endl;
		
		return;
	}


	//获得当前session
	TransSession & session = iter->second;

	//ldbg1 << "CurrTransSession.state = " << session.GetState() << endl;


	//执行处理
	if( session.GetState() == SSTATE_CONNECT && e_type == EDTYPE_CONN_NET_RES )
	{
		//转换为ConnNetRes
		ConnNetRes * p_conn_res = dynamic_cast<ConnNetRes*>(e._p_edata);
		if(!p_conn_res)
		{
			lerr << "ERROR:Convert to ConnNetRes Failed." << endl;
			return;
		}

		//执行操作
		on_connected(iter, p_conn_res);

	}else if(session.GetState() == SSTATE_RECVING && e_type == EDTYPE_RECV_NET_RES)
	{
		//转换为RecvNetRes
		RecvNetRes * p_recv_res = dynamic_cast<RecvNetRes*>(e._p_edata);
		if(!p_recv_res)
		{
			lerr << "ERROR: Convert to RecvNetRes Failed." << endl;
			return;
		}

		//执行操作
		on_recv_packet(iter, p_recv_res);

	}else if(session.GetState() == SSTATE_SENDING && e_type == EDTYPE_SEND_NET_RES)
	{
		//转换为RecvNetRes
		SendNetRes * p_send_res = dynamic_cast<SendNetRes*>(e._p_edata);
		if(!p_send_res)
		{
			lerr << "ERROR: Convert to SendNetRes Failed." << endl;
			return;
		}

		//执行操作
		on_send_packet(iter, p_send_res);

	}else if(session.GetState() == SSTATE_CLOSING && e_type == EDTYPE_CLOSE_NET_RES)
	{
		//转换为RecvNetRes
		CloseNetRes * p_close_res = dynamic_cast<CloseNetRes*>(e._p_edata);
		if(!p_close_res)
		{
			lerr << "ERROR: Convert to CloseNetRes Failed." << endl;
			return;
		}

		//执行操作
		on_close_sock(iter, p_close_res);

	}else
	{
		lerr << "ERROR: Net res's type is not match session'state sockfd = " << session.GetSock() << endl;
		lerr << "                                             NetResType = " << e_type << endl;
		lerr << "                                            sessionstat = " << session.GetState() << endl;
		
		return;
	}
}

void TransController::on_connected(trans_sess_map_t::iterator iter, ConnNetRes * p_conn_res)
{
	//lout<< "->> TransController::on_connected() start." << endl;
	TransSession & session = iter->second;
	#ifdef OUTPUT_TRANS_TIME
	session.SetTransEndTime();
	#endif

	assert(p_conn_res);
	

	if(!p_conn_res->GetRes())
	{
		//连接失败
		lerr<< "TransController::on_connected() connect is failed. sockfd = " << p_conn_res->GetSrcSock() << endl;
		lerr<< "                                                       ip = " << session.GetServerIP() << endl;
		lerr<< "                                                     port = " << session.GetServerPort() << endl;
		lerr<< "                                               sock_error = " <<p_conn_res->GetSockErrno() << endl;

		//调用翻译错误处理函数，删除session，回收TransSrc，踢出翻译服务器，检查是否有新的翻译服务器可用，否则放回TransCell
		on_session_failed(iter);

		return;
	}

	//连接成功
	session.SetState(SSTATE_SENDING);
	//构造报文
	if( !session.BuildSendPacket())
	{
		//构造报文失败
		lerr<< "TransController::on_connected() build send packet failed. sockfd = " << p_conn_res->GetSrcSock() << endl;
		lerr<< "                                                              ip = " << session.GetServerIP() << endl;
		lerr<< "                                                            port = " << session.GetServerPort() << endl;
	
		//调用翻译错误处理函数，删除session，回收TransSrc，踢出翻译服务器，检查是否有新的翻译服务器可用，否则放回TransCell
		on_session_failed(iter);

		return;
		
	}

	//执行发送
	m_net_driver.Send(session.GetSock(), &(session._send_packet), this, session.GetCallID());
}

void TransController::on_recv_packet(trans_sess_map_t::iterator iter, RecvNetRes * p_recv_res)
{
	//lout<< "--> TransController::on_recv_packet() start." << endl;
	TransSession & session = iter->second;

	assert(p_recv_res);

	if(!p_recv_res->GetRes())
	{
		//接收失败
		lerr<< "TransController::on_recv_packet() recv is failed. sockfd = " << p_recv_res->GetSrcSock() << endl;
		lerr<< "                                                       ip = " << session.GetServerIP() << endl;
		lerr<< "                                                     port = " << session.GetServerPort() << endl;
		lerr<< "                                             _protl_errno = " << p_recv_res->GetProtlErrno() << endl;
		lerr<< "                                              _sock_errno = " << p_recv_res->GetSockErrno() << endl;

		//调用翻译错误处理函数，删除session，回收TransSrc，踢出翻译服务器，检查是否有新的翻译服务器可用，否则放回TransCell
		on_session_failed(iter);

		return;
	}

	//接收成功
	if(!session.ParseRecvPacekt())
	{
		//解析报文失败
		lerr << " parse recv packet is failed. sockfd = " << p_recv_res->GetSrcSock() << endl;
		lerr << "                                  ip = " << session.GetServerIP() << endl;
		lerr << "                                port = " << session.GetServerPort() << endl;
		lerr << "                            _rescode = " << session.GetServerRescode() << endl;

		//调用翻译错误处理函数，删除session，回收TransSrc，踢出翻译服务器，检查是否有新的翻译服务器可用，否则放回TransCell
		on_session_failed(iter);

		return;
	}

	//执行关闭
	on_session_success(iter);

}


void TransController::on_send_packet(trans_sess_map_t::iterator iter, SendNetRes * p_send_res)
{
	//lout<< "--> TransController::on_send_packet() start ." << endl;
	TransSession & session = iter->second;

	assert(p_send_res);

	if(!p_send_res->GetRes())
	{
		//发送失败
		lerr<< "TransController::on_send_packet() send is failed. sockfd = " << p_send_res->GetSrcSock() << endl;
		lerr<< "                                                       ip = " << session.GetServerIP() << endl;
		lerr<< "                                                     port = " << session.GetServerPort() << endl;
		lerr<< "                                             _protl_errno = " << p_send_res->GetProtlErrno() << endl;
		lerr<< "                                              _sock_errno = " << p_send_res->GetSockErrno() << endl;

		//调用翻译错误处理函数，删除session，回收TransSrc，踢出翻译服务器，检查是否有新的翻译服务器可用，否则放回TransCell
		on_session_failed(iter);

		return;
	}

	//发送成功
	session.SetState(SSTATE_RECVING);
	session._recv_packet.Clear();

	//执行接收
	m_net_driver.Receive(session.GetSock(), &(session._recv_packet), this, session.GetCallID());
	//lout << " start receive." << endl;

}

void TransController::on_close_sock(trans_sess_map_t::iterator iter, CloseNetRes * p_close_res)
{
	//lout<< "--> TransController::on_close_sock() start ." << endl;
	//删除此session
	//不需要自行关闭socket， 由netdriver关闭
	m_trans_sess_map.erase(iter);
	
}

void TransController::on_session_success(trans_sess_map_t::iterator iter)
{
	TransSession & session = iter->second;
	#ifdef OUTPUT_TRANS_TIME
	session.SetTransEndTime();
	#endif

	trans_task_share_ptr sp_task = session.GetTask();

	TransText * p_text = sp_task->sp_trans_core->SuccessTask(sp_task);

	if(p_text)
	{
		finish_translation(SUCCESS, p_text);
	}

	vector<trans_task_share_ptr> ready_task_vec = sp_task->sp_trans_core->GetReadyTask();

	call_slave_for_trans(ready_task_vec);

	//完毕 关闭socket session的删除由on_close执行
	//设置session状态
	session.SetState(SSTATE_CLOSING);
	m_net_driver.Shutdown(session.GetSock(), this, session.GetCallID());
}

void TransController::on_session_failed(trans_sess_map_t::iterator iter)
{
	TransSession & session = iter->second;

	trans_task_share_ptr sp_task = session.GetTask();

	sp_task->sp_trans_core->FailedTask(sp_task);

	vector<trans_task_share_ptr> ready_task_vec = sp_task->sp_trans_core->GetReadyTask();

	call_slave_for_trans(ready_task_vec);

	//完毕 关闭socket session的删除由on_close执行
	//设置session状态
	session.SetState(SSTATE_CLOSING);
	m_net_driver.Shutdown(session.GetSock(), this, session.GetCallID());
}

void TransController::finish_translation(const int res, TransText * p_trans_text)
{
	int ret = ( res == SUCCESS || res == ERR_NO_NEED_TRANS ) ? SUCCESS : res;

	assert(p_trans_text);


	/*if(SUCCESS == ret)
	{
		p_trans_text->PrintDetail();
	}*/

	TransResult * p_trans_res = new TransResult(ret, p_trans_text);
	Event e(p_trans_res, this); 

	if( TRANS_TYPE_RTT == m_trans_type )
	{
		
		//发送给用户Session管理器
		UsrSessManager::GetInstance()->PostEvent(e);
		
	}else if( TRANS_TYPE_OLT == m_trans_type)
	{
		//发送给OltTaskManager
		OltTaskManager::GetInstance()->PostEvent(e);

	}else
	{
		delete p_trans_text;
		lerr << "ERROR: TransController::send_back_finish_text() m_trans_type is illegal. = " << m_trans_type << endl;
	}

	return;
}

void TransController::PostTransSubmit(TransText * p_trans_text)
{
	assert(p_trans_text);

	TransSubmitReq * p_req = new TransSubmitReq(p_trans_text);
	Event e(p_req, NULL);
	this->PostEvent(e);
}

void TransController::PostSlavePluse(const TransSlaveInfo & slave_info, const CallID & cid, EventEngine * p_caller)
{
	assert(p_caller);

	SlavePulseReq * p_req = new SlavePulseReq(cid, slave_info);
	Event e(p_req, p_caller);
	this->PostEvent(e);
}


void TransController::on_timer(Event & e)
{
	TimeEvent * p_time_event = dynamic_cast<TimeEvent*>(e._p_edata);

	if(!p_time_event)
	{
		lerr << "ERROR: Convert to TimeEvent Failed." << endl;
		return;
	}

	//检查各个领域超时
	trans_core_map_t::iterator iter = m_trans_core_map.begin();

	for(; iter != m_trans_core_map.end(); ++iter)
	{
		iter->second->CheckSlaveTimeout();
	}

	
	return;
}

void TransController::PostTransSuspend(const DomainType & domain, const UsrID & usr_id, const TextID & text_id, const CallID & cid, EventEngine * p_caller)
{
	TransSuspendReq * p_req = new TransSuspendReq(cid, domain, usr_id, text_id);

	Event e(p_req, p_caller);
	this->PostEvent(e);
}

void TransController::PostTransRerun(const DomainType & domain, const UsrID & usr_id, const TextID & text_id, const CallID & cid, EventEngine * p_caller)
{
	TransRerunReq * p_req = new TransRerunReq(cid, domain, usr_id, text_id);

	Event e(p_req, p_caller);
	this->PostEvent(e);
}

void TransController::PostTransCancel(const DomainType & domain, const UsrID & usr_id, const TextID & text_id, const CallID & cid, EventEngine * p_caller)
{
	TransCancelReq * p_req = new TransCancelReq(cid, domain, usr_id, text_id);

	Event e(p_req, p_caller);
	this->PostEvent(e);
}

void TransController::GetTransSent(const DomainType & domain, 
								   const UsrID & usr_id, 
								   const TextID & text_id, 
								   const size_t offset, 
								   const CallID & cid, 
								   EventEngine * p_caller)
{
	GetTransSentReq * p_req = new GetTransSentReq(cid, domain, usr_id, offset, text_id);

	Event e(p_req, p_caller);
	this->PostEvent(e);
}

#ifdef ENABLE_SOFT_PROTECT
void TransController::handle_max_use_cnt(trans_task_share_ptr sp_task)
{
    sp_task->trans_tgt._decode_result = "License Warning : Permission denied (max use cnt).";
	sp_task->trans_tgt._tgt_str = "License Warning : Permission denied (max use cnt).";
	sp_task->trans_tgt._assist_str = "";
	sp_task->trans_tgt._assist2_str = "";

	TransText * p_text = sp_task->sp_trans_core->SuccessTask(sp_task);

	if(p_text)
	{
		finish_translation(SUCCESS, p_text);
	}

	vector<trans_task_share_ptr> ready_task_vec = sp_task->sp_trans_core->GetReadyTask();

	call_slave_for_trans(ready_task_vec);
}
#endif //ENABLE_SOFT_PROTECT
