#include "TimeLog.h"
#include <iostream>
using namespace std;

TimeLog::TimeLog(const string & name, const double value) : m_name(name), time_value(value)
{
}

TimeLog::~TimeLog()
{
}

TimeLog & TimeLog::operator [] (const string & name)
{
	pair<map<string, time_log_share_ptr>::iterator, bool> res = m_time_map.insert(make_pair(name, time_log_share_ptr(new TimeLog(name))));

	if(true == res.second)
		m_seq_vec.push_back(res.first->second);

	return *(res.first->second);
}

TimeLog & TimeLog::operator [] (const size_t i)
{
	pair<map<string, time_log_share_ptr>::iterator, bool> res = m_time_map.insert(make_pair(num_2_str(i), time_log_share_ptr(new TimeLog(num_2_str(i)))));

	if(true == res.second)
		m_seq_vec.push_back(res.first->second);

	return *(res.first->second);
}

void TimeLog::Start()
{
	m_timer.Start();
}

void TimeLog::Stop()
{
	time_value = m_timer.Stop();
}

string TimeLog::Serialize()
{
	
	re_count_time();
	
	return this->serialize(0);
}

string TimeLog::serialize(const size_t deep)
{
	string format_blank(deep*2, ' ');
	stringstream ss;

	ss << format_blank << "<" << m_name << " time=\"" << time_value << "\"" ;

	if(m_seq_vec.size() == 0)
	{
		ss << " />" << endl;
	}else
	{
		ss << " >" << endl;

		for(size_t i=0; i<m_seq_vec.size(); ++i)
		{
			ss << m_seq_vec[i]->serialize(deep+1);
		}

		ss << format_blank << "</" << m_name << ">" << endl;
	}

	return ss.str();

}

void TimeLog::re_count_time()
{

	bool is_re_count_root = (time_value == -1) ? true : false;
	
	if(is_re_count_root)
		time_value = 0;
	
	map<string, time_log_share_ptr>::iterator iter = m_time_map.begin();
	
	for(; iter != m_time_map.end(); ++iter)
	{
		iter->second->re_count_time();

		if(is_re_count_root)
			time_value += iter->second->time_value;
	}
}
