#ifndef TRANS_CONTROL_EVENT_H
#define TRANS_CONTROL_EVENT_H

//MAIN
#include "Common/BasicType.h"
#include "EventEngine/EventData.h"
#include "TransText/TransText.h"
#include "TransSlave.h"

class SlavePulseReq
	: public EventData
{
public:
	SlavePulseReq(const CallID & cid, const TransSlaveInfo & slave_info) : EventData(cid), m_slave_info(slave_info) {}
	~SlavePulseReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_SLAVE_PULSE_REQ;
	}
	
	const TransSlaveInfo & GetSlaveInfo() const 
	{
		return m_slave_info;
	}

private:
	TransSlaveInfo m_slave_info;
};

class SlavePulseRes
	: public EventData
{
public:
	SlavePulseRes(const CallID & cid) : EventData(cid) {}
	~SlavePulseRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_SLAVE_PULSE_RES;
	}

	const int GetResult() const
	{
		return m_result;
	}

	void SetResult(const int result)
	{
		m_result = result;
	}

private:
	int m_result;
};

class TransSubmitReq
	: public EventData
{
public:
	TransSubmitReq(TransText * p_trans_text): mp_trans_text(p_trans_text) {}
	~TransSubmitReq() {}

	const EDType GetType() const
	{
		return EDTYPE_TRANS_CONTROL_SUBMIT_REQ;
	}

	TransText * GetTransText() 
	{
		return mp_trans_text;
	}

private:
	TransText * mp_trans_text;
};


class TransSuspendReq
	: public EventData
{
public:
	TransSuspendReq(const CallID & cid, 
					const DomainType & domain,
					const UsrID & usr_id,
					const TextID & text_id) : EventData(cid), m_domain(domain), m_usr_id(usr_id), m_text_id(text_id) {}

	~TransSuspendReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_TRANS_CONTROL_SUSPEND_REQ;
	}
	
	const DomainType & GetDomain() const 
	{
		return m_domain;
	}

	const TextID & GetTextID() const
	{
		return m_text_id;
	}

	const UsrID & GetUsrID() const
	{
		return m_usr_id;
	}

private:
	DomainType m_domain;
	UsrID m_usr_id;
	TextID m_text_id;
};


class TransSuspendRes
	: public EventData
{
public:
	TransSuspendRes(const CallID & cid) : EventData(cid) {}
	~TransSuspendRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_TRANS_CONTROL_SUSPEND_RES;
	}

	const int GetResult() const
	{
		return m_result;
	}

	void SetResult(const int result)
	{
		m_result = result;
	}

private:
	int m_result;
};


class TransRerunReq
	: public EventData
{
public:
	TransRerunReq(const CallID & cid, 
				  const DomainType & domain,
				  const UsrID & usr_id,
				  const TextID & text_id) : EventData(cid), m_domain(domain), m_usr_id(usr_id), m_text_id(text_id) {}

	~TransRerunReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_TRANS_CONTROL_RERUN_REQ;
	}
	
	const DomainType & GetDomain() const 
	{
		return m_domain;
	}

	const TextID & GetTextID() const
	{
		return m_text_id;
	}

	const UsrID & GetUsrID() const
	{
		return m_usr_id;
	}

private:
	DomainType m_domain;
	UsrID m_usr_id;
	TextID m_text_id;
};


class TransRerunRes
	: public EventData
{
public:
	TransRerunRes(const CallID & cid) : EventData(cid) {}
	~TransRerunRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_TRANS_CONTROL_RERUN_RES;
	}

	const int GetResult() const
	{
		return m_result;
	}

	void SetResult(const int result)
	{
		m_result = result;
	}

private:
	int m_result;
};

class TransCancelReq
	: public EventData
{
public:
	TransCancelReq(const CallID & cid, 
				  const DomainType & domain,
				  const UsrID & usr_id,
				  const TextID & text_id) : EventData(cid), m_domain(domain), m_usr_id(usr_id), m_text_id(text_id) {}

	~TransCancelReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_TRANS_CONTROL_CANCEL_REQ;
	}
	
	const DomainType & GetDomain() const 
	{
		return m_domain;
	}

	const TextID & GetTextID() const
	{
		return m_text_id;
	}

	const UsrID & GetUsrID() const
	{
		return m_usr_id;
	}

private:
	DomainType m_domain;
	UsrID m_usr_id;
	TextID m_text_id;
};


class TransCancelRes
	: public EventData
{
public:
	TransCancelRes(const CallID & cid) : EventData(cid) {}
	~TransCancelRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_TRANS_CONTROL_CANCEL_RES;
	}

	const int GetResult() const
	{
		return m_result;
	}

	void SetResult(const int result)
	{
		m_result = result;
	}

private:
	int m_result;
};

class TransResult
	: public EventData
{
public:
	TransResult(const int result, TransText * p_trans_text): m_result(result), mp_trans_text(p_trans_text) {}
	~TransResult() {}

	const EDType GetType() const
	{
		return EDTYPE_TRANS_RESULT;
	}

	int GetResultCode() const 
	{
		return m_result;
	}

	TransText * GetTransText() const
	{
		return mp_trans_text;
	}

private:
	int m_result;
	TransText * mp_trans_text;
};



class GetTransSentReq
	: public EventData
{
public:
	GetTransSentReq(const CallID & cid, 
				  const DomainType & domain,
				  const UsrID & usr_id,
				  const size_t offset,
				  const TextID & text_id) : EventData(cid), m_domain(domain), m_usr_id(usr_id), m_text_id(text_id), m_offset(offset) {}

	~GetTransSentReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_GET_TRANS_SENT_REQ;
	}
	
	const DomainType & GetDomain() const 
	{
		return m_domain;
	}

	const TextID & GetTextID() const
	{
		return m_text_id;
	}

	const UsrID & GetUsrID() const
	{
		return m_usr_id;
	}

	size_t Offset() const
	{
		return m_offset;
	}

private:
	DomainType m_domain;
	UsrID m_usr_id;
	TextID m_text_id;
	size_t m_offset;
};


class GetTransSentRes
	: public EventData
{
public:
	GetTransSentRes(const CallID & cid) : EventData(cid), is_exist(false), total_sent(0) {}
	~GetTransSentRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_GET_TRANS_SENT_RES;
	}

	bool is_exist;
	size_t total_sent;
	vector<pair<long, pair<string, string> > > sent_vec;

};

#endif //TRANS_CONTROL_EVENT_H
