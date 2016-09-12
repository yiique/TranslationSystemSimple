/**
* Copyright (c) 2010, 智能信息重点实验室,中国科学院计算技术研究所
* All rights reserved.
*
* 文件名： LogPrinter.h
* @brief  日志输出类，继承自EventEngine
* @project ToWin在线翻译平台
*
* @comment 
* @version 1.1
* @author 傅春霖
* @date 2010-06-13
* @
*/

#ifndef LOG_PRINTER_H
#define LOG_PRINTER_H

//BOOST
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

//MAIN
#include "EventEngine/EventEngine.h"
#include "LogData.h"
#include "Common/f_utility.h"

//STL
#include <fstream>
#include <string>
#include <map>
#include <iostream>
using namespace std;


class LogPrinter
	: public EventEngine
{
public:
	~LogPrinter(void);

	static LogPrinter * GetInstance();

	/**
	 * @brief  重写父类的Start方法
	 * @param[in]  is_std_io  是否同时将日志输出到标准输出 true 输出， false 不输出
	 * @return 返回值 无
	 */
	bool Start(bool is_std_io = true);

private:

	//私有化构造器 单态模式
	LogPrinter(void);

	//唯一实例
	static LogPrinter ms_log_instance;

	//是否已经启动，避免重复启动
	bool m_is_start;

	//是否同时输出到标准输出
	bool m_is_std_io;

	//输出流
	ofstream m_log_file;

	//日志行计数
	unsigned long m_log_cnt;	

	/**
	 * @brief  事件处理函数  继承自父类EventEngine
	 * @return 返回值 无
	 */
	void on_event(Event & e);

	/**
	 * @brief  输入一个日志数据
	 * @return 返回值 无
	 */
	void print_data(const LogData & data);

};


#endif //LOG_PRINTER_H

