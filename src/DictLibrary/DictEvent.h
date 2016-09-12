#ifndef DICT_EVENT_H
#define DICT_EVENT_H

#include "EventEngine/EventData.h"
#include "DictDef.h"

#include <set>
#include <map>
#include <vector>
using namespace std;

class DictCreateReq
	: public EventData
{
public:
	DictCreateReq(const CallID & cid) : EventData(cid) {}
	~DictCreateReq(void) {}

	const DictInfo & GetDictInfo() { return m_dictinfo; }
	void SetDictInfo(const DictInfo & info) { m_dictinfo=info; }
	const EDType GetType() const { return EDTYPE_DICT_CREATE_REQ; }

private:
	DictInfo m_dictinfo;
};

class DictCreateRes
	:public EventData
{
public:
	DictCreateRes(const CallID & cid): EventData(cid) {}
	~DictCreateRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_CREATE_RES; }

private:
	int m_res;
};


class DictDeleteReq
	: public EventData
{
public:
	DictDeleteReq(const CallID & cid) : EventData(cid) {}
	~DictDeleteReq(void) {}

	const set<DictID> & GetDictIDList() const { return m_dict_id_set; } 

	void AddDictID(const DictID dict_id) { m_dict_id_set.insert(dict_id); }
	
	const EDType GetType() const { return EDTYPE_DICT_DELETE_REQ; } 

private:
	set<DictID> m_dict_id_set;

};

class DictDeleteRes
	:public EventData
{
public:
	DictDeleteRes(const CallID & cid) : EventData(cid) {}
	~DictDeleteRes() {}


	void AddFailure(const DictID dict_id, const int result) { m_failure_map.insert(make_pair(dict_id, result)); }
	
	const map<DictID, int> & GetFailureList() const { return m_failure_map; }

	const EDType GetType() const {return EDTYPE_DICT_DELETE_RES; }

private:
	map<DictID, int> m_failure_map;
};

class WordInsertReq
	: public EventData
{
public:
	WordInsertReq(const CallID & cid) : EventData(cid) {}
	~WordInsertReq(void) {}

	const DictID & GetDictID() const { return m_dict_id; }
	WordInfo & GetWordInfo() { return m_word_info; }
	void SetDictID(const DictID dict_id) { m_dict_id = dict_id; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_INSERT_REQ; }

private:
	WordInfo m_word_info;
	DictID m_dict_id;
};

class WordInsertRes
	:public EventData
{
public:
	WordInsertRes(const CallID & cid): EventData(cid) {}
	~WordInsertRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_INSERT_RES; }

private:
	int m_res;
};

class WordDeleteReq
	: public EventData
{
public:
	WordDeleteReq(const CallID & cid) : EventData(cid) {}
	~WordDeleteReq(void) {}

	const DictID & GetDictID() const { return m_dict_id; }
	const WordID & GetWordID() const { return m_word_id; }
	void SetDictID(const DictID dict_id) { m_dict_id = dict_id; }
	void SetWordID(const WordID word_id) { m_word_id = word_id; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_DELETE_REQ; } 

private:
	DictID m_dict_id;
	WordID m_word_id;

};

class WordDeleteRes
	:public EventData
{
public:
	WordDeleteRes(const CallID & cid): EventData(cid) {}
	~WordDeleteRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_DELETE_RES; }

private:
	int m_res;
};


class WordLoadReq
	: public EventData
{
public:
	WordLoadReq(const CallID & cid) : EventData(cid) {}
	~WordLoadReq(void) {}

	const DictID & GetDictID() const { return m_dict_id; }
	const string & GetWordFileName() const { return m_word_file_name; }
	int GetIsActive() const { return m_is_active; }
	void SetActive(const int is_active) { m_is_active = is_active; }
	void SetDictID(const DictID dict_id) { m_dict_id = dict_id; }
	void SetWordFileName(const string & word_file_name) { m_word_file_name = word_file_name; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_LOAD_REQ; } 

	void SetIsUTF8(bool is)
	{
		m_is_utf8 = is;
	}

	bool IsUTF8() const { return m_is_utf8; }

private:
	DictID m_dict_id;
	string m_word_file_name;
	int m_is_active;
	bool m_is_utf8;

};

class WordLoadRes
	:public EventData
{
public:
	WordLoadRes(const CallID & cid): EventData(cid) {}
	~WordLoadRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_LOAD_RES; }

private:
	int m_res;
};

