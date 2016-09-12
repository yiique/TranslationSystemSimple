#ifndef RWLOCK_H
#define RWLOCK_H

//BOOST
#include <boost/noncopyable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

//STL
#include <iostream>
using namespace std;

//写优先读写锁
class RWLock
	: private boost::noncopyable
{
public:
	RWLock(void);
	~RWLock(void);

	void ReadLock();
	void ReadUnlock();

	void WriteLock();
	void WriteUnlock();

private:
	boost::mutex m_mutex;
	boost::condition_variable  m_read_cond;
	boost::condition_variable  m_write_cond;

	int m_hold_flag;  // >0 表示读操作数目 <0 表示写操作数目 =0表示空闲

	size_t m_read_wait_cnt;
	size_t m_write_wait_cnt;
};

class ReadLockGuard
	: private boost::noncopyable
{
public:
	explicit ReadLockGuard(RWLock & rw_lock) : m_rw_lock(rw_lock)
	{
		m_rw_lock.ReadLock();
	}

	~ReadLockGuard()
	{
		m_rw_lock.ReadUnlock();
	}

private:
	RWLock & m_rw_lock;
};

class WriteLockGuard
	: private boost::noncopyable
{
public:
	explicit WriteLockGuard(RWLock & rw_lock) : m_rw_lock(rw_lock)
	{
		m_rw_lock.WriteLock();
	}

	~WriteLockGuard()
	{
		m_rw_lock.WriteUnlock();
	}

private:
	RWLock & m_rw_lock;
};


#endif //RWLOCK_H
