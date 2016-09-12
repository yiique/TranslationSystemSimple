#include "OltSubmitReq.h"

OltSubmitReq::OltSubmitReq(const CallID & cid,
						   const vector<TextID> & textid_vec,
						   const string & domain_name,
						   const string & src_language,
						   const string & tgt_language,
						   const string & usr_id): m_textid_vec(textid_vec),
												   m_domain_name(domain_name),
												   m_src_language(src_language),
												   m_tgt_language(tgt_language),
												   m_usr_id(usr_id)
{
	this->m_call_id = cid;
}

OltSubmitReq::~OltSubmitReq(void)
{
}


const EDType OltSubmitReq::GetType() const
{
	return EDTYPE_OLT_SUBMIT_REQ;
}

