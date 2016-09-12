/**
* Copyright (c) 2010, 智能信息重点实验室,中国科学院计算技术研究所
* All rights reserved.
*
* 文件名： LogData.h
* @brief  Log数据包装类，继承自EventData
* @project ToWin在线翻译平台
*
* @comment 
* @version 1.1
* @author 傅春霖
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
	 * @brief 继承自EventData，返回LogData 的类型
	 * @return 返回值EDType 
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
	
	//输出此日志的来源线程id
	boost::thread::id m_log_tid;

	//日志内容
	string m_log_content;

	//输出时间戳
	boost::posix_time::ptime m_time_stamp;

};


#endif //LOG_DATA_H

