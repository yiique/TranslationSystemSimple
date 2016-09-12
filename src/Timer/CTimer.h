#ifndef CTIMER_H
#define CTIMER_H


//定时器类  向所有注册的EventEngine发送定时事件

//BOOST
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

//MAIN
#include "EventEngine/EventEngine.h"
#include "Log/LogStream.h"
#include "TimeEvent.h"

//STL
#include <vector>
using namespace std;

#define DEF_SLEEP_TIME 5

typedef struct
{
	int _id; //debug
	EventEngine * _pCaller;
	size_t        _frequency;
	int           _left_time;
	unsigned long _time_seq;

} TimeCell;

class CTimer
{
public:
	
	~CTimer(void);

	static CTimer * GetInstance();

	bool Start(void);

	//不可重入
	bool RegTimerBeforeStart(EventEngine * p_caller, size_t frequency);

private:
	
	//单态
	CTimer(void);
	static CTimer ms_timer;

	bool m_is_start;
	boost::thread_group m_thread_group;

	vector<TimeCell*> m_reg_vec;

	int timer_run();

	int time_alarm();

	time_t m_last_alarm_time;
};



#endif //CTIMER_H

