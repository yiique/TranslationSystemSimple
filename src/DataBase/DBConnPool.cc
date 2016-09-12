#include "DBConnPool.h"
#include "Log/LogStream.h"

DBConnPool DBConnPool::ms_instance;

DBConnPool * DBConnPool::GetInstance()
{
	return &ms_instance;
}


int DBConnPool::Runable(RunFun run_fun)
{
	mysqlpp::Connection * p_conn = m_conn_deq.PopFront();

	check_connect(p_conn);

	mysqlpp::Query query = p_conn->query();
	int ret = run_fun(query);

	m_conn_deq.PushBack(p_conn);

	return ret;
}


bool DBConnPool::check_connect(mysqlpp::Connection * p_conn)
{
	while(!p_conn->ping())
	{
		lwrn << "Ping database failed. Reconnecting..." << endl;
		try
		{
			p_conn->connect(m_db_name.c_str(), m_db_ip.c_str(), m_db_usr.c_str(), m_db_pwd.c_str(), m_db_port);
		}catch(mysqlpp::Exception e)
		{
			lerr << "Failed with mysql exception : " << e.what() << endl;
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
	}

	return true;
}

bool DBConnPool::Initialize(const string & db_name,
						    const string & db_ip,
						    const string & db_usr,
						    const string & db_pwd,
						    const int db_port,
							const int db_conn_num)
{
	m_db_name = db_name;
	m_db_ip = db_ip;
	m_db_usr = db_usr;
	m_db_pwd = db_pwd;
	m_db_port = db_port;
	m_db_conn_num = db_conn_num;

	if(m_db_conn_num <= 0)
	{

		lerr << "DB connetion pool number can't set to : " << m_db_conn_num << endl;
		return false;
	}

	try
	{

		for(int i=0; i<m_db_conn_num; ++i)
		{
			mysqlpp::Connection * p_conn = new mysqlpp::Connection(true);

			p_conn->connect(m_db_name.c_str(), m_db_ip.c_str(), m_db_usr.c_str(), m_db_pwd.c_str(), m_db_port);

			m_conn_deq.PushBack(p_conn);

		}

	}catch(mysqlpp::Exception e)
	{
		lerr << "Connect to database failed : " << e.what() << endl;
		return false;
	}

	return true;
}
