/**
* Copyright (c) 2011, ������Ϣ�ص�ʵ����,�й���ѧԺ���㼼���о���
* All rights reserved.
*
* �ļ�����BaseMutex.h
* @brief  ������, linux/windowsƽ̨ͨ��
* @project ���߷���ƽ̨
*
* @comment 
* @version 1.0
* @author ������
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
