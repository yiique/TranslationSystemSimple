/**
* Copyright (c) 2010, 智能信息重点实验室,中国科学院计算技术研究所
* All rights reserved.
*
* 文件名： LogStream.h
* @brief  日志类，封装日志的输出的格式和方式
* @project ToWin在线翻译平台
*
* @comment 
* @version 1.1
* @author 傅春霖
* @date 2010-06-13
* @
*/


#ifndef LOG_STREAM_H
#define LOG_STREAM_H

//MAIN
#include "Common/BasicType.h"
#include "LogPrinter.h"
#include "LogData.h"

//STL
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class LogStream
{
public:
	~LogStream(void);

	/**
	 * @brief  工厂方法，产生一个LogStream实例
	 * @return 返回值 无
	 */
	static LogStream CreateLogStream();

	// Typedef  类型定义：endl, 用于写<<操作符
	typedef basic_ostream<char, char_traits<char> >& (*EndlType)(basic_ostream<char, char_traits<char> >&);

	/**
	 * @brief 重载操作符<< 在endl时添加操作
	 * @param[in]  endl_obj endl对象
	 * @return 返回值 无
	 */
	LogStream & operator << (EndlType endl_obj);

	/**
	 * @brief 重载操作符<< 特别输出DomainType对象
	 * @param[in]  domain  领域
	 * @return 返回值 无
	 */
	LogStream & operator << (const DomainType & domain);
	
	/**
	 * @brief 重载操作符<< 特别输出ServerAddrType对象
	 * @param[in]  addr  服务器地址
	 * @return 返回值 无
	 */
	LogStream & operator << (const ServerAddrType & addr);

	/**
	 * @brief 重载操作符<< 其他类型
	 * @param[in]  content  待输出内容
	 * @return 返回值 无
	 */
	template <class Type>
	LogStream & operator << (const Type & content)
	{
		m_sstream << content;
		return *this;
	};


private:

	//构造函数私有 只能有工厂方法产生
	LogStream(void);

	//私有拷贝构造函数和拷贝操作符
	LogStream(const LogStream & rsh);
	LogStream & operator = (const LogStream &rsh);

    // Member   输出缓存 当遇到endl时 才输出
    stringstream m_sstream;

	/**
	 * @brief  输出 将现有数据发送给LogPrinter
	 * @return 返回值 无
	 */
	void output();

};

class NullLogStream
{
public:
	NullLogStream(void) {};
	~NullLogStream(void) {};

	static NullLogStream CreateNullLogStream()
	{
		NullLogStream nulllog;

		return nulllog;
	}

	// Typedef  类型定义：endl, 用于写<<操作符
	typedef basic_ostream<char, char_traits<char> >& (*EndlType)(basic_ostream<char, char_traits<char> >&);

	NullLogStream & operator << (EndlType endl_obj)
	{
		//DO NOTHING
		return *this;
	}

	template <class Type>
	NullLogStream & operator << (const Type & content)
	{
		//DO NOTHING
		return *this;
	}
};


//=========================================//
//定义日志输出级别
//调试1 ldbg1  输出调试信息，如程序执行的上下文，程序中间输出结果等
//调试2 ldbg2  输出调试信息中的计划内错误，如插入删除失败，词典ID未找到，报文格式解析错误等   
//普通  lout   输出常规信息，如资源读取的路径，配置文件信息等
//警告  lwrn   输出警告信息，即可以忽略的错误信息（此错误可丢弃，仅对本次业务有影响），如：收到错误的报文，错误的事件类型等
//错误  lerr   输出致命错误，即造成程序功能无法继续执行的错误，如：读取资源文件失败，程序逻辑错误等


//1. 调试1 ldbg1
#ifdef LOG_DEBUG1_OFF 
	#define ldbg1 ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define ldbg1 ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ]" << " [DEBUG_1] " )
	#else
		#define ldbg1 ( LogStream::CreateLogStream() << " [DEBUG_1] " )
	#endif
#endif

//2. 调试 ldbg2
#ifdef LOG_DEBUG2_OFF 
	#define ldbg2 ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define ldbg2 ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ]" << " [DEBUG_2] " ) 
	#else
		#define ldbg2 ( LogStream::CreateLogStream() << " [DEBUG_2] " )
	#endif
#endif

//3. 普通 lout
#ifdef LOG_INFO_OFF 
	#define lout ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define lout ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ] " ) 
	#else
		#define lout ( LogStream::CreateLogStream() )
	#endif
#endif

//4. 警告 lwrn
#ifdef LOG_WARNNING_OFF 
	#define lwrn ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define lwrn ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ]" << " [WARNNING] ") 
	#else
		#define lwrn ( LogStream::CreateLogStream() << " [WARNNING] " )
	#endif
#endif


//5. 错误 lerr
#ifdef LOG_ERROR_OFF 
	#define lerr ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define lerr ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ]" << " [ERROR] ") 
	#else
		#define lerr ( LogStream::CreateLogStream() << " [ERROR] " )
	#endif
#endif

//END
//==========================================//


#endif //LOG_STREAM_H
