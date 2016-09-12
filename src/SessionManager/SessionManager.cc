
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

	//ע�������и�˳�����⣬������������淢�ͺ������յ��˻ظ��¼����ظ��¼����̱�������
	//Session������������������棬���Ǵ�ʱ�����������δ������ϣ��䴦���̻߳�δ���ɣ���
	//������Ȼû�����⣬������Ϊ��ʱ���������Ϊ��������Ѿ�ʵ����������Ļ������׳�һ���쳣����

	//������������
	const CallID cid = generate_call_id(); //����һ��callid
	//cout << "cqw debug: listening at " << m_listen_ip.c_str() << ":" << m_listen_port << endl;
	int lissock = m_net_driver.Listen(m_listen_ip.c_str(), m_listen_port, this, cid);
	//ldbg1 << "Listen cid = " << cid << endl;

	if(lissock == -1)
	{
		lwrn << "Listen net driver failed." << endl;
		release_call_id(cid);
		return false;
	}

	//��������session
	CSession lis_session(cid, lissock);
	lis_session.SetState(SSTATE_LISTEN);
	
	//��ӵ�sessionmap��
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

	//���CallID ���¼�����
	const CallID cid = e._p_edata->GetCallID();
	const EDType e_type = e._p_edata->GetType();

	//lout << "Get a new Event : cid = " << cid << " e_type = " << e_type << endl;

	//��session���ҳ���Ӧ��session
	session_map_t::iterator iter = m_session_map.find(cid);

	if( iter == m_session_map.end() )
	{
		on_no_session_event(e);
		return;
	}

	//���session
	CSession & session = iter->second;

	//�������͵�����Ӧ�Ĵ�����
	switch(e_type)
	{

	//�����¼� BEG
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
	//�����¼� END

	//�����¼� 
	default:
		on_proc_event(session, e._p_edata);
	}


	//����EventData
	delete e._p_edata;

}

void SessionManager::on_no_session_event(Event & e)
{
	//����û���ҵ�session���¼�
	//Ĭ�ϴ����Ƕ���
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

	//����ת��
	ListenNetRes * p_listen_res = dynamic_cast<ListenNetRes*>(p_edata);

	assert(p_listen_res);

	//�жϽ���Ƿ���ȷ
	if( !p_listen_res->GetRes())
	{
		lerr << "Listen Failed." << endl;
		return -1;
	}

	//�����µ�����
	const CallID cid = EventEngine::generate_call_id();
	int clifd = p_listen_res->GetClientSock();
	//lout<< " get a new sock = " << clifd << endl;

	//lout << "New Socket cid = " << cid << endl;

	//��SessionMap�в���
	pair<session_map_t::iterator, bool> res = m_session_map.insert(make_pair(cid, CSession(cid, clifd)));

	if(res.second == false)
	{
		//�����Ӵ���  �����ܳ����������
		lerr << "ERROR: the client fd is already in the session map. cid = " << clifd << endl;
		lerr << "                                      clisession._state = " << res.first->second.GetState() << endl;
		return -1;
	}

	//����½��� ���û�Session
	CSession & clisession = res.first->second;
	//lout << "New CSession cid = " << clisession.GetCallID() << endl;

	//׼�����û�Session�н���NetPacket
	clisession.SetState(SSTATE_RECVING);
	clisession._recv_packet.Clear();
	m_net_driver.Receive(clifd, &clisession._recv_packet, this, clisession.GetCallID());

	return clifd;
	

}

void SessionManager::on_recv_packet(CSession & session, EventData * p_edata)
{
	assert(p_edata);

	//����ת��
	RecvNetRes * p_recv_res = dynamic_cast<RecvNetRes*>(p_edata);

	if(!p_recv_res)
	{
		lerr << "Dynamic_cast EventData to RecvNetRes failed." << endl;
		return;
	}

	//�б�sockfd�Ƿ����
	if(session.GetSock() != p_recv_res->GetSrcSock())
	{
		lerr << "Session's sockfd is not ths same as RecvNetRes's sockfd. session sockfd = " << session.GetSock() << endl;
		lerr << "                                                      RecvNetRes sockfd = " << p_recv_res->GetSrcSock() << endl;
		return;
	}

	//�б�ǰsession�Ƿ��ǽ���״̬
	if(session.GetState() != SSTATE_RECVING)
	{
		lerr << "Session's state is not recving. CurrState = " << session.GetState() << endl;
		return;
	}

	//�����ս��
	if(!p_recv_res->GetRes())
	{
		lerr << "Net driver recv return error. socket error = " << p_recv_res->GetSockErrno() << endl;
		lerr << "                               protl error = " << p_recv_res->GetProtlErrno() << endl;

		//�ر�socket, �ر���ɺ���on_close()����������session
		session.SetState(SSTATE_CLOSING);
		m_net_driver.Shutdown(session.GetSock(), this, session.GetCallID());
		return;
	}

	//���ճɹ� ������ս��
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


	//��ʼҵ������
	start_process(session);

}

