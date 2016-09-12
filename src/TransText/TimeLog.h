#ifndef TIME_LOG_H
#define TIME_LOG_H


//BOOST
#include <boost/timer.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

//MAIN
#include "Common/f_utility.h"

//STL
#include <utility>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using namespace std;


class Timer
{
public:
	Timer(void) {}
	~Timer(void) {}

	void Start()
	{
		m_timer.restart();
	}

	double Stop()
	{
		return m_timer.elapsed();
	}

private:

	boost::timer m_timer;
};

class TimeLog;

typedef boost::shared_ptr<TimeLog> time_log_share_ptr;

class TimeLog
	: public boost::enable_shared_from_this<TimeLog>, 
	  private boost::noncopyable
{

public:

	TimeLog(const string & name = "TimeLog", const double value = -1);
	~TimeLog();

public:

	double time_value;

	TimeLog & operator [] (const string & name);

	TimeLog & operator [] (const size_t i);

	string Serialize();

	void Start();
	void Stop();

private:

	string serialize(const size_t deep);

	void re_count_time();

	Timer m_timer;
	string m_name;
	map<string, time_log_share_ptr> m_time_map;
	vector<time_log_share_ptr> m_seq_vec;

};



#define T_TOTAL                  "T_TOTAL"

#define T_TEXT_FORMAT            "T_TEXT_FORMAT"

#define T_PER_PROCESS            "T_PER_PROCESS"
#define T_PRE_PROCESS_TEXT       "T_PRE_PROCESS_TEXT"
#define T_PER_PROCESS_SPLITE     "T_PER_PROCESS_SPLITE"
#define T_PER_PROCESS_SENT       "T_PER_PROCESS_SENT"

#define T_TRANSLATE              "T_TRANSLATE"
#define T_TRANSLATE_QUEUE        "T_TRANSLATE_QUEUE"
#define T_TRANSLATE_REAL         "T_TRANSLATE_REAL"
#define T_TRANSLATE_REAL_DECODE  "T_TRANSLATE_REAL_DECODE"

#define T_POST_PROCESS           "T_POST_PROCESS"
#define T_POST_PROCESS_SENT           "T_POST_PROCESS_SENT"

#endif //TIME_LOG_H

