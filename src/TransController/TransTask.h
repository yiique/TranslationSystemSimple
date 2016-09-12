#ifndef TRANS_TASK_H
#define TRANS_TASK_H

//BOOST
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

//MAIN
#include "TransSlave.h"
#include "TransText/TransText.h"

//STL
using namespace std;

class TransCore;
class UsrControlBlock;

class TransTask
{
public:
	TransTask(const TransSrc & _trans_src, 
			  const SubLanguageType _sub_src_language, 
			  boost::shared_ptr<UsrControlBlock> _sp_ucb): sp_ucb(_sp_ucb),
														   sub_src_language(_sub_src_language),
														   trans_src(_trans_src),
														   trans_tgt(_trans_src._text_id, _trans_src._cell_id) {}


	const TextID & GetTextID() const
	{
		return trans_src._text_id;
	}

public:

	const TransSrc trans_src;
	const SubLanguageType sub_src_language;
	TransTgt trans_tgt;

	boost::shared_ptr<TransSlave> sp_slave;
	boost::shared_ptr<TransCore> sp_trans_core;
	boost::shared_ptr<UsrControlBlock> sp_ucb;
};

typedef boost::shared_ptr<TransTask> trans_task_share_ptr;

#endif //TRANS_TASK_H
