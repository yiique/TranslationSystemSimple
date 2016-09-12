#ifndef PRE_PROC_REQ_H
#define PRE_PROC_REQ_H

//MAIN
#include "EventEngine/EventData.h"
#include "TransText/TransText.h"

class PreProcReq
	: public EventData
{
public:
	PreProcReq(const CallID & cid, 
			   TransText * p_trans_text);

	~PreProcReq(void);

	const EDType GetType() const;

	TransText * GetTransText()
	{
		return mp_trans_text;
	};

private:
	TransText * mp_trans_text;
};

#endif //PRE_PROC_REQ_H

