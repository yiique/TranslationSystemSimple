/**
* Copyright (c) 2012, �����п���Ϣ���������������޹�˾
* All rights reserved.
*
* �ļ�����EventEngine.h
* @brief  �����¼��Ĵ�������, linux/windowsͨ��
* @project ToWin���߷���ƽ̨
*
* @comment 
* @version 1.1
* @author ������
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

//ǰ������
class EventEngine;


/*
 * @brief �����¼�Event�����ȼ�
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

	//�¼�����
	EventData *    _p_edata;

	//�¼����ͷ�
	EventEngine *  _p_caller;

	//�¼����ȼ�
	EventPriority _priority;
	
};


class EventEngine
{
protected:

	//��ǰ״̬
	typedef enum{STATE_STOP, STATE_RUN} state_t;

	//Ĭ���¼���ȡ��������С 0��ʾȫ�������ƴ�С һ��ȫ��ȡ��
	static const size_t DEF_INPUT_SIZE = 1; 

public:
	EventEngine(void);

	virtual ~EventEngine(void);

	/**
	 * @brief ��EventEngine����һ���������¼�
	 * @param[in]  e �¼�����
	 * @return ����ֵ int�� ���ط�0ֵ��ʾ����
	 */
	int PostEvent(const Event & e);

	/**
	 * @brief ����EventEngine��Start�ڲ������һ�������̣߳�Ȼ���start��������
	 * @return ����ֵ bool 
	 */
	virtual bool Start(const size_t num = 1);

protected:

	//�¼���������
	BlockingDeque<Event> m_input_blkque;

	//��ǰ����״̬
	state_t m_state;

	//����ÿ�δ��������������ȡ������¼���
	size_t m_input_size;

	//�����߳���
	size_t m_thread_num;
	boost::thread_group m_thread_group;

	/**
	 * @brief  �����̵߳��õ���ѭ��������ѭ�����¼����������ȡ�¼�������״̬��һֱ���У����᷵��
	 * @return ����ֵ 0 ����ʾ���������� ����ֵ ����
	 */
	virtual int run();

	/**
	 * @brief  �¼�������  ������ʵ��
	 * @return ����ֵ ��
	 */
	virtual void on_event(Event & e) = 0;

	//���̰߳�ȫ����m_thread_num ����1ʱ��ֹʹ��
	//CallID������
	CallID m_next_cid;
	set<CallID> m_using_cid_set;

	CallID generate_call_id();



	void release_call_id(const CallID & cid);
};



#endif 
