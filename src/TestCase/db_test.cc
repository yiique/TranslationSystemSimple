#include "DataBase/DBOperation.h"
#include "Log/LogStream.h"
#include <iostream>
using namespace std;

int main()
{
	//Init Data Base .
	lout << "MySQLMan Start..." << endl;
	if( !MySQLMan::InitDBOperation("test", "127.0.0.1", "root", "123654", 3306))
	{
		lerr<< "MySQLMan Start failed." << endl;
		exit(1);
	}
	lout << "MySQLMan Start Success." << endl;

	return 0;
}