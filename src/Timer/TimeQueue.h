#ifndef TIME_QUEUE_H
#define TIME_QUEUE_H


//MAIN
#include "Log/LogStream.h"

#include <assert.h>

#ifdef WINDOWS
	#include <time.h>
#else //LINUX
	#include <sys/time.h>
#endif

//STL
#include <map>
#include <iostream>
#include <utility>
#include <vector>
using namespace std;


template <typename KEY_T, typename VAL_T>
class TimeQueue
{
private:

	class TimeNode
	{
	public:
		TimeNode(void): _in_list(false), _pre(NULL), _next(NULL)  {};
		TimeNode(const KEY_T & key, 
				 const VAL_T & val,
				 const time_t   time_stamp,
				 TimeNode * pre = NULL,
				 TimeNode * next = NULL) : _in_list(false), _key(key), _val(val), _time_stamp(time_stamp), _pre(pre), _next(next)
		{};

		~TimeNode(void) {};
	
		bool _in_list;
		KEY_T _key;
		VAL_T _val;
		time_t   _time_stamp;

		
	
		TimeNode * _pre;
		TimeNode * _next;
	};

public:
	TimeQueue(int timeout_s): m_timeout_s(timeout_s)
	{
		mp_time_head = new TimeNode();
		mp_time_tail = new TimeNode();

		mp_time_head->_next = mp_time_tail;
		mp_time_tail->_pre = mp_time_head;

		mp_time_head->_in_list = true;
		mp_time_tail->_in_list = true;
	};

	~TimeQueue(void) 
	{

		//node可能不全在list中，固应该从map中删除
		typename map<KEY_T, TimeNode*>::iterator iter = m_time_map.begin();
		for(; iter != m_time_map.end(); ++iter)
		{
			delete iter->second;
		}

		//再删除头尾节点
		delete mp_time_head;
		delete mp_time_tail;
	};

	void Append(const KEY_T & key, const VAL_T & val);
	void Remove(const KEY_T & key);
	void Refresh(const KEY_T & key);
	void Check(vector<VAL_T> & timeout_vec);  //超时后不会自动将节点从timequeue中删除， 而仅仅是不计入超时队列，需要手动调用remove

	void Print() const;

private:

	//禁止拷贝
	TimeQueue(const TimeQueue & rsh) {};
	TimeQueue & operator = (const TimeQueue & rsh) {};

	//时间节点链表  按时间戳从小到大有序
	TimeNode * mp_time_head;
	TimeNode * mp_time_tail;

	map<KEY_T, TimeNode*> m_time_map;

	int m_timeout_s;


	void reorder_with_time(TimeNode * node);


public:
	static void TestCase();
};

template <typename KEY_T, typename VAL_T>
void TimeQueue<KEY_T, VAL_T>::TestCase()
{
	TimeQueue<int,int> tq(1);

    tq.Append(1, 1);
    tq.Append(2, 2);
    tq.Append(3, 3);

    tq.Print();

    sleep(1);
    tq.Refresh(2);
    tq.Remove(3);
    tq.Print();
    sleep(1);

    vector<int> timeout_vec;
    tq.Check(timeout_vec);

    ldbg1 << "Timeout List:" << endl;
    for(size_t i=0; i<timeout_vec.size(); ++i)
    {
        ldbg1 << "Timeout[" << i << "] = " << timeout_vec[i] << endl; 
    }

    tq.Print();
}

template <typename KEY_T, typename VAL_T>
void TimeQueue<KEY_T, VAL_T>::Append(const KEY_T & key, const VAL_T & val)
{
	//lout << "Append key = " << key << endl;

	//生成时间戳
	time_t time_stamp;
	if( -1 == (time_stamp = time((time_t*)NULL)) )
	{
		lerr << "Get time failed, err = " << strerror(errno) << endl;

		return;
	}
	
	TimeNode * node = new TimeNode(key, val, time_stamp, mp_time_tail->_pre, mp_time_tail);

	//加入到map中
	pair<typename map<KEY_T, TimeNode*>::iterator, bool> res = m_time_map.insert(make_pair(key, node));

	if( false == res.second )
	{
		lwrn << "Can't append a same key in time queue. key = " << key << endl;
		delete node;

		return;
	}

	//加入到时间链表中 后生成的节点时间戳比如较大 排在后面
	mp_time_tail->_pre->_next = node;
	mp_time_tail->_pre = node;
	
	node->_in_list = true;

	//lout << "debug" << endl;
	//Print();
}

