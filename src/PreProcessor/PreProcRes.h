#ifndef PRE_PROC_RES_H
#define PRE_PROC_RES_H

//MAIN
#include "EventEngine/EventData.h"
#include "TransText/TransText.h"

class PreProcRes
	: public EventData
{
public:
	PreProcRes(const CallID & cid, 
			   TransText * p_trans_text,
			   const int result = 0);

	~PreProcRes(void);

	const EDType GetType() const;

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

#endif //PRE_PROC_RES_H
