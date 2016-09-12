#ifndef AFTERTREAT_EVENT_H
#define AFTERTREAT_EVENT_H

#include "EventEngine/EventData.h"
#include "AfterTreatDef.h"

#include <set>
#include <map>
using namespace std;

class AfterDictCreateReq
	: public EventData
{
public:
	AfterDictCreateReq(const CallID & cid) : EventData(cid) {}
	~AfterDictCreateReq(void) {}

	const AfterDictInfo & GetDictInfo() { return m_dictinfo; }
	void SetDictInfo(const AfterDictInfo & info) { m_dictinfo=info; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_CREATE_REQ; }

private:
	AfterDictInfo m_dictinfo;
};

class AfterDictCreateRes
	:public EventData
{
public:
	AfterDictCreateRes(const CallID & cid): EventData(cid) {}
	~AfterDictCreateRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_CREATE_RES; }

private:
	int m_res;
};


class AfterDictModifyReq
	: public EventData
{
public:
	AfterDictModifyReq(const CallID & cid) : EventData(cid) {}
	~AfterDictModifyReq(void) {}

	AfterDictInfo & GetDictInfo() { return m_dict_info; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_MODIFY_REQ; }

private:

	AfterDictInfo m_dict_info;
};

class AfterDictModifyRes
	:public EventData
{
public:
	AfterDictModifyRes(const CallID & cid): EventData(cid) {}
	~AfterDictModifyRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_MODIFY_RES; }

private:
	int m_res;
};


class AfterDictDeleteReq
	: public EventData
{
public:
	AfterDictDeleteReq(const CallID & cid) : EventData(cid) {}
	~AfterDictDeleteReq(void) {}

	const set<AfterDictID> & GetDictIDList() const { return m_dict_id_set; } 

	void AddDictID(const AfterDictID dict_id) { m_dict_id_set.insert(dict_id); }
	
	const EDType GetType() const { return EDTYPE_AFTER_DICT_DELETE_REQ; } 

private:
	set<AfterDictID> m_dict_id_set;

};

class AfterDictDeleteRes
	:public EventData
{
public:
	AfterDictDeleteRes(const CallID & cid) : EventData(cid) {}
	~AfterDictDeleteRes() {}


	void AddFailure(const AfterDictID dict_id, const int result) { m_failure_map.insert(make_pair(dict_id, result)); }
	
	const map<AfterDictID, int> & GetFailureList() const { return m_failure_map; }

	const EDType GetType() const {return EDTYPE_AFTER_DICT_DELETE_RES; }

private:
	map<AfterDictID, int> m_failure_map;
};


class AfterWordInsertReq
	: public EventData
{
public:
	AfterWordInsertReq(const CallID & cid) : EventData(cid) {}
	~AfterWordInsertReq(void) {}

	const AfterDictID & GetDictID() const { return m_dict_id; }
	AfterWordInfo & GetWordInfo() { return m_word_info; }
	void SetDictID(const AfterDictID dict_id) { m_dict_id = dict_id; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_WORDS_INSERT_REQ; }

private:
	AfterWordInfo m_word_info;
	AfterDictID m_dict_id;
};

class AfterWordInsertRes
	:public EventData
{
public:
	AfterWordInsertRes(const CallID & cid): EventData(cid) {}
	~AfterWordInsertRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_WORDS_INSERT_RES; }

private:
	int m_res;
};


class AfterWordDeleteReq
	: public EventData
{
public:
	AfterWordDeleteReq(const CallID & cid) : EventData(cid) {}
	~AfterWordDeleteReq(void) {}

	const AfterDictID & GetDictID() const { return m_dict_id; }
	const AfterWordID & GetWordID() const { return m_word_id; }
	void SetDictID(const AfterDictID dict_id) { m_dict_id = dict_id; }
	void SetWordID(const AfterWordID word_id) { m_word_id = word_id; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_WORDS_DELETE_REQ; } 

private:
	AfterDictID m_dict_id;
	AfterWordID m_word_id;

};