void SessionManager::on_send_packet(CSession & session, EventData * p_edata)
{
	assert(p_edata);

	//����ת��
	SendNetRes * p_send_res = dynamic_cast<SendNetRes*>(p_edata);

	if(!p_send_res)
	{
		lerr << "Dynamic_cast EventData to SendNetRes failed." << endl;
		return;
	}

	//�б�sockfd�Ƿ����
	if(session.GetSock() != p_send_res->GetSrcSock())
	{
		lerr << "Session's sockfd is not ths same as SendNetRes's sockfd. session sockfd = " << session.GetSock() << endl;
		lerr << "                                                      RecvNetRes sockfd = " << p_send_res->GetSrcSock() << endl;
		return;
	}

	//�б�ǰsession�Ƿ��Ƿ���״̬
	if(session.GetState() != SSTATE_SENDING)
	{
		lerr << "Session's state is not sending. CurrState = " << session.GetState() << endl;
		return;
	}

	//��鷢�ͽ��
	if(!p_send_res->GetRes())
	{
		lerr << "Net driver send return error. socket error = " << p_send_res->GetSockErrno() << endl;
		lerr << "                               protl error = " << p_send_res->GetProtlErrno() << endl;
	}

	//�������
	//�ر�socket, �ر���ɺ���on_close()����������session
	session.SetState(SSTATE_CLOSING);
	m_net_driver.Shutdown(session.GetSock(), this, session.GetCallID());
}

void SessionManager::on_close_sock(CSession & session, EventData * p_edata)
{
	assert(p_edata);

	//����ת��
	CloseNetRes * p_close_res = dynamic_cast<CloseNetRes*>(p_edata);

	if(!p_close_res)
	{
		lerr << "Dynamic_cast EventData to CloseNetRes failed." << endl;
		return;
	}

	//�б�sockfd�Ƿ����
	if(session.GetSock() != p_close_res->GetSrcSock())
	{
		lerr << "Session's sockfd is not ths same as CloseNetRes's sockfd. session sockfd = " << session.GetSock() << endl;
		lerr << "                                                       RecvNetRes sockfd = " << p_close_res->GetSrcSock() << endl;
		return;
	}

	//�б�ǰsession�Ƿ��ǹر���״̬
	if(session.GetState() != SSTATE_CLOSING)
	{
		lerr << "Session's state is not closing. CurrState = " << session.GetState() << endl;
		return;
	}

	//�����ս��
	if(!p_close_res->GetRes())
	{
		lerr << "Net driver close return error. socket error = " << p_close_res->GetSockErrno() << endl;
		lerr << "                                protl error = " << p_close_res->GetProtlErrno() << endl;
	}

	//����Session
	EventEngine::release_call_id(session.GetCallID());  //����callid
	m_session_map.erase(session.GetCallID());
}

void SessionManager::start_process(CSession & session)
{
	//����session״̬
	session.SetState(SSTATE_PROCESS);

	//����MSG-TYPE���ɶ�Ӧprocess
	NetPacket & packet = session._recv_packet;
	if(!packet.IsGood())
	{
		lerr << "The session's recv packet is not recv good. session call id is : " << session.GetCallID() << endl;
		return;
	}
	
	session._p_process = create_process(session);
	if( !session._p_process )
	{
		//packet���ʹ������ô�����
		session._send_packet.Clear();
		string type;
		session._recv_packet.GetMsgType(type);
		string reBody = "<Msg><ResCode>";
		reBody += num_2_str(ERR_USR_UNKNOW_MSG_TYPE);
		reBody += "</ResCode><Content /></Msg>";
		session._send_packet.Write(HEAD_RESPONSE, type, reBody);

		//������������
		end_process(session);
		return;
	}

	//���̴���ʼ
	ProcessRes res = session._p_process->Begin();

	if(PROCESS_FINISH == res)
	{
		
		end_process(session);
	}

}

void SessionManager::end_process(CSession & session)
{
	//����process
	delete session._p_process;
	session._p_process = NULL;

	session.SetState(SSTATE_SENDING);
	m_net_driver.Send(session.GetSock(), &session._send_packet, this, session.GetCallID());
}

void SessionManager::on_proc_event(CSession & session, EventData * p_edata)
{
	assert(p_edata);
	
	//����CProcess����
	CProcess * p_proc = session._p_process;

	if(!p_proc)
	{
		
		lerr << "session's process is null. callid = " << session.GetCallID() << endl;
		lerr << "                     SessionState = " << session.GetState() << endl;
		lerr << "                        EventType = " << p_edata->GetType() << endl;

		return;
	}

	//ִ�д���
	ProcessRes res = p_proc->Work(p_edata);

	//�ж��Ƿ����̽���  �����������end_process
	if( PROCESS_FINISH == res )
	{
		//���̽���
		end_process(session);
	}

}

CProcess * SessionManager::create_process(CSession & session)
{
	//����ֱ�ӷ���Ĭ�ϴ���Ĭ��Ϊecho����
	return new CProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
}

