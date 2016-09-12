//MAIN
#include "Common/RWLock.h"

//BOOST
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

//STL
#include <iostream>
using namespace std;

RWLock rw_lock;

static size_t flag = 0;

void read_thr(const size_t id, const size_t sec)
{
	cout << "thr " << id << " start." << endl;
	ReadLockGuard guard(rw_lock);

	cout << "thr " << id << " read in..." << endl;
	boost::this_thread::sleep(boost::posix_time::seconds(sec));
	cout << "thr " << id << " read out, sleep time " << sec << endl;
}


void write_thr(const size_t id, const size_t sec)
{
	cout << "thr " << id << " start." << endl;
	WriteLockGuard guard(rw_lock);

	cout << "thr " << id << " write in..." << endl;
	boost::this_thread::sleep(boost::posix_time::seconds(sec));
	cout << "thr " << id << " write out, sleep time " << sec << endl;
}

void read_thr_mi(const size_t id, const size_t mi_sec, const size_t loop)
{
	cout << "thr " << id << " start." << endl;

	for(size_t i=0; i<loop; ++i)
	{
		ReadLockGuard guard(rw_lock);

		size_t curr_flag = flag;

		boost::this_thread::sleep(boost::posix_time::milliseconds(mi_sec));

		if(curr_flag != flag)
		{
			cerr << "Error! read curr_flag = " << curr_flag << " flag = " << flag << endl;
			exit(0);
		}

		cout << "thr id " << id << " flag = " << flag << endl;
	}

}


void write_thr_mi(const size_t id, const size_t mi_sec, const size_t loop)
{
	cout << "thr " << id << " start." << endl;

	for(size_t i=0; i<loop; ++i)
	{
		{
		WriteLockGuard guard(rw_lock);

		size_t curr_flag = flag;
		flag++;

		boost::this_thread::sleep(boost::posix_time::milliseconds(mi_sec));

		if(curr_flag + 1 != flag)
		{
			cerr << "Error! write curr_flag = " << curr_flag << " flag = " << flag << endl;
			exit(0);
		}
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(mi_sec));
	}

}
 

//TEST CASE 1: µ¥¶À²âÊÔ¶ÁËø

void test_case_1()
{
	cout << "Test 1 start : " << endl;
	cout << endl;

	boost::thread thrd1(boost::bind(&read_thr, 1, 5));

	boost::this_thread::sleep(boost::posix_time::seconds(1));

    boost::thread thrd2(boost::bind(&read_thr, 2, 1));

    thrd1.join();
    thrd2.join();

	cout << endl;
	cout << "Test 1 finish." << endl;

	cout << endl << endl;
    return;
}

//TEST CASE 2: µ¥¶À²âÊÔÐ´Ëø
void test_case_2()
{
	cout << "Test 2 start : " << endl;
	cout << endl;

	boost::thread thrd1(boost::bind(&write_thr, 1, 5));

	boost::this_thread::sleep(boost::posix_time::seconds(1));

    boost::thread thrd2(boost::bind(&write_thr, 2, 1));

    thrd1.join();
    thrd2.join();

	cout << endl;
	cout << "Test 2 finish." << endl;

	cout << endl << endl;
    return;
}

//TEST CASE 3: »ìºÏ²âÊÔ 
void test_case_3()
{
	cout << "Test 3 start : " << endl;
	cout << endl;

	boost::thread thrd1(boost::bind(&write_thr, 1, 5));

	boost::this_thread::sleep(boost::posix_time::seconds(1));

    boost::thread thrd2(boost::bind(&read_thr, 2, 1));

    thrd1.join();
    thrd2.join();

	cout << endl;
	cout << "Test 3 finish." << endl;

	cout << endl << endl;
    return;
}

//TEST CASE 4: »ìºÏ²âÊÔ 
void test_case_4()
{
	cout << "Test 4 start : " << endl;
	cout << endl;

	boost::thread thrd1(boost::bind(&read_thr, 1, 5));

	boost::this_thread::sleep(boost::posix_time::seconds(1));

    boost::thread thrd2(boost::bind(&write_thr, 2, 1));

    thrd1.join();
    thrd2.join();

	cout << endl;
	cout << "Test 4 finish." << endl;

	cout << endl << endl;
    return;
}

//TEST CASE 5: »ìºÏ²âÊÔ 
void test_case_5()
{
	cout << "Test 5 start : " << endl;
	cout << endl;

	boost::thread thrd1(boost::bind(&read_thr, 1, 5));

	boost::this_thread::sleep(boost::posix_time::seconds(1));

	boost::thread thrd2(boost::bind(&read_thr, 2, 2));

	boost::this_thread::sleep(boost::posix_time::seconds(1));

    boost::thread thrd3(boost::bind(&write_thr, 3, 5));

	boost::this_thread::sleep(boost::posix_time::seconds(1));

    boost::thread thrd4(boost::bind(&read_thr, 4, 1));

	boost::this_thread::sleep(boost::posix_time::seconds(1));

	boost::thread thrd5(boost::bind(&write_thr, 5, 1));

    thrd1.join();
    thrd2.join();
	thrd3.join();
    thrd4.join();
	thrd5.join();

	cout << endl;
	cout << "Test 5 finish." << endl;

	cout << endl << endl;
    return;
}


//TEST CASE 7: 
void test_case_7()
{
	cout << "Test 7 start : " << endl;
	cout << endl;

	size_t read_thread_num = 10;
	size_t write_thread_num = 1;

	boost::thread_group rtg;
	boost::thread_group wtg;
	
	for(size_t i=0; i<read_thread_num; ++i)
	{
		rtg.add_thread(new boost::thread(boost::bind(&read_thr_mi, i, 100, 100)));
	}
	
	for(size_t i=0; i<write_thread_num; ++i)
	{
		wtg.add_thread(new boost::thread(boost::bind(&write_thr_mi, i, 50, 100)));
	}
 
	rtg.join_all();
    wtg.join_all();

	cout << endl;
	cout << "Test 7 finish. flag = " << flag << endl;

	cout << endl << endl;
    return;
}

int main(int argc, char * argv[])
{
	/*test_case_1();
	test_case_2();
	test_case_3();
	test_case_4();
	test_case_5();*/

	test_case_7();

    return 0;
}
