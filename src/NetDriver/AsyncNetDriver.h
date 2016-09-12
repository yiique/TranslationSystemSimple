
#ifndef ASYNC_NET_DRIVER_H
#define ASYNC_NET_DRIVER_H

//BOOST
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

//MAIN
#include "EventEngine/EventEngine.h"
#include "NetPacket.h"
#include "NetType.h"
#include "NetRequest.h"
#include "NetResponse.h"
#include "UidGenerator.h"
#include "Log/LogStream.h"
#include "Acceptor.h"
#include "Connection.h"


//STL
#include <string>
#include <queue>
#include <map>
#include <utility>
#include <iostream>
#include <vector>
using namespace std;



typedef enum
{
	SOCKET_LISTEN, //0
	SOCKET_CONNECT, //1
	SOCKET_RECV, //2
	SOCKET_SEND, //3
	SOCKET_SUSPEND, //4
	SOCKET_CLOSE //5
} SocketState;

typedef enum
{
	PROTL_SUC,
	PROTL_ERR_NOSOCK,  // 此socket未注册
	PROTL_ERR_STATE,   //操作与当前状态不匹配
	PROTL_ERR_RECV,     //接收数据包格式错
	PROTL_ERR_TIMEOUT   //超时
} ProtlErr;




#define DEF_MAX_EPOLL_SIZE  1000
#define RECV_BUF_SIZE 		2048000 
#define PIPE_BUF_SIZE 		1024
#define TCP_STACK_SIZE      100
#define SOCK_IDLE_TIMEOUT   300


class AsyncNetDriver 
	: public EventEngine,
	  private boost::noncopyable
{
public:
	AsyncNetDriver(void);
	~AsyncNetDriver(void);


	//Function: 侦听本地端口
	//Return:  创建侦听socket成功，返回此socket句柄，否则返回-1，可通过errno获得错误代码
	//Event :  1） 收到新连接， 事件内容：新连接句柄
	//         2） socket错误，事件内容：错误代码
	sock_t Listen(const char * ip,
				const int port,
				EventEngine *p_caller,
				const CallID & cid);


	//Function: 连接远端服务器
	//Return:  创建连接socket成功，返回此socket句柄，否则返回-1，可通过errno获得错误代码
	//Event :  1） 连接远程服务器成功， 事件内容：NULL
	//         2） socket错误，事件内容：错误代码
	sock_t Connect(const char * ip,
				const int port,
				const char * cliip,
				EventEngine *p_caller,
				const CallID & cid);


	//Function: 接收数据
	//Return:  通知AsyncNetDriver成功，返回0；参数或其他错误，返回-1；
	//Event :  1） 接收数据成功， 事件内容：NetPacket句柄
	//         2） socket错误，事件内容：错误代码
	//         3） 协议层错误，事件内容：错误代码
	int Receive(sock_t sockfd,
				NetPacket * p_packet,
				EventEngine *p_caller,
				const CallID & cid);

	//Function: 发送数据
	//Return:  通知AsyncNetDriver成功，返回0；参数或其他错误，返回-1；
	//Event :  1） 发送数据成功， 事件内容：NetPacket句柄
	//         2） socket错误，事件内容：错误代码
	//         3） 协议层错误，事件内容：错误代码
	int Send(sock_t sockfd,
				NetPacket * p_packet,
				EventEngine *p_caller,
				const CallID & cid);

	//Function: 在epoll中去除socket
	//Return:  通知AsyncNetDriver成功，返回0；参数或其他错误，返回-1；
	//Event :  1） 关闭成功， 事件内容：NULL
	//         2） socket错误，事件内容：错误代码
	//         3） 协议层错误，事件内容：错误代码
	int Shutdown(sock_t sockfd,
				 EventEngine *p_caller,
				 const CallID & cid);

	//重写基类PostEvent方法，主要区别是需要调用notify()唤醒epoll
	int PostEvent(const Event & e);

private:

	int run();                                           //重新实现基类run方法

	void req_listen(EventData * p_edata, EventEngine * p_caller);
	void req_connect(EventData * p_edata, EventEngine * p_caller);
	void req_close(EventData * p_edata, EventEngine * p_caller);
	void req_recv(EventData * p_edata, EventEngine* p_caller);
	void req_send(EventData * p_edata, EventEngine* p_caller);

	//忽略原 基类on_event()，因为已经将run重载。
	void on_event(Event & e) {}
	void on_timer(const boost::system::error_code& error);

public:
	ConnectionManager m_connection_manager;
	AcceptorManager m_acceptor_manager;

	boost::asio::io_service m_io_service;
	boost::shared_ptr<boost::asio::io_service::work > msp_io_service_worker;

	boost::asio::deadline_timer m_timer;

	UidGenerator m_sockfd_generator;

	void post_handler();

};




#endif //ASYNC_NET_DRIVER_H
