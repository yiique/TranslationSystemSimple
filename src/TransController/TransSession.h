#ifndef TRANS_SESSION_H
#define TRANS_SESSION_H

//MAIN
#include "SessionManager/CSession.h"
#include "Common/BasicType.h"
#include "TransText/TransText.h"
#include "TinyXml/tinyxml.h"
#include "Common/MsgType.h"
#include "Common/f_utility.h"
#include "Log/LogStream.h"
#include "TransTask.h"


//STL
#include <string>
using namespace std;

#define RES_CODE_SUC "0"

class TransSession
	: public CSession
{
public:
	TransSession(const CallID & cid,
				 const int sockfd, 
				 trans_task_share_ptr sp_task);

	~TransSession(void);

	bool BuildSendPacket();
	bool ParseRecvPacekt();

	trans_task_share_ptr GetTask()
	{
		return m_sp_task;
	}

	const string & GetServerIP() const
	{
		return m_sp_task->sp_slave->GetInfo().ip;
	}

	int GetServerPort() const
	{
		return m_sp_task->sp_slave->GetInfo().port;
	}

	const string & GetServerRescode() const
	{
		return m_ser_rescode;
	}

	void SetErrTransTgt() //设置用于返回错误消息的tgt 主要用于判断错误超过重试次数的句子
	{
		m_sp_task->trans_tgt._tgt_str = "Notice: can't trans this sent.";
	}

	void SetTransEndTime()
	{
		#ifdef OUTPUT_TRANS_TIME

		struct timeval trans_end_time;
		gettimeofday(&trans_end_time,NULL);
		m_sp_task->trans_tgt._trans_time = (double)(trans_end_time.tv_sec - m_trans_beg_time.tv_sec) + ((double) (trans_end_time.tv_usec - m_trans_beg_time.tv_usec)) / 1000000;
		
		#endif
	}

private:
	
	trans_task_share_ptr m_sp_task;
	string m_ser_rescode;

	#ifdef OUTPUT_TRANS_TIME
	struct timeval m_trans_beg_time;
	#endif


};


#endif //TRANS_SESSION_H

