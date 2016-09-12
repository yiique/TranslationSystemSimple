#ifndef OLT_SUBMIT_REQ_H
#define OLT_SUBMIT_REQ_H

//MAIN
#include "EventEngine/EventData.h"
#include "TransText/TransText.h"

//STL
#include <vector>
using namespace std;

class OltSubmitReq
	: public EventData
{
public:
	OltSubmitReq(const CallID & cid,
				 const vector<TextID> & textid_vec,
				 const string & domain_name,
				 const string & src_language,
				 const string & tgt_language,
				 const string & usr_id); 

	~OltSubmitReq(void);

	const EDType GetType() const;

	const vector<TextID> & GetTextIDVec() const
	{
		return m_textid_vec;
	};

	const string & GetDomainName() const
	{
		return m_domain_name;
	};

	const string & GetSrcLanguage() const
	{
		return m_src_language;
	};

	const string & GetTgtLanguage() const
	{
		return m_tgt_language;
	};

	const string & GetUsrID() const
	{
		return m_usr_id;
	};


private:

	vector<TextID> m_textid_vec;//文件在数据库中的唯一ID号

	string m_domain_name;
	string m_src_language;
	string m_tgt_language;
	string m_usr_id;

	
};

#endif //OLT_SUBMIT_REQ_H


