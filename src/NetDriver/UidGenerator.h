#ifndef UID_GENERATOR_H
#define UID_GENERATOR_H


//BOOST
#include <boost/noncopyable.hpp>

//MAIN
#include "Common/BaseMutex.h"

//STL
#include <set>
using namespace std;

class UidGenerator
	: private boost::noncopyable
{
public:

	UidGenerator(void): m_last_id(0) {}
	~UidGenerator(void) {}

	int Generate()
	{
		MutexLockGuard guard(m_lock);

		int id = 0;

		do
		{
			id = ++m_last_id;

		}while(!m_used_id_set.insert(id).second || id < 0);

		return id;
	}

	void Recycle(const int id)
	{
		MutexLockGuard guard(m_lock);
		m_used_id_set.erase(id);
	}

private:
	MutexLock m_lock;

	int m_last_id;
	set<int> m_used_id_set;
};

#endif //UID_GENERATOR_H
