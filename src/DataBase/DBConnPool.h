#ifndef DB_CONN_POOL_H
#define DB_CONN_POOL_H

//MAIN
#include "EventEngine/BlockingDeque.h"

//BOOST
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

//MYSQLPP
#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

using namespace std;

typedef boost::function< int (mysqlpp::Query&) > RunFun;

class DBConnPool
	: private boost::noncopyable
{
public:
	~DBConnPool(void) {}

	static DBConnPool * GetInstance();

	bool Initialize(const string & db_name,
					const string & db_ip,
					const string & db_usr,
					const string & db_pwd,
					const int db_port = 3306,
					const int db_conn_num = 5);

	int Runable(RunFun run_fun);

private:
	static DBConnPool ms_instance;

	string m_db_name;
	string m_db_ip;
	string m_db_usr;
	string m_db_pwd;
	int    m_db_port;
	int    m_db_conn_num;

private:

	BlockingDeque<mysqlpp::Connection*> m_conn_deq;

	bool check_connect(mysqlpp::Connection * p_conn);
};

#endif //DB_CONN_POOL_H
