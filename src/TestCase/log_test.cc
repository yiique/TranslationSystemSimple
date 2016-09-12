#include "Log/LogPrinter.h"
#include "Log/LogStream.h"

int main()
{
	//先启动日志
	if(!LogPrinter::GetInstance()->Start())
	{
		cout << "LogPrinter init failed." << endl;
		exit(1);
	}

	cout << "log success" << endl;


	string x;

	while(cin >> x)
	{
		lout << x << endl;
	}

	return 0;
}
