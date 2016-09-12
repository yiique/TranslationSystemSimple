/**
* Copyright (c) 2011, 智能信息重点实验室,中国科学院计算技术研究所
* All rights reserved.
*
* 文件名：BaseMutex.h
* @brief  互斥锁, linux/windows平台通用
* @project 在线翻译平台
*
* @comment 
* @version 1.0
* @author 傅春霖
* @date 2011-08-10
* @
*/

#ifndef BASE_MUTEX_H
#define BASE_MUTEX_H

//BOOST
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

class MutexLock 
	: public boost::noncopyable
{
private:
	boost::mutex m_mutex;

public:
	MutexLock() {}
	
	~MutexLock() {}
	
	void Lock()
	{
		m_mutex.lock();
	}
	
	void Unlock()
	{
		m_mutex.unlock();
	}

	boost::mutex & GetMutexRef()
	{
		return m_mutex;
	}
};

class MutexLockGuard
	: public boost::noncopyable
{
private:
	boost::lock_guard<boost::mutex> m_lock_guard;

public:
	explicit MutexLockGuard(MutexLock & mutex) : m_lock_guard(mutex.GetMutexRef()) {}
	
	~MutexLockGuard() {}
	
};


#endif //BASE_MUTEX_H
