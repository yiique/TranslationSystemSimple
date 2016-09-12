#ifndef USR_SESS_MANAGER_H
#define USR_SESS_MANAGER_H

//MAIN
#include "SessionManager.h"
#include "Common/MsgType.h"
#include "NetDriver/NetInterface.h"


//STL
#include <utility>
#include <map>
using namespace std;

class UsrSessManager
	: public SessionManager
{
public:
	UsrSessManager(AsyncNetDriver * p_net_driver);
	~UsrSessManager(void);
	
	static UsrSessManager * GetInstance();

	bool Start();

private:
	//重写基类create_process函数
	CProcess * create_process(CSession & session);

	//重写基类void on_no_session_event(Event & e)函数 用于拦截即时翻译结果
	void on_no_session_event(Event & e);

	//重现基类end_process方法，用于清理m_rtt_sess_text_map
	void end_process(CSession & session);

	//注册即时翻译ID
	TextID registe_rtt_trans_text(const CallID & sess_cid);
	map<TextID, CallID> m_rtt_sess_text_map;

	//重载基类函数，用于输出来源ip等信息
	int on_listen_newsock(CSession & session, EventData * p_edata);

};




#endif //USR_SESS_MANAGER_H