template <typename KEY_T, typename VAL_T>
void TimeQueue<KEY_T, VAL_T>::Print() const
{
	ldbg1 << "============Print TimeQueue============" << endl;

	typename map<KEY_T, TimeNode*>::const_iterator iter = m_time_map.begin();

	for(; iter != m_time_map.end(); ++iter)
	{
		TimeNode * node = iter->second;
		ldbg1 << "  TimeNode: key[" << iter->first << "] node_key[" << node->_key << "] val[" << node->_val << "] time_stamp[" << node->_time_stamp << "]" << endl; 
		node = node->_next;
	}

	ldbg1 << "=======================================" << endl << endl;

}

template <typename KEY_T, typename VAL_T>
void TimeQueue<KEY_T, VAL_T>::Remove(const KEY_T & key)
{
	//lout << "Remove time key = [" << key << "]" << endl;
	//Print();

	typename map<KEY_T, TimeNode*>::iterator iter = m_time_map.find(key);

	if(iter == m_time_map.end())
	{
		lwrn << "Can't find key when remove. key = " << key << endl;
		return;
	}

	TimeNode * node = iter->second;
	
	m_time_map.erase(iter);

	assert(node);

	//Print();
	if(true == node->_in_list)
	{	
		node->_pre->_next = node->_next;
		node->_next->_pre = node->_pre;
	}

	delete node;

	//lout << "debug" << endl;
	
	
}


template <typename KEY_T, typename VAL_T>
void TimeQueue<KEY_T, VAL_T>::Refresh(const KEY_T & key)
{

	typename map<KEY_T, TimeNode*>::iterator iter = m_time_map.find(key);

	if(iter == m_time_map.end())
	{
		lwrn << "Can't find key when remove. key = " << key << endl;
		return;
	}

	TimeNode * node = iter->second;
	assert(node);

	//生成时间戳
	time_t time_stamp;
	if( -1 == (time_stamp = time((time_t*)NULL)) )
	{
		lerr << "Get time failed, err = " << strerror(errno) << endl;
		return;
	}

	node->_time_stamp = time_stamp;

	//根据节点的时间戳重新排序
	reorder_with_time(node);

	//lout << "debug" << endl;
	//Print();
}

template <typename KEY_T, typename VAL_T>
void TimeQueue<KEY_T, VAL_T>::Check(vector<VAL_T> & timeout_vec)
{


	//生成时间戳
	time_t curr_time_stamp;
	if( -1 == (curr_time_stamp = time((time_t*)NULL)) )
	{
		lerr << "Get time failed, err = " << strerror(errno) << endl;
		return;
	}

	//找出超时的时间节点并删除
	TimeNode * node = mp_time_head->_next;
	TimeNode * timeout_end_node = NULL;

	while(node != mp_time_tail)
	{
		if( (curr_time_stamp - node->_time_stamp) > m_timeout_s )
		{
			timeout_end_node = node;
			node = node->_next;
		}else
		{
			//当前节点不超时 则后面的节点也不超时
			break;
		}
	}

	if(NULL != timeout_end_node)
	{
		//删除并返回所有超时节点
		TimeNode * timeout_beg_node = mp_time_head->_next;

		mp_time_head->_next = timeout_end_node->_next;
		timeout_end_node->_next->_pre = mp_time_head;

		timeout_beg_node->_pre = NULL;
		timeout_end_node->_next = NULL;

		TimeNode * node = timeout_beg_node;

		while(node)
		{
			timeout_vec.push_back(node->_val);

			//超时后不会自动将节点从timequeue中删除， 而仅仅是不计入超时队列，需要手动调用remove
			node->_in_list = false;
			node = node->_next; //原node节点保存在map中

		}// end while

	}// end if

	//lout << "debug" << endl;
	//Print();
}

template <typename KEY_T, typename VAL_T>
void TimeQueue<KEY_T, VAL_T>::reorder_with_time(TimeNode * node)
{
	if(!node || !node->_next) return;

	TimeNode * back_node = node;

	while(back_node != mp_time_tail)
	{
		if(node->_time_stamp < back_node->_time_stamp)
			break;

		back_node = back_node->_next;
	}

	if(node != back_node)
	{
		//将node移动到back_node的前面

		//step 1 删除node
		node->_pre->_next = node->_next;
		node->_next->_pre = node->_pre;

		//step 2 插入新位置
		back_node->_pre->_next = node;
		node->_pre = back_node->_pre;
		node->_next = back_node;
		back_node->_pre = node;
	}

	//lout << "debug" << endl;
	//Print();
}

#endif //TIME_QUEUE_H
