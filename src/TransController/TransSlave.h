#ifndef TRANS_SLAVE_H
#define TRANS_SLAVE_H

//BOOST
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

//MAIN
#include "Common/BasicType.h"
#include "Log/LogStream.h"

//STL
#include <string>
using namespace std;

class TransSlaveInfo
{
public:

	string     slave_uuid;
	DomainType domain;
	string ip;
	int port;
	size_t concurrent_num;
};

class TransSlave
	:private boost::noncopyable
{
public:

	TransSlave(const TransSlaveInfo & slave_info) : m_slave_info(slave_info), m_idel_num(slave_info.concurrent_num)  { }

	const TransSlaveInfo & GetInfo() const 
	{
		return m_slave_info;
	}

	void Refresh(const TransSlaveInfo & slave_info)
	{
		//TODO
		return;
	}

	size_t GetIdelNum() const
	{
		return m_idel_num;
	}

	void ReduceIdelNum()
	{
		assert(m_idel_num > 0);

		--m_idel_num;
	}

	void IncreaseIdelNum()
	{
		++m_idel_num;
	}

private:
	TransSlaveInfo m_slave_info;

	size_t m_idel_num;
};

typedef boost::shared_ptr<TransSlave> trans_slave_share_ptr;

#endif //TRANS_SLAVE_H
