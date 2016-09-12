#include "LogPrinter.h"


//唯一静态实例
LogPrinter LogPrinter::ms_log_instance;



LogPrinter::LogPrinter(void): m_is_start(false), m_log_cnt(0)
{
}


LogPrinter::~LogPrinter(void)
{
	this->m_log_file.close();
}


LogPrinter * LogPrinter::GetInstance()
{
	return (&ms_log_instance);
}

bool LogPrinter::Start(bool is_std_io)
{
	if(m_is_start)
	{
		return true;
	}
	
	m_is_std_io = is_std_io;

	//打开文件
	string log_name("log");
	time_t time_stamp;
	if( -1 == (time_stamp = time((time_t*)NULL)) )
	{
		cerr << "Get time for log name failed , err = " << strerror(errno) << endl;
		log_name += ".default.txt";
	}else
	{
		log_name += ".";
		log_name += num_2_str(time_stamp);
		log_name += ".txt";
	}

	m_log_file.clear();
	m_log_file.open(log_name.c_str());
	if(!m_log_file.is_open())
	{
		cout << "ERROR: LogPrinter::Start() open failed. err = " << strerror(errno) << endl;
		return false;
	}

	bool res = EventEngine::Start();
	if(false == res)
	{
		cout << "ERROR: LogPrinter::Start() call base event engine start failed." << endl;
		return false;
	}

	m_is_start = true;
	return true;
}

void LogPrinter::on_event(Event & e)
{

	if(!e._p_edata)
	{
		return;
	}

	//转换为LogData
	LogData * p_log_data = dynamic_cast<LogData*>(e._p_edata);

	if(p_log_data)
	{
		//转换成功
		print_data(*p_log_data);
	}

	//处理完毕 销毁
	delete p_log_data;
}

void LogPrinter::print_data(const LogData & data)
{
	unsigned long curr_cnt = m_log_cnt;
	m_log_cnt++;
	
	//输出到文件
	m_log_file << "[" << curr_cnt << "]-[ " << boost::posix_time::to_simple_string(data.GetTimeStamp()) << " ]-[ " << data.GetThreadID() << " ] " << data.GetContent() << endl; 

	//输出到标准输入输出
	if(m_is_std_io)
	{
		cout << "[" << curr_cnt << "]-[ " << boost::posix_time::to_simple_string(data.GetTimeStamp()) << " ]-[ " << data.GetThreadID() << " ] " << data.GetContent() << endl; 
	}
}
