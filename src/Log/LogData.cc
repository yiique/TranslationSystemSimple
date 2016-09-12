#include "LogData.h"

LogData::LogData(boost::thread::id log_tid, 
				 const string & log_content,
				 const boost::posix_time::ptime & time_stamp): m_log_tid(log_tid),
										   m_log_content(log_content),
										   m_time_stamp(time_stamp)
{
}

LogData::~LogData(void)
{
}


const EDType LogData::GetType() const
{
	return EDTYPE_LOG_DATA;
}