class AfterWordDeleteRes
	:public EventData
{
public:
	AfterWordDeleteRes(const CallID & cid): EventData(cid) {}
	~AfterWordDeleteRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_WORDS_DELETE_RES; }

private:
	int m_res;
};


class AfterWordLoadReq
	: public EventData
{
public:
	AfterWordLoadReq(const CallID & cid) : EventData(cid) {}
	~AfterWordLoadReq(void) {}

	const AfterDictID & GetDictID() const { return m_dict_id; }
	const string & GetWordFileName() const { return m_word_file_name; }
	int GetIsActive() const { return m_is_active; }
	void SetActive(const int is_active) { m_is_active = is_active; }
	void SetDictID(const AfterDictID dict_id) { m_dict_id = dict_id; }
	void SetWordFileName(const string & word_file_name) { m_word_file_name = word_file_name; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_WORDS_LOAD_REQ; } 

	void SetIsUTF8(bool is)
	{
		m_is_utf8 = is;
	}

	bool IsUTF8() const { return m_is_utf8; }

private:
	AfterDictID m_dict_id;
	string m_word_file_name;
	int m_is_active;
	bool m_is_utf8;
};

class AfterWordLoadRes
	:public EventData
{
public:
	AfterWordLoadRes(const CallID & cid): EventData(cid) {}
	~AfterWordLoadRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_WORDS_LOAD_RES; }

private:
	int m_res;
};


class AfterWordModifyReq
	: public EventData
{
public:
	AfterWordModifyReq(const CallID & cid) : EventData(cid) {}
	~AfterWordModifyReq(void) {}

	const AfterDictID & GetDictID() const { return m_dict_id; }
	AfterWordInfo & GetWordInfo() { return m_word_info; }
	void SetDictID(const AfterDictID dict_id) { m_dict_id = dict_id; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_WORDS_MODIFY_REQ; }

private:
	AfterWordInfo m_word_info;
	AfterDictID m_dict_id;
};

class AfterWordModifyRes
	:public EventData
{
public:
	AfterWordModifyRes(const CallID & cid): EventData(cid) {}
	~AfterWordModifyRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_AFTER_DICT_WORDS_MODIFY_RES; }

private:
	int m_res;
};


class AfterWordRecoverReq
	: public EventData
{
public:
	AfterWordRecoverReq(const CallID & cid) : EventData(cid) {}
	~AfterWordRecoverReq(void) {}

	const AfterDictID & GetDictID() const { return m_dict_id; }
	const AfterWordID & GetWordID() const { return m_word_id; }
	void SetDictID(const AfterDictID dict_id) { m_dict_id = dict_id; }
	void SetWordID(const AfterWordID word_id) { m_word_id = word_id; }
	const EDType GetType() const { return EDTYPE_DICT_AFTER_WORDS_RECOVER_REQ; } 

private:
	AfterDictID m_dict_id;
	AfterWordID m_word_id;

};

class AfterWordRecoverRes
	:public EventData
{
public:
	AfterWordRecoverRes(const CallID & cid): EventData(cid) {}
	~AfterWordRecoverRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_AFTER_WORDS_RECOVER_RES; }

private:
	int m_res;
};


class AfterDictRecoverReq
	: public EventData
{
public:
	AfterDictRecoverReq(const CallID & cid) : EventData(cid) {}
	~AfterDictRecoverReq(void) {}

	const AfterDictID & GetDictID() const { return m_dict_id; }
	void SetDictID(const AfterDictID dict_id) { m_dict_id = dict_id; }
	const EDType GetType() const { return EDTYPE_DICT_AFTER_RECOVER_REQ; } 

private:
	AfterDictID m_dict_id;
};

class AfterDictRecoverRes
	:public EventData
{
public:
	AfterDictRecoverRes(const CallID & cid): EventData(cid) {}
	~AfterDictRecoverRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_AFTER_RECOVER_RES; }

private:
	int m_res;
};


#endif

