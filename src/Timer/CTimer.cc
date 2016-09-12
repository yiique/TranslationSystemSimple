#include "CTimer.h"

CTimer CTimer::ms_timer;

CTimer * CTimer::GetInstance()
{
	return &(CTimer::ms_timer);
}

CTimer::CTimer(void)
{
}


CTimer::~CTimer(void)
{
	for(size_t i=0; i<m_reg_vec.size(); ++i)
	{
		delete m_reg_vec[i];
	}
}


int CTimer::timer_run()
{
	bool loop = true;

	m_last_alarm_time = time( (time_t*) NULL );
	
	while(loop)
	{
		int sleep_time = time_alarm();

		if( sleep_time > 0 )
			boost::this_thread::sleep(boost::posix_time::seconds(sleep_time));
	}

	return 0;
}


int CTimer::time_alarm()
{
	if( m_reg_vec.size() == 0 )
		return DEF_SLEEP_TIME;

	//获得当前时间
	time_t curr_time = time( (time_t*) NULL );

	int spend_time = (int) (curr_time - m_last_alarm_time);
	m_last_alarm_time = curr_time; //重置last时间

	if( spend_time < 0 )
	{
		lerr << "ERROR: spend_time less than 0. curr_time = " << curr_time << "   last_alarm_time = " << m_last_alarm_time << endl;
		return DEF_SLEEP_TIME;
	}

	//遍历列表
	int sleep_time = -1;
	
	for(size_t i=0; i<m_reg_vec.size(); ++i)
	{
		TimeCell & tcell = *(m_reg_vec[i]);

		tcell._left_time -= spend_time;

		if( tcell._left_time <= 0 )
		{
			//通知
			//lout << "DEBUG : ALARM CALLER " << tcell._id << endl;
			TimeEvent * p_time_event = new TimeEvent(tcell._time_seq++);

			Event e(p_time_event, NULL, EVENT_PRIORITY_HIGH); //使用高优先级
			tcell._pCaller->PostEvent(e);

			tcell._left_time = tcell._frequency;
		}

		if( -1 == sleep_time || tcell._left_time < sleep_time )
		{
			sleep_time = tcell._left_time;
		}
	}

	return sleep_time;
}


bool CTimer::Start(void)
{
	if( true == m_is_start )
		return true;

	m_thread_group.create_thread(boost::bind(&CTimer::timer_run, this));

	m_is_start = true;

	return true;
}


bool CTimer::RegTimerBeforeStart(EventEngine * pCaller, size_t frequency)
{	
	if( 0 == frequency || !pCaller)
		return false;

	TimeCell * pCell = new TimeCell;
	pCell->_pCaller = pCaller;
	pCell->_frequency = frequency;
	pCell->_left_time = frequency;
	pCell->_time_seq = 1;

	m_reg_vec.push_back(pCell);

	return true;

}

