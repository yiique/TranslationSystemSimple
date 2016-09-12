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
	//��д����create_process����
	CProcess * create_process(CSession & session);

	//��д����void on_no_session_event(Event & e)���� �������ؼ�ʱ������
	void on_no_session_event(Event & e);

	//���ֻ���end_process��������������m_rtt_sess_text_map
	void end_process(CSession & session);

	//ע�ἴʱ����ID
	TextID registe_rtt_trans_text(const CallID & sess_cid);
	map<TextID, CallID> m_rtt_sess_text_map;

	//���ػ��ຯ�������������Դip����Ϣ
	int on_listen_newsock(CSession & session, EventData * p_edata);

};




#endif //USR_SESS_MANAGER_H

