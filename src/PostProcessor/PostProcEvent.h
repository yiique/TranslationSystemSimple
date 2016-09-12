#ifndef POST_PROC_EVENT_H
#define POST_PROC_EVENT_H

//MAIN
#include "EventEngine/EventData.h"
#include "TransText/TransText.h"
#include "Common/ErrorCode.h"

class PostProcReq
	: public EventData
{
public:
	PostProcReq(const CallID & cid, 
			   TransText * p_trans_text): EventData(cid),
										  mp_trans_text(p_trans_text)
	{
	}

	~PostProcReq(void)
	{
	}

	const EDType GetType() const
	{
		return EDTYPE_POSTPROC_REQ;
	}

	TransText * GetTransText()
	{
		return mp_trans_text;
	};

private:
	TransText * mp_trans_text;
};


class PostProcRes
	: public EventData
{
public:
	PostProcRes(const CallID & cid, 
			   TransText * p_trans_text,
			   const int result = SUCCESS): EventData(cid),
											mp_trans_text(p_trans_text),
								            m_result(result) 
	{}

	~PostProcRes(void)
	{
	}

	const EDType GetType() const
	{
		return EDTYPE_POSTPROC_RES;
	}

	TransText * GetTransText()
	{
		return mp_trans_text;
	};

	int GetResult() const
	{ 
		return m_result;
	};

	void SetResult(const int result)
	{
		m_result = result;
	};

private:
	
	TransText * mp_trans_text;
	int m_result;
};

#endif //POST_PROC_EVENT_H
