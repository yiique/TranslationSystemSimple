#ifndef TRANS_CONTROLLER_H
#define TRANS_CONTROLLER_H

//BOOST
#include <boost/noncopyable.hpp>

//MAIN
#include "NetDriver/NetInterface.h"
#include "EventEngine/EventData.h"
#include "EventEngine/EventEngine.h"
#include "TransCore.h"
#include "Log/LogStream.h"
#include "Common/ErrorCode.h"
#include "Configurator/Configurator.h"
#include "TransSession.h"
#include "TransControlEvent.h"
#include "Common/BasicType.h"
#include "SessionManager/UsrSessManager.h"
#include "OltManager/OltTaskManager.h"
#include "Timer/TimeEvent.h"

//STL
#include <map>
#include <string>
using namespace std;

#define DEF_SER_TIMEOUT 60
#define MAX_TRANS_TIMES_PER_SENT 5

#define CONTROL_TYPE_STR_SUSPEND "SUSPEND"
#define CONTROL_TYPE_STR_RERUN   "RERUN"
#define CONTROL_TYPE_STR_CANCEL  "CANCEL"

class TransController
	: public EventEngine
{
public:
	~TransController(void);

	//获得翻译控制器
	static TransController * GetRttInstance();  //即时翻译控制器
	static TransController * GetOltInstance();  //离线翻译控制器


	//重写基类Start函数
	bool Start(const Configurator & config, int timeout_sec = DEF_SER_TIMEOUT);
	
	void PostSlavePluse(const TransSlaveInfo & slave_info, const CallID & cid, EventEngine * p_caller);
	void PostTransSubmit(TransText * p_trans_text);
	void PostTransSuspend(const DomainType & domain, const UsrID & usr_id, const TextID & text_id, const CallID & cid, EventEngine * p_caller);
	void PostTransRerun(const DomainType & domain, const UsrID & usr_id, const TextID & text_id, const CallID & cid, EventEngine * p_caller);
	void PostTransCancel(const DomainType & domain, const UsrID & usr_id, const TextID & text_id, const CallID & cid, EventEngine * p_caller);

	void GetTransSent(const DomainType & domain, 
					  const UsrID & usr_id, 
					  const TextID & text_id, 
					  const size_t offset, 
					  const CallID & cid, 
					  EventEngine * p_caller);

private:

	//唯一实例
	static TransController ms_rtt_trans_controller; //即时翻译控制器实例
	static TransController ms_olt_trans_controller;	//离线翻译控制器实例

	TransType m_trans_type;
	bool m_is_report_state;

	Configurator m_config;
	string m_connect_ip;
	int m_timeout_sec;

	typedef map<CallID, TransSession> trans_sess_map_t;
	trans_sess_map_t m_trans_sess_map;    //翻译Session列表
	AsyncNetDriver & m_net_driver;          //用于连接翻译服务器的网络接口

	typedef map<DomainType, trans_core_share_ptr> trans_core_map_t;
	trans_core_map_t m_trans_core_map;

	//yl.
	LangDirectionType m_langtype;
	DomainType m_domaintype;

private:

	//单态模式
	TransController(AsyncNetDriver * p_net_driver, const TransType trans_type);

	//禁掉基类的Start函数
	bool Start(void) { return false; } 

	bool init_trans_cores();

	//基类事件处理函数
	void on_event(Event & e);

	//事件处理
	void on_slave_pluse(Event & e); 
	void on_trans_submit(Event & e); 
	void on_trans_suspend(Event & e); 
	void on_trans_rerun(Event & e);   
	void on_trans_cancel(Event & e);
	void on_get_trans_sent(Event & e);
	void on_timer(Event & e);
	void on_net(Event & e);

	//操作
	void call_slave_for_trans(const vector<trans_task_share_ptr> & task_ptr_vec);
	void finish_translation(const int res, TransText * p_trans_text);

	//网络相关操作
	void on_session_success(trans_sess_map_t::iterator iter);
	void on_session_failed(trans_sess_map_t::iterator iter);
	void on_connected(trans_sess_map_t::iterator iter, ConnNetRes * p_conn_res);
	void on_recv_packet(trans_sess_map_t::iterator iter, RecvNetRes * p_recv_res);
	void on_send_packet(trans_sess_map_t::iterator iter, SendNetRes * p_send_res);
	void on_close_sock(trans_sess_map_t::iterator iter, CloseNetRes * p_close_res);

#ifdef ENABLE_SOFT_PROTECT
	void handle_max_use_cnt(trans_task_share_ptr sp_task);
#endif //ENABLE_SOFT_PROTECT

};





#endif //TRANS_CONTROLLER_H
