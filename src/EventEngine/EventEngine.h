/**
* Copyright (c) 2012, 橙译中科信息技术（北京）有限公司
* All rights reserved.
*
* 文件名：EventEngine.h
* @brief  基于事件的处理引擎, linux/windows通用
* @project ToWin在线翻译平台
*
* @comment 
* @version 1.1
* @author 傅春霖
* @date 2012-02-07
* @
*/


#ifndef EVENT_ENGINE_H
#define EVENT_ENGINE_H

//BOOST
#include <boost/thread/thread.hpp>

//MAIN
#include "BlockingDeque.h"
#include "EventData.h"

//STL
#include <vector>
#include <utility>
#include <iostream>
#include <set>
using namespace std;

//前置声明
class EventEngine;


/*
 * @brief 定义事件Event的优先级
 */
typedef enum
{
	EVENT_PRIORITY_HIGH,
	EVENT_PRIORITY_NORMAL
} EventPriority;



class Event
{

public:
	Event(EventData * p_edata = NULL, 
		  EventEngine * p_caller = NULL, 
		  const EventPriority priority = EVENT_PRIORITY_NORMAL ): _p_edata(p_edata),
										 						  _p_caller(p_caller),
				 											      _priority(priority)
	{
	};

	Event(const Event & rsh)
	{
		if( &rsh != this)
		{
			_p_edata  = rsh._p_edata;
			_p_caller = rsh._p_caller;
			_priority = rsh._priority;
		}
	};

	~Event(void) {};

	//事件数据
	EventData *    _p_edata;

	//事件发送方
	EventEngine *  _p_caller;

	//事件优先级
	EventPriority _priority;
	
};


class EventEngine
{
protected:

	//当前状态
	typedef enum{STATE_STOP, STATE_RUN} state_t;

	//默认事件读取缓冲区大小 0表示全部不限制大小 一次全部取空
	static const size_t DEF_INPUT_SIZE = 1; 

public:
	EventEngine(void);

	virtual ~EventEngine(void);

	/**
	 * @brief 向EventEngine插入一个待处理事件
	 * @param[in]  e 事件对象
	 * @return 返回值 int， 返回非0值表示出错
	 */
	int PostEvent(const Event & e);

	/**
	 * @brief 启动EventEngine，Start内部会产生一个工作线程，然后从start函数返回
	 * @return 返回值 bool 
	 */
	virtual bool Start(const size_t num = 1);

protected:

	//事件阻塞队列
	BlockingDeque<Event> m_input_blkque;

	//当前运行状态
	state_t m_state;

	//设置每次从阻塞队列里面读取的最大事件数
	size_t m_input_size;

	//工作线程数
	size_t m_thread_num;
	boost::thread_group m_thread_group;

	/**
	 * @brief  工作线程调用的主循环函数，循环从事件队列里面读取事件，正常状态下一直运行，不会返回
	 * @return 返回值 0 ，表示正常结束， 其他值 错误
	 */
	virtual int run();

	/**
	 * @brief  事件处理函数  由子类实现
	 * @return 返回值 无
	 */
	virtual void on_event(Event & e) = 0;

	//非线程安全，当m_thread_num 大于1时禁止使用
	//CallID产生器
	CallID m_next_cid;
	set<CallID> m_using_cid_set;

	CallID generate_call_id();



	void release_call_id(const CallID & cid);
};



#endif 
