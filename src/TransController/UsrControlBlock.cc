#include "UsrControlBlock.h"
#include "Common/ErrorCode.h"
#include "DataBase/DBOperation.h"

int UsrControlBlock::Submit(TransText * p_trans_text)
{
	assert(p_trans_text);

	if(p_trans_text->IsAllTrans())
	{
		return ERR_NO_NEED_TRANS;
	}

	m_text_block_list.push_back(text_block_share_ptr(new TextBlock(p_trans_text)));

	return SUCCESS;
}

list<text_block_share_ptr>::iterator UsrControlBlock::find_text_block(const TextID & text_id)
{
	list<text_block_share_ptr>::iterator iter = m_text_block_list.begin();

	for(; iter != m_text_block_list.end(); ++iter)
	{
		TransText * p_text = (*iter)->p_trans_text;
		if(p_text->GetID() == text_id)
		{
			return iter;
		}
	}

	return m_text_block_list.end();
}

pair<int, TransText *> UsrControlBlock::Cancel(const TextID & text_id)
{

	list<text_block_share_ptr>::iterator iter = find_text_block(text_id);

	if(m_text_block_list.end() != iter)
	{
		TransText * p_text = (*iter)->p_trans_text;

		m_text_block_list.erase(iter);
		return make_pair(SUCCESS, p_text);
	}

	return make_pair(ERR_NOT_FIND_TEXT, (TransText *) NULL);
}

TransText * UsrControlBlock::GetTransTextCopy(const TextID & text_id)
{
	list<text_block_share_ptr>::iterator iter = find_text_block(text_id);

	if(m_text_block_list.end() != iter)
	{
		return (*iter)->p_trans_text;
	}

	return NULL;
}

TransText * UsrControlBlock::GetTransText(const TextID & text_id)
{
	list<text_block_share_ptr>::iterator iter = find_text_block(text_id);

	if(m_text_block_list.end() != iter)
	{
		TransText * p_text = (*iter)->p_trans_text;

		m_text_block_list.erase(iter);
		return p_text;
	}

	return NULL;
}

int UsrControlBlock::Suspend(const TextID & text_id)
{
	list<text_block_share_ptr>::iterator iter = find_text_block(text_id);

	if(m_text_block_list.end() != iter)
	{
		(*iter)->text_state = STATE_SUSPEND;
		return SUCCESS;
	}

	return ERR_NOT_FIND_TEXT;
}

int UsrControlBlock::Rerun(const TextID & text_id)
{
	list<text_block_share_ptr>::iterator iter = find_text_block(text_id);

	if(m_text_block_list.end() != iter)
	{
		(*iter)->text_state = STATE_READY;
		return SUCCESS;
	}

	return ERR_NOT_FIND_TEXT;
}

trans_task_share_ptr UsrControlBlock::CreateTask()
{
	const size_t block_cnt = m_text_block_list.size();
	size_t cnt = 0;

	while(cnt < block_cnt)
	{
		++cnt;
		text_block_share_ptr sp_block = m_text_block_list.front();
		m_text_block_list.pop_front();
		m_text_block_list.push_back(sp_block);

		if(STATE_READY == sp_block->text_state)
		{
			TransSrc trans_src;

			if(sp_block->GetUnTransSrc(trans_src))
			{
				return trans_task_share_ptr(new TransTask(trans_src, sp_block->p_trans_text->SubSrcLanguage(), shared_from_this()));
			}
		}
	}

	return trans_task_share_ptr();
}

TransText * UsrControlBlock::Success(trans_task_share_ptr sp_task)
{
	list<text_block_share_ptr>::iterator iter = find_text_block(sp_task->GetTextID());

	if(iter == m_text_block_list.end())
	{
		return NULL;
	}

	text_block_share_ptr sp_block = *iter;

	TransText * p_trans_text = sp_block->p_trans_text;

	sp_block->SetTransResult(sp_task->trans_tgt);

	if(m_is_report_state)
	{
		int percent = p_trans_text->GetCurrTransSize() * 100 / p_trans_text->GetTransSize();
		DBOperation::ReportTransPercent(p_trans_text->GetID(), percent);
	}

	if(sp_block->p_trans_text->IsAllTrans())
	{
		m_text_block_list.erase(iter);
		return p_trans_text;
	}else
	{
		return NULL;
	}
}

void UsrControlBlock::Failed(trans_task_share_ptr sp_task)
{
	list<text_block_share_ptr>::iterator iter = find_text_block(sp_task->GetTextID());

	if(iter == m_text_block_list.end())
	{
		return;
	}

	(*iter)->BackUnTransSrc(sp_task->trans_src._cell_id);
}
