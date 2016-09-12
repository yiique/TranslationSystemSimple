
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

	//设置用户监听端口
	bool SetListen(const char * ip, const int port);

	//重写基类Start函数
	bool Start(void);

protected:

	

	string m_listen_ip;    //监听IP
	int    m_listen_port;  //监听端口

	AsyncNetDriver & m_net_driver; //网络接口

	typedef map<CallID, CSession> session_map_t;
	session_map_t m_session_map;

	//基类事件处理函数
	void on_event(Event & e);
	//TODO 内存泄漏隐患 如果对方投递了的event中包含一个NEW的成员指针，如果event没有找到session，则无法处理，也就无法销毁此指针
	virtual void on_no_session_event(Event & e); //处理没有找到session的事件

	//网络事件
	virtual int on_listen_newsock(CSession & session, EventData * p_edata); //返回新到连接的socket fd，错误返回-1
	void on_recv_packet(CSession & session, EventData * p_edata);
	void on_send_packet(CSession & session, EventData * p_edata);
	void on_close_sock(CSession & session, EventData * p_edata);

	//业务流程处理
	void on_proc_event(CSession & session, EventData * p_edata);
	void start_process(CSession & session);
	virtual void end_process(CSession & session);

	//根据接收packet的类型返回对应的处理流程，如果不存在此类型，则返回NULL
	virtual CProcess * create_process(CSession & session);
};



#endif //SESSION_MANAGER_H
