
//BOOST
#include <boost/thread/thread.hpp>

#include "TransText/TimeLog.h"
#include <iostream>
using namespace std;



int main()
{
	Timer t;

	t.Start();

	for(size_t i=0; i<1000000000; ++i)
	{}

	//boost::this_thread::sleep(boost::posix_time::seconds(1));
	cout << t.Stop() << endl;

	return 0;

}
