#include "PreProcRes.h"


PreProcRes::PreProcRes(const CallID & cid, 
			   TransText * p_trans_text,
			   const int result): mp_trans_text(p_trans_text),
								  m_result(result)
{
	m_call_id = cid;
}


PreProcRes::~PreProcRes(void)
{
}

const EDType PreProcRes::GetType() const 
{
	return EDTYPE_PREPROC_RES;
}