class WordModifyReq
	: public EventData
{
public:
	WordModifyReq(const CallID & cid) : EventData(cid) {}
	~WordModifyReq(void) {}

	const DictID & GetDictID() const { return m_dict_id; }
	WordInfo & GetWordInfo() { return m_word_info; }
	void SetDictID(const DictID dict_id) { m_dict_id = dict_id; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_MODIFY_REQ; }

private:
	WordInfo m_word_info;
	DictID m_dict_id;
};

class WordModifyRes
	:public EventData
{
public:
	WordModifyRes(const CallID & cid): EventData(cid) {}
	~WordModifyRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_MODIFY_RES; }

private:
	int m_res;
};


class DictModifyReq
	: public EventData
{
public:
	DictModifyReq(const CallID & cid) : EventData(cid) {}
	~DictModifyReq(void) {}

	DictInfo & GetDictInfo() { return m_dict_info; }
	const EDType GetType() const { return EDTYPE_DICT_MODIFY_REQ; }

private:

	DictInfo m_dict_info;
};

class DictModifyRes
	:public EventData
{
public:
	DictModifyRes(const CallID & cid): EventData(cid) {}
	~DictModifyRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_MODIFY_RES; }

private:
	int m_res;
};



class ToSysDictReq
	: public EventData
{
public:
	ToSysDictReq(const CallID & cid) : EventData(cid) {}
	~ToSysDictReq(void) {}

	const EDType GetType() const { return EDTYPE_DICT_TOSYSTEM_REQ; }
	vector<size_t> & GetSrcWordID()
	{ 
		 return m_word_id;
	}
	const DictID & GetTgtDictID() const { return m_tgt_dict_id; }
	void SetSrcWordID(const vector<size_t> & src_word_id) 
	{
		for(int i=0; i<src_word_id.size(); i++)
		{
			m_word_id.push_back(src_word_id[i]);
		}
	}
	void SetTgtDictID(DictID tgt_dict_id) {m_tgt_dict_id=tgt_dict_id;}
private:
	vector<size_t> m_word_id;
	DictID m_tgt_dict_id;
};

class ToSysDictRes
	:public EventData
{
public:
	ToSysDictRes(const CallID & cid) : EventData(cid) {}
	~ToSysDictRes() {}
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_TOSYSTEM_RES; }
private:
	int m_res;
};


class WordRecoverReq
	: public EventData
{
public:
	WordRecoverReq(const CallID & cid) : EventData(cid) {}
	~WordRecoverReq(void) {}

	const DictID & GetDictID() const { return m_dict_id; }
	const WordID & GetWordID() const { return m_word_id; }
	void SetDictID(const DictID dict_id) { m_dict_id = dict_id; }
	void SetWordID(const WordID word_id) { m_word_id = word_id; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_RECOVER_REQ; } 

private:
	DictID m_dict_id;
	WordID m_word_id;

};

class WordRecoverRes
	:public EventData
{
public:
	WordRecoverRes(const CallID & cid): EventData(cid) {}
	~WordRecoverRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_RECOVER_RES; }

private:
	int m_res;
};


class DictRecoverReq
	: public EventData
{
public:
	DictRecoverReq(const CallID & cid) : EventData(cid) {}
	~DictRecoverReq(void) {}

	const DictID & GetDictID() const { return m_dict_id; }
	void SetDictID(const DictID dict_id) { m_dict_id = dict_id; }
	const EDType GetType() const { return EDTYPE_DICT_RECOVER_REQ; } 

private:
	DictID m_dict_id;
};

class DictRecoverRes
	:public EventData
{
public:
	DictRecoverRes(const CallID & cid): EventData(cid) {}
	~DictRecoverRes() {}
	
	
	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_RECOVER_RES; }

private:
	int m_res;
};
//ÐÂÔöÏÂÔØ
/*
class WordDownloadReq
	: public EventData
{
public:
	class WordDownloadReq(const CallID & cid) : EventData(cid) {}
	class WordDownloadReq(void) {}

	const DictID & GetDictID() const { return m_dict_id; }
	vector<WordID> & GetWordid() { return m_word_info; }
	void SetDictID(const DictID dict_id) { m_dict_id = dict_id; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_DOWNLOAD_REQ; }

private:
	vector<WordID> m_word_id;
	DictID m_dict_id;
};

class WordDownloadRes
	:public EventData
{
public:
	WordDownloadRes(const CallID & cid): EventData(cid) {}
	~WordDownloadRes() {}


	int GetRes() const { return m_res; }
	void SetRes(int res) { m_res=res; }
	const EDType GetType() const { return EDTYPE_DICT_WORDS_DOWNLOAD_RES; }

private:
	int m_res;
};
*/
#endif //DICT_EVENT_H
