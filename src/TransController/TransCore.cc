#include "TransCore.h"

vector<trans_task_share_ptr> TransCore::GetReadyTask()
{

	vector<trans_task_share_ptr> task_vec;

	trans_slave_share_ptr slave_ptr = get_idle_slave();
	size_t loop_fail_cnt = 0;
	const size_t usr_cnt = m_usr_list.size();

	while(slave_ptr && loop_fail_cnt < usr_cnt)
	{
		ucb_share_ptr ucb_ptr = m_usr_list.front();
		m_usr_list.pop_front();
		m_usr_list.push_back(ucb_ptr);
		

		trans_task_share_ptr task_ptr = ucb_ptr->CreateTask();

		if(task_ptr)
		{
			task_ptr->sp_trans_core = shared_from_this();
			task_ptr->sp_slave = slave_ptr;
			slave_ptr->ReduceIdelNum();

			task_vec.push_back(task_ptr);

			loop_fail_cnt = 0;
		}else
		{
			++loop_fail_cnt;
		}

		slave_ptr = get_idle_slave();
	}

	return task_vec;
}

trans_slave_share_ptr TransCore::get_idle_slave()
{
	//lout << "slave map size = " << m_slave_map.size() << endl;

	map<string, trans_slave_share_ptr>::iterator iter = m_slave_map.begin();

	for(; iter != m_slave_map.end(); ++iter)
	{
		trans_slave_share_ptr & sp_slave = iter->second; 

		if(sp_slave->GetIdelNum() > 0)
		{
			return sp_slave;
		}
	}

	return trans_slave_share_ptr();

}


void TransCore::PluseSlave(const TransSlaveInfo & slave_info)
{
	

	pair<map<string, trans_slave_share_ptr>::iterator, bool> res = m_slave_map.insert(make_pair(slave_info.slave_uuid, trans_slave_share_ptr()));

	if(true == res.second)
	{
		res.first->second.reset(new TransSlave(slave_info)); 
		m_slave_timeout_que.Append(slave_info.slave_uuid, res.first->second);

		lout << "New server_addr = " << res.first->second->GetInfo().ip << ":" << res.first->second->GetInfo().port << " : " << slave_info.domain << endl;
		lout << "    ser_uuid = " << res.first->second->GetInfo().slave_uuid << endl;

	}else
	{
		res.first->second->Refresh(slave_info);
	}
	m_slave_timeout_que.Refresh(slave_info.slave_uuid);
}


void TransCore::CheckSlaveTimeout()
{
	//检查时间队列
	vector<trans_slave_share_ptr> timeout_vec;
	m_slave_timeout_que.Check(timeout_vec);

	for(size_t i=0; i<timeout_vec.size(); ++i)
	{
		trans_slave_share_ptr sp_slave = timeout_vec[i];

		lwrn << "TIME_OUT:       Domain = " << this->m_domain << endl; 
		lwrn << "           server_addr = " << sp_slave->GetInfo().ip << ":" << sp_slave->GetInfo().port << endl;
		lwrn << "              ser_uuid = " << sp_slave->GetInfo().slave_uuid << endl;

		//删除此超时服务器
		delete_slave(sp_slave);

	}
}

void TransCore::delete_slave(trans_slave_share_ptr sp_slave)
{
	m_slave_timeout_que.Remove(sp_slave->GetInfo().slave_uuid);
	lout << "delete server : " << sp_slave->GetInfo().ip << ":" << sp_slave->GetInfo().port << endl;

	m_slave_map.erase(sp_slave->GetInfo().slave_uuid);
}

bool TransCore::CheckAlive()
{
	return (m_slave_map.size() > 0) ? true : false;
}

int TransCore::SubmitTranslation(TransText * p_trans_text)
{
	assert(p_trans_text);

	lout << "call SubmitTranslation. text_id = " << p_trans_text->GetID() << endl;

	if(p_trans_text->IsAllTrans())
	{
		return ERR_NO_NEED_TRANS;
	}

	list<ucb_share_ptr>::iterator iter = find_ucb(p_trans_text->GetUsrID());

	if(m_usr_list.end() != iter)
	{
		return (*iter)->Submit(p_trans_text);
	}else
	{
		ucb_share_ptr sp_ucb(new UsrControlBlock(p_trans_text->GetUsrID(), m_is_report_state));

		int ret = sp_ucb->Submit(p_trans_text);

		if(SUCCESS != ret)
			return ret;

		m_usr_list.push_back(sp_ucb);

		return SUCCESS;

	}

}

pair<int, TransText *> TransCore::CancelTranslation(const UsrID & usr_id, const TextID & text_id)
{
	list<ucb_share_ptr>::iterator iter = find_ucb(usr_id);

	if(m_usr_list.end() == iter)
	{
		return make_pair(ERR_TRANS_CONTROL_NOT_THIS_USR, (TransText*) NULL);
	}

	ucb_share_ptr sp_ucb = *iter;

	pair<int, TransText *> ret = sp_ucb->Cancel(text_id);

	if(SUCCESS != ret.first)
		return ret;

	if(sp_ucb->Empty())
		m_usr_list.erase(iter);

	return ret;
}

int TransCore::SuspendTranslation(const UsrID & usr_id, const TextID & text_id)
{
	list<ucb_share_ptr>::iterator iter = find_ucb(usr_id);

	if(m_usr_list.end() == iter)
	{
		return ERR_TRANS_CONTROL_NOT_THIS_USR;
	}

	return (*iter)->Suspend(text_id);
}

int TransCore::RerunTranslation(const UsrID & usr_id, const TextID & text_id)
{
	list<ucb_share_ptr>::iterator iter = find_ucb(usr_id);

	if(m_usr_list.end() == iter)
	{
		return ERR_TRANS_CONTROL_NOT_THIS_USR;
	}

	return (*iter)->Rerun(text_id);
}

TransText * TransCore::GetTransTextCopy(const UsrID & usr_id, const TextID & text_id)
{
	list<ucb_share_ptr>::iterator iter = find_ucb(usr_id);

	if(m_usr_list.end() == iter)
	{
		return NULL;
	}

	return (*iter)->GetTransTextCopy(text_id);
}

TransText * TransCore::GetTransText(const UsrID & usr_id, const TextID & text_id)
{
	list<ucb_share_ptr>::iterator iter = find_ucb(usr_id);

	if(m_usr_list.end() == iter)
	{
		return NULL;
	}

	return (*iter)->GetTransText(text_id);
}

list<ucb_share_ptr>::iterator TransCore::find_ucb(const UsrID & usr_id)
{
	list<ucb_share_ptr>::iterator iter = m_usr_list.begin();

	for(; iter != m_usr_list.end(); ++iter)
	{
		if((*iter)->GetUsrID() == usr_id)
			return iter;
	}

	return m_usr_list.end();
}


TransText * TransCore::SuccessTask(trans_task_share_ptr sp_task)
{
	TransText * p_trans_text = sp_task->sp_ucb->Success(sp_task);

	if(sp_task->sp_ucb->Empty())
	{
		list<ucb_share_ptr>::iterator iter = find_ucb(sp_task->sp_ucb->GetUsrID());

		if(m_usr_list.end() != iter)
			m_usr_list.erase(iter);
	}

	sp_task->sp_slave->IncreaseIdelNum();

	return p_trans_text;
}

void TransCore::FailedTask(trans_task_share_ptr sp_task)
{
	sp_task->sp_ucb->Failed(sp_task);

	delete_slave(sp_task->sp_slave);
}
