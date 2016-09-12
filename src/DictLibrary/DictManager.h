#ifndef DICT_MANAGER_H
#define DICT_MANAGER_H

//MAIN
#include "Dictionary.h"
#include "DictDef.h"
#include "Common/f_utility.h"
#include "Common/ErrorCode.h"
#include "Common/RWLock.h"
#include "WordIndex.h"

//STL
#include <list>
using namespace std;

namespace dictionary
{


class MatchResult
{
public:
	string src;
	string tgt;

	size_t pos_beg;
	size_t pos_end;

	DictType type;
	string weight;

	static bool sort_less(const MatchResult & a, const MatchResult & b)
	{
		if( a.pos_beg < b.pos_beg )
			return true;
		else if( a.pos_beg == b.pos_beg )
			return a.pos_end > b.pos_end;
		else
			return false;
	}

	bool conflict(const MatchResult & other)
	{
		if(other.pos_beg >= pos_end || other.pos_end <= pos_beg)
			return false;
		else
			return true;
	}

public:
	string serialize();
	bool un_serialize(const string & str);
};


class DictManager
{
public:
	~DictManager(void) {}

	static DictManager * GetInstance() { return &ms_instance; }

	bool Initialize(const string & usr_dict_weight = "100 100 100 100 ",
					const string & pro_dict_weight = "50 50 50 50 ",
					const string & sys_dict_weight = "20 20 20 20 ",
					const bool & enable_remote_dict = false);

	int CreateDict(DictInfo & dict_info);
	int ModifyDict(const DictInfo & dict_info);
	int DeleteDict(const DictID dict_id);
	
	int InsertWord(const DictID dict_id, WordInfo & word_info);
	int InsertWord(const DictID dict_id, vector<WordInfo> & word_info_vec);
	int DeleteWord(const DictID dict_id, const WordID word_id);
	int ModifyWord(const DictID dict_id, WordInfo & word_info);

	int MatchSent(const UsrID & usrid, 
				const DomainType & domain_info, 
				const bool & is_with_blank,
				const string & sent,
				list<MatchResult> & result_list);
	int MatchSent(const UsrID & usrid, 
				  const DomainType & domain_info, 
				  const string & sent, 
				  list<MatchResult> & result_list);

	int ToSysDict(const vector<size_t> & wordid_vec, const DictID & tgt_dict_id);

	int RecoverWord(const DictID dict_id, const WordID word_id)
	{
		return ERROR_UNDEFINE;
	}

	int RecoverDict(const DictID dict_id)
	{
		return ERROR_UNDEFINE;
	}


	string DebugInfo() const;

private:
	DictManager(void) {} 

	int match_sent_local(const UsrID & usrid, 
						const DomainType & domain_info, 
						const bool & is_with_blank,
						const string & sent, 
						list<MatchResult> & result_list);
	int match_sent_local(const UsrID & usrid, 
						 const DomainType & domain_info, 
						 const string & sent, 
						 list<MatchResult> & result_list);
	int match_sent_remote(const UsrID & usrid, 
						  const DomainType & domain_info, 
						  const string & sent, 
						  list<MatchResult> & result_list);

	int insert_dict(const size_t new_dict_id, const DictInfo & dict_info);
	int insert_word(const size_t new_word_id, const DictID dict_id, const WordInfo & word_info);

	size_t generate_unique_dictid() { return m_dict_base_id++; }
	size_t generate_unique_wordid() { return m_word_base_id++; }
	
	string generate_key(const WordInfo & word_info);

	void filter_match_result(list<MatchResult> & result_list);
	void erase_conflict(list<MatchResult> & result_list);

private:

	static DictManager ms_instance;

	RWLock m_lock;

	size_t m_dict_base_id;
	size_t m_word_base_id;

	string m_usr_dict_weight;
	string m_pro_dict_weight;
	string m_sys_dict_weight;

	bool m_enable_remote_dict;

	WordIndex m_word_index;
	map<DictID, dict_ptr> m_dict_map;
};

}

#endif //DICT_MANAGER_H
