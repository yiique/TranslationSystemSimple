#include "PreProcReq.h"


PreProcReq::PreProcReq(const CallID & cid,  
			           TransText * p_trans_text): mp_trans_text(p_trans_text)
{
	m_call_id = cid;
}


PreProcReq::~PreProcReq(void)
{
}

const EDType PreProcReq::GetType() const 
{
	return EDTYPE_PREPROC_REQ;
}
