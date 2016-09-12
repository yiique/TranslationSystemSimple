
#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#define DEV_DEBUG

//MAIN
#include "EventEngine/EventEngine.h"
#include "NetDriver/AsyncNetDriver.h"
#include "CSession.h"
#include "Log/LogStream.h"

//STL
#include <iostream>
#include <string>
#include <map>
using namespace std;

class SessionManager
	: public EventEngine
{
public:
	SessionManager(AsyncNetDriver * p_net_driver);
	virtual ~SessionManager(void);

	//�����û������˿�
	bool SetListen(const char * ip, const int port);

	//��д����Start����
	bool Start(void);

protected:

	

	string m_listen_ip;    //����IP
	int    m_listen_port;  //�����˿�

	AsyncNetDriver & m_net_driver; //����ӿ�

	typedef map<CallID, CSession> session_map_t;
	session_map_t m_session_map;

	//�����¼�������
	void on_event(Event & e);
	//TODO �ڴ�й©���� ����Է�Ͷ���˵�event�а���һ��NEW�ĳ�Աָ�룬���eventû���ҵ�session�����޷�����Ҳ���޷����ٴ�ָ��
	virtual void on_no_session_event(Event & e); //����û���ҵ�session���¼�

	//�����¼�
	virtual int on_listen_newsock(CSession & session, EventData * p_edata); //�����µ����ӵ�socket fd�����󷵻�-1
	void on_recv_packet(CSession & session, EventData * p_edata);
	void on_send_packet(CSession & session, EventData * p_edata);
	void on_close_sock(CSession & session, EventData * p_edata);

	//ҵ�����̴���
	void on_proc_event(CSession & session, EventData * p_edata);
	void start_process(CSession & session);
	virtual void end_process(CSession & session);

	//���ݽ���packet�����ͷ��ض�Ӧ�Ĵ������̣���������ڴ����ͣ��򷵻�NULL
	virtual CProcess * create_process(CSession & session);
};



#endif //SESSION_MANAGER_H
