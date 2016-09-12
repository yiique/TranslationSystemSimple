#ifndef AFTER_DICTIONARY_H
#define AFTER_DICTIONARY_H

#include "AfterTreatDef.h"
#include "LanguageTools/ZDQToken.h"

#include <map>
#include <vector>
#include <sstream>

using namespace std;


class AfterDictWord
{
public:
	AfterDictWord(const AfterWordInfo & word_info);
	string Serialize(const size_t beg_pos, const size_t end_pos);

public:
	AfterWordID word_id;

	string src_key;
	string src;
	string tgt;
	int is_deleted; //是否已删除  1表示已经删除 持久层用
	int is_active; //是否启用  1表示启用	
};

class AfterDictionary
{
public:
	~AfterDictionary(void);

	static bool Initialize();

	static bool LoadAll(vector<AfterDictionary *> & dict_vec);
	static AfterDictionary * New(AfterDictInfo & dict_info); //此方法会在持久层中创建
	static void Destroy(AfterDictionary * p_dict);
	static void Delete(AfterDictionary * p_dict);

	int InsertWord(AfterWordInfo & word_info);
	int DeleteWord(const AfterWordID & word_id);
	int ModifyWord(AfterWordInfo & word_info);
	void FindAppend(const string & key,  const string & src_in_sent, const size_t beg, const size_t end, vector<string> & result_vec);

	int ModifyDictInfo(const AfterDictInfo & dict_info);

	const AfterDictInfo & GetInfo() const
	{
		return m_dict_info;
	}

	int InsertWordGroup(vector<AfterWordInfo> & word_info_vec);

	void Print();
	void Find(const string & key, vector<string> & result_vec);
	int RecoverWord(const AfterWordID & word_id);
	static AfterDictionary * Recover(AfterDictInfo & dict_info);
	int RecoverWordGroup(vector<AfterWordInfo> & word_info_vec);
private:
	AfterDictionary(const AfterDictInfo & dict_info);

	AfterDictInfo m_dict_info;

	typedef vector<AfterDictWord*> word_tgt_vec_t;
	typedef map<string, word_tgt_vec_t > word_map_t;

	word_map_t m_word_map;
	map<AfterWordID, AfterDictWord* > m_wordid_map;

	AfterDictWord * mount_word(AfterWordInfo & word_info);
	
	pair<pair<word_map_t::iterator, AfterDictWord*>, bool> insert_word_without_wordid(AfterWordInfo & word_info);

	void generate_src_key(AfterWordInfo & word_info);

	string serialize_match_result_with_s_offset(const string & src_in_sent, const size_t src_beg, const size_t src_end, const AfterDictWord & word);

	void remove_word_in_tgt_vec(vector<AfterDictWord*> & dict_vec, AfterDictWord * p_word);

	static AfterWordID m_global_wordid;
};


#endif
