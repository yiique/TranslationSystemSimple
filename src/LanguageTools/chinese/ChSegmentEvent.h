#ifndef CH_SEGMENT_EVENT_H
#define CH_SEGMENT_EVENT_H

//MAIN
#include "Common/BasicType.h"
#include "EventEngine/EventData.h"
#include "ChSegmentDef.h"

//STL
#include <string>
#include <vector>
using namespace std;

class InsertChSegWordReq
	: public EventData
{
public:
	InsertChSegWordReq(const CallID & cid) : EventData(cid) {}
	~InsertChSegWordReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_INSERT_REQ;
	}

	void AddWord(seg_word_share_ptr sp_word)
	{
		m_word_vec.push_back(sp_word);
	}
	
	vector<seg_word_share_ptr> & GetWordVec() 
	{
		return m_word_vec;
	}

private:
	vector<seg_word_share_ptr> m_word_vec;
};

class InsertChSegWordRes
	: public EventData
{
public:
	InsertChSegWordRes(const CallID & cid) : EventData(cid) {}
	~InsertChSegWordRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_INSERT_REQ;
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

class DeleteChSegWordReq
	: public EventData
{
public:
	DeleteChSegWordReq(const CallID & cid) : EventData(cid) {}
	~DeleteChSegWordReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_DELETE_REQ;
	}

	void AddWordID(const int word_id)
	{
		m_word_id_vec.push_back(word_id);
	}
	
	const vector<int> & GetWordIdVec() const 
	{
		return m_word_id_vec;
	}

private:
	vector<int> m_word_id_vec;
};

class DeleteChSegWordRes
	: public EventData
{
public:
	DeleteChSegWordRes(const CallID & cid) : EventData(cid) {}
	~DeleteChSegWordRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_DELETE_REQ;
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


class ModifyChSegWordReq
	: public EventData
{
public:
	ModifyChSegWordReq(const CallID & cid, seg_word_share_ptr sp_word = seg_word_share_ptr()) : EventData(cid), m_sp_word(sp_word) {}
	~ModifyChSegWordReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_MODIFY_REQ;
	}

	void SetWord(seg_word_share_ptr sp_word)
	{
		m_sp_word = sp_word;
	}

	seg_word_share_ptr GetWord() const
	{
		return m_sp_word;
	}

private:
	seg_word_share_ptr m_sp_word;
};

class ModifyChSegWordRes
	: public EventData
{
public:
	ModifyChSegWordRes(const CallID & cid) : EventData(cid) {}
	~ModifyChSegWordRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_MODIFY_REQ;
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


class LoadChSegWordReq
	: public EventData
{
public:
	LoadChSegWordReq(const CallID & cid) : EventData(cid), m_is_active(1) {}
	~LoadChSegWordReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_LOAD_REQ;
	}

	void SetFileName(const string & file_name)
	{
		m_file_name = file_name;
	}

	const string & GetFileName() const 
	{
		return m_file_name;
	}

	int IsActive() const
	{
		return m_is_active;
	}

	void SetActive(const int is_active)
	{
		m_is_active = is_active;
	}

	void SetIsUTF8(bool is)
	{
		m_is_utf8 = is;
	}

	bool IsUTF8() const { return m_is_utf8; }
	
private:
	string m_file_name;
	int m_is_active;
	bool m_is_utf8;
};

class LoadChSegWordRes
	: public EventData
{
public:
	LoadChSegWordRes(const CallID & cid) : EventData(cid) {}
	~LoadChSegWordRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_LOAD_REQ;
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


class RecoveryChSegWordReq
	: public EventData
{
public:
	RecoveryChSegWordReq(const CallID & cid) : EventData(cid) {}
	~RecoveryChSegWordReq(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_RECOVERY_REQ;
	}

	void AddWordID(const int word_id)
	{
		m_word_id_vec.push_back(word_id);
	}
	
	const vector<int> & GetWordIdVec() const 
	{
		return m_word_id_vec;
	}

private:
	vector<int> m_word_id_vec;
};

class RecoveryChSegWordRes
	: public EventData
{
public:
	RecoveryChSegWordRes(const CallID & cid) : EventData(cid) {}
	~RecoveryChSegWordRes(void) {}

	const EDType GetType() const
	{
		return EDTYPE_CH_SEG_RECOVERY_RES;
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

#endif //CH_SEGMENT_EVENT_H
