#include "OltSubmitRes.h"

OltSubmitRes::OltSubmitRes(const CallID & cid, const vector<int> & result_vec): m_result_vec(result_vec)
{
	m_call_id = cid;
}

OltSubmitRes::~OltSubmitRes(void)
{
}


const EDType OltSubmitRes::GetType() const
{
	return EDTYPE_OLT_SUBMIT_REQ;
}

