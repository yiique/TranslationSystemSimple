#ifndef TEXT_HANDLER_EVENT_H
#define TEXT_HANDLER_EVENT_H

#include "EventEngine/EventData.h"
#include "TransText/TransText.h"
#include "Common/ErrorCode.h"

class TextLoadReq
	: public EventData
{
public:
	TextLoadReq(const CallID & cid, 
			   TransText * p_trans_text) : mp_trans_text(p_trans_text)
	{
		m_call_id = cid;
	}

	~TextLoadReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_LOAD_TEXT_REQ;
	}

	TransText * GetTransText()
	{
		return mp_trans_text;
	};

private:
	TransText * mp_trans_text;
};

class TextLoadRes
	: public EventData
{
public:
	TextLoadRes(const CallID & cid, 
			   TransText * p_trans_text) : mp_trans_text(p_trans_text),
										   m_result(SUCCESS)
	{
		m_call_id = cid;
	}

	~TextLoadRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_LOAD_TEXT_RES;
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



class TextCreateReq
	: public EventData
{
public:
	TextCreateReq(const CallID & cid) 
	{
		m_call_id = cid;
		m_is_bilingual = false;
	}

	~TextCreateReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CREATE_RESULT_REQ;
	}

	void SetTextID(const TextID & tid)
	{
		m_text_id = tid;
	}

	const TextID & GetTextID() const
	{
		return m_text_id;
	}

	const string & CreateType() const
	{
		return m_create_type;
	}

	void SetCreateType(const string create_type)
	{
		m_create_type = create_type;
	}

	bool IsBilingual() const
	{
		return m_is_bilingual;
	}

	void SetBilingual(const bool is_bilingual)
	{
		m_is_bilingual = is_bilingual;
	}

private:
	TextID m_text_id;
	string m_create_type;
	bool m_is_bilingual;
};

class TextCreateRes
	: public EventData
{
public:
	TextCreateRes(const CallID & cid) : m_result(SUCCESS)
	{
		m_call_id = cid;
	}

	~TextCreateRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CREATE_RESULT_RES;
	}

	const string & GetResultFileName() const 
	{
		return m_result_file_name;
	};

	void SetResultFileName(const string & file_name)
	{
		m_result_file_name = file_name;
	}

	int GetResult() const
	{ 
		return m_result;
	};

	void SetResult(const int result)
	{
		m_result = result;
	};

private:
	
	string m_result_file_name;
	int m_result;
};


class TextSubmitReq
	: public EventData
{
public:
	TextSubmitReq(const CallID & cid, 
			      TransText * p_trans_text): mp_trans_text(p_trans_text)
	{
		m_call_id = cid;
	}

	~TextSubmitReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_SUBMIT_RESULT_REQ;
	}

	TransText * GetTransText()
	{
		return mp_trans_text;
	};

private:
	TransText * mp_trans_text;
};

class TextSubmitRes
	: public EventData
{
public:
	TextSubmitRes(const CallID & cid) : m_result(SUCCESS)
	{
		m_call_id = cid;
	}

	~TextSubmitRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_SUBMIT_RESULT_RES;
	}

	int GetResult() const
	{ 
		return m_result;
	};

	void SetResult(const int result)
	{
		m_result = result;
	};

private:

	int m_result;
};


#endif  //TEXT_HANDLER_EVENT_H
