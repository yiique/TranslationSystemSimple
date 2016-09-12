#include "LogStream.h"


LogStream LogStream::CreateLogStream()
{
	LogStream log;
	return log;
}


LogStream::LogStream(void)
{
}

LogStream::~LogStream(void)
{
	//输出缓存中的剩余数据
	if(m_sstream.str() != "")
	{
		output();
	}
}

void LogStream::output()
{
	//获取待输出数据
	boost::thread::id tid = boost::this_thread::get_id();
	boost::posix_time::ptime curr_time_stamp = boost::posix_time::second_clock::local_time();
	
	LogData * p_data = new LogData(tid, m_sstream.str(), curr_time_stamp);
    m_sstream.str("");

	//发送给LogPrinter
	Event e(p_data);
	LogPrinter::GetInstance()->PostEvent(e);
}

LogStream & LogStream::operator << (EndlType _p)
{
	output();
	return *this;
}

LogStream & LogStream::operator << (const DomainType & domain)
{
	m_sstream << "[ " << domain.first << " : " << domain.second.first << " -> " << domain.second.second << " ] " ;
	return *this;
}

LogStream & LogStream::operator << (const ServerAddrType & addr)
{
	m_sstream << "[ " << addr.first << " : " << addr.second << " ]";
	return *this;
}

LogStream::LogStream(const LogStream & rsh)
{
	if( &rsh != this)
	{
		m_sstream.str("");
		m_sstream.str(rsh.m_sstream.str());
	}
}

LogStream & LogStream::operator = (const LogStream &rsh)
{
	if( &rsh != this)
	{
		m_sstream.str("");
		m_sstream.str(rsh.m_sstream.str());
	}

	return *this;
}


