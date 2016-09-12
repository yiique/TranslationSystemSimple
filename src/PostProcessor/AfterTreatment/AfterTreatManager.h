#ifndef AFTER_TREAT_MANAGER_H
#define AFTER_TREAT_MANAGER_H

#include "Common/RWLock.h"
#include "Common/BasicType.h"

#include "AfterDictionary.h"
#include "Log/LogStream.h"

#include <string>
#include <map>
using namespace std;


class UsrAfterDict
{
	friend class AfterTreatManager;
public:
	UsrAfterDict(const UsrID & usr_id) : m_usr_id(usr_id)
	{
	}

	int CreateDict(AfterDictInfo & dict_info);
	int ModifyDict(const DomainType & old_domain_info, const AfterDictInfo & dict_info);
	int DeleteDict(const DomainType & domain_info, const AfterDictID dict_id);

	int InsertWord(const DomainType & domain_info, const AfterDictID dict_id, AfterWordInfo & word_info);
	int InsertWordGroup(const DomainType & domain_info, const AfterDictID dict_id, vector<AfterWordInfo> & word_info_vec);
	int DeleteWord(const DomainType & domain_info, const AfterDictID dict_id, const AfterWordID word_id);
	int ModifyWord(const DomainType & domain_info, const AfterDictID dict_id, AfterWordInfo & word_info);

	int Match(const DomainType & domain_info, vector<string> & src_vec, vector<string> & result_vec, bool is_key_with_blank);
	int Find(const DomainType & domain_info, const string & src, vector<string> & result_vec);

	bool MountDict(AfterDictionary * p_dict);

	void Print();
	int RecoverWord(const DomainType & domain_info, const AfterDictID dict_id, const AfterWordID word_id);
	int RecoverDict(AfterDictInfo & dict_info);
private:
	string m_usr_id;

	map<DomainType, map<AfterDictID, AfterDictionary*> > m_dict_map;

	AfterDictionary * find_dictionary(const DomainType & domain_info, const AfterDictID dict_id);

	void generate_key(const vector<string> & word_vec, const size_t beg, const size_t end, const bool is_key_with_blank, string & key);

};


class AfterTreatManager
{
public:
	~AfterTreatManager(void) {}

    static AfterTreatManager * GetInstance();

	bool Initialize(const string & upload_file_path);

	int CreateDict(AfterDictInfo & dict_info);
	int ModifyDict(const AfterDictInfo & dict_info);
	int DeleteDict(const AfterDictID dict_id);
	int ToSysDict(const AfterDictID & src_dict_id, const AfterDictID & tgt_dict_id);

	int LoadWordFile(const AfterDictID dict_id, const string & word_file_path, const int is_active = 1, const bool is_utf8 = true);
	int InsertWord(const AfterDictID dict_id, AfterWordInfo & word_info);
	int DeleteWord(const AfterDictID dict_id, const AfterWordID word_id);
	int ModifyWord(const AfterDictID dict_id, AfterWordInfo & word_info);

	void PrintAllDict();

	int RecoverWord(const AfterDictID dict_id, const AfterWordID word_id);
	int RecoverDict(const AfterDictID dict_id);

	bool Check(const string & src);
	string en_vector2string(vector<string> & src, int len);
	string ch_vector2string(vector<string> & src, int len);
	string Replace(const string & src, const DomainType & domain_info, const UsrID & usrid);
private:
    AfterTreatManager(void) {}
    
    static AfterTreatManager ms_instance;

	RWLock m_rw_lock;

	string m_upload_file_path;
	map<UsrID, UsrAfterDict*> m_usrdict_map;
	map<AfterDictID, AfterDictInfo> m_dict_info_map;

	AfterDictInfo * find_dict_info(const AfterDictID dict_id);
	UsrAfterDict * find_usr_dict(const UsrID & usr_id);
	bool load_word_file(const string & file_path, const bool is_utf8, vector<AfterWordInfo> & word_info_vec);
};

#endif
