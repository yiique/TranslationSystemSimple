#ifndef USR_CONTROL_BLOCK_H
#define USR_CONTROL_BLOCK_H

#include <assert.h>

//BOOST
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

//MAIN
#include "TransTask.h"
#include "Common/BasicType.h"
#include "TransText/TransText.h"

//STL
#include <list>
#include <set>
#include <utility>
using namespace std;

typedef enum {
	STATE_READY,
	STATE_SUSPEND
	
} TextState;

class TextBlock
	: private boost::noncopyable
{
public:
	TextBlock(TransText * p_text, TextState state = STATE_READY): p_trans_text(p_text), text_state(state) 
	{
		assert(p_text);

		for(size_t i=0; i<p_text->GetTransSize(); ++i)
		{
			m_untrans_idx_set.insert(i);
		}
	}

	bool GetUnTransSrc(TransSrc & trans_src)
	{
		if(m_untrans_idx_set.size() == 0 )
			return false;

		set<size_t>::iterator iter = m_untrans_idx_set.begin();
		const size_t idx = *(iter);
		m_untrans_idx_set.erase(iter);

		trans_src = p_trans_text->GetTransSrc(idx);

		#ifdef ENABLE_TIME_LOG
		p_trans_text->time_log[T_TOTAL][T_TRANSLATE][T_TRANSLATE_REAL][idx].Start();
		#endif //ENABLE_TIME_LOG

		return true;
	}

	void BackUnTransSrc(const size_t idx)
	{
		m_untrans_idx_set.insert(idx);
	}

	bool SetTransResult(const TransTgt & trans_tgt)
	{
		#ifdef ENABLE_TIME_LOG
		p_trans_text->time_log[T_TOTAL][T_TRANSLATE][T_TRANSLATE_REAL][trans_tgt._cell_id].Stop();
		#endif //ENABLE_TIME_LOG

		return p_trans_text->SetTransResult(trans_tgt);
	}

	size_t GetUntransSize() const
	{
		return m_untrans_idx_set.size();
	}


public:
	TextState text_state;
	TransText * p_trans_text;

private:
	set<size_t> m_untrans_idx_set;
	
};

typedef boost::shared_ptr<TextBlock> text_block_share_ptr;

class UsrControlBlock
	: public boost::enable_shared_from_this<UsrControlBlock>, 
	  private boost::noncopyable
{
public:
	UsrControlBlock(const UsrID & usr_id, const bool is_report_state) : m_usr_id(usr_id), m_is_report_state(is_report_state) {}

	trans_task_share_ptr CreateTask();
	int Submit(TransText * p_trans_text);
	pair<int, TransText *> Cancel(const TextID & text_id);
	int Suspend(const TextID & text_id);
	int Rerun(const TextID & text_id);

	const UsrID & GetUsrID() const { return m_usr_id; }
	TransText * GetTransText(const TextID & text_id);
	TransText * GetTransTextCopy(const TextID & text_id);

	TransText * Success(trans_task_share_ptr sp_task);
	void Failed(trans_task_share_ptr sp_task);

	bool Empty() const
	{
		return (m_text_block_list.size() == 0) ? true : false;
	}

private:

	UsrID m_usr_id;

	bool m_is_report_state;

	list<text_block_share_ptr> m_text_block_list;

	list<text_block_share_ptr>::iterator find_text_block(const TextID & text_id);

};

typedef boost::shared_ptr<UsrControlBlock> ucb_share_ptr; 

#endif //USR_CONTROL_BLOCK_H
