#ifndef TRANS_CORE_H
#define TRANS_CORE_H

//BOOST
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

//MAIN
#include "TransSlave.h"
#include "Common/BasicType.h"
#include "TransText/TransText.h"
#include "UsrControlBlock.h"
#include "Timer/TimeQueue.h"
#include "Common/ErrorCode.h"
#include "TransTask.h"

//STL
#include <utility>
#include <string>
#include <map>
#include <list>
using namespace std;

class TransCore
	: public boost::enable_shared_from_this<TransCore>, 
	  private boost::noncopyable
{
public:
	TransCore(const DomainType & domain, 
			  const bool is_report_state, 
			  const int timeout_sec) : m_domain(domain), m_is_report_state(is_report_state), m_slave_timeout_que(timeout_sec) { }

	void PluseSlave(const TransSlaveInfo & slave_info);
	void CheckSlaveTimeout();
	bool CheckAlive();

	int SubmitTranslation(TransText * p_trans_text);
	pair<int, TransText *> CancelTranslation(const UsrID & usr_id, const TextID & text_id);
	int SuspendTranslation(const UsrID & usr_id, const TextID & text_id);
	int RerunTranslation(const UsrID & usr_id, const TextID & text_id);
	TransText * GetTransText(const UsrID & usr_id, const TextID & text_id);
	
	TransText * GetTransTextCopy(const UsrID & usr_id, const TextID & text_id);
	vector<trans_task_share_ptr> GetReadyTask();
	TransText * SuccessTask(trans_task_share_ptr sp_task);
	void FailedTask(trans_task_share_ptr sp_task);

private:

	DomainType m_domain;
	bool m_is_report_state;

	map<string, trans_slave_share_ptr> m_slave_map;
	TimeQueue<string, trans_slave_share_ptr> m_slave_timeout_que;

	list<ucb_share_ptr> m_usr_list;
	
private:

	trans_slave_share_ptr get_idle_slave();
	list<ucb_share_ptr>::iterator find_ucb(const UsrID & usr_id);
	void delete_slave(trans_slave_share_ptr sp_slave);
	
};

typedef boost::shared_ptr<TransCore> trans_core_share_ptr; 

#endif //TRANS_CORE_H
