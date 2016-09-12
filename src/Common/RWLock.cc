#include "RWLock.h"

//STL
#include <iostream>
using namespace std;

RWLock::RWLock(void) : m_hold_flag(0), m_read_wait_cnt(0), m_write_wait_cnt(0)
{
}

RWLock::~RWLock(void) 
{
}

void RWLock::ReadLock()
{
	boost::mutex::scoped_lock lock(m_mutex);

	++m_read_wait_cnt;

	while(true)
	{
		if(m_hold_flag >= 0 && m_write_wait_cnt == 0)
		{
			++m_hold_flag;
			--m_read_wait_cnt;

			return;
		}

		m_read_cond.wait(lock);
	}

}

void RWLock::ReadUnlock()
{
	boost::mutex::scoped_lock lock(m_mutex);

	assert(m_hold_flag > 0);

	--m_hold_flag;

	if(m_hold_flag == 0 && m_write_wait_cnt > 0)
	{
		m_write_cond.notify_one();

	}else if(m_hold_flag >= 0 && m_write_wait_cnt == 0 && m_read_wait_cnt > 0)
	{
		m_read_cond.notify_all();
	}

}

void RWLock::WriteLock()
{
	boost::mutex::scoped_lock lock(m_mutex);

	++m_write_wait_cnt;

	while(true)
	{
		if(m_hold_flag == 0)
		{
			--m_hold_flag;
			--m_write_wait_cnt;

			return;
		}

		m_write_cond.wait(lock);
	}
}

void RWLock::WriteUnlock()
{
	boost::mutex::scoped_lock lock(m_mutex);

	assert(m_hold_flag == -1);

	++m_hold_flag;

	if(m_hold_flag == 0 && m_write_wait_cnt > 0)
	{
		m_write_cond.notify_one();
	}else if(m_hold_flag == 0 && m_write_wait_cnt == 0 && m_read_wait_cnt > 0)
	{
		m_read_cond.notify_all();
	}

}


