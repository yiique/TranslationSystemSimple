//BOOST
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

//STL
#include <iostream>
#include <string>
using namespace std;

//FULCRUM
#include "EventEngine/BlockingDeque.h"

BlockingDeque<int> blq;

boost::mutex mutex;

void funtest(int id)
{
    for(size_t i=0; i<10000; ++i)
    {
        int elem = blq.PopFront();

        boost::lock_guard<boost::mutex> lock(mutex);
        cout << "Thread " << id << " get : " << elem << endl;
    }
}

int main(int argc, char *argv[])
{
    boost::thread_group tgroup;
    for(size_t i=0; i<1; ++i)
    {
        tgroup.create_thread(boost::bind(&funtest, i));
        //boost::thread t1( boost::bind(&funtest, 1) );
    }

    for(size_t i=0; i<100; ++i)
    {
        blq.PushFront(i);
    }

    tgroup.join_all();

    return 0;
}
