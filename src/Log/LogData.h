/**
* Copyright (c) 2010, ������Ϣ�ص�ʵ����,�й���ѧԺ���㼼���о���
* All rights reserved.
*
* �ļ����� LogData.h
* @brief  Log���ݰ�װ�࣬�̳���EventData
* @project ToWin���߷���ƽ̨
*
* @comment 
* @version 1.1
* @author ������
* @date 2010-06-13
* @
*/

#ifndef LOG_DATA_H
#define LOG_DATA_H

//BOOST
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

//MAIN
#include "EventEngine/EventData.h"

//STL
#include <string>
using namespace std;

class LogData
	: public EventData
{
public:
	LogData(boost::thread::id tid, const string & log_content, const boost::posix_time::ptime & time_stamp);
	~LogData(void);

	/**
	 * @brief �̳���EventData������LogData ������
	 * @return ����ֵEDType 
	 */
	virtual const EDType GetType() const;

	const boost::thread::id & GetThreadID() const
	{
		return m_log_tid;
	};

	const string & GetContent() const
	{
		return m_log_content;
	};

	const boost::posix_time::ptime & GetTimeStamp() const
	{
		return m_time_stamp;
	}

private:
	
	//�������־����Դ�߳�id
	boost::thread::id m_log_tid;

	//��־����
	string m_log_content;

	//���ʱ���
	boost::posix_time::ptime m_time_stamp;

};


#endif //LOG_DATA_H

