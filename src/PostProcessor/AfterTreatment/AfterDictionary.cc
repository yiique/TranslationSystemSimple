#include "AfterDictionary.h"
#include "AfterTreatPersistent.h"
#include "Log/LogStream.h"
#include "Common/f_utility.h"
#include "LanguageTools/CLanguage.h"

AfterWordID AfterDictionary::m_global_wordid = 0;

bool AfterDictionary::Initialize()
{
	return true;
}

void AfterDictionary::Print()
{
	lout << "    Print dict id = " << this->GetInfo().dict_id << endl;
	word_map_t::iterator iter = m_word_map.begin();

	size_t idx = 0;
	for(; iter != m_word_map.end(); ++iter)
	{
		word_tgt_vec_t & word_vec = iter->second;
		lout << "                [" << idx++ << "] src_key = (" << iter->first << ")" << endl;
		for(size_t i=0; i<word_vec.size(); ++i)
		{
			lout << "                    [" << i << "] word = (" << word_vec[i]->Serialize(0,0) << ")" << endl;
		}
	}
}

AfterDictWord::AfterDictWord(const AfterWordInfo & word_info): word_id(word_info.word_id),
												src_key(word_info.src_key),
												src(word_info.src),
												tgt(word_info.tgt),
												is_deleted(word_info.is_deleted),
												is_active(word_info.is_active)
{
}

void AfterDictionary::FindAppend(const string & key, const string & src_in_sent, const size_t beg, const size_t end, vector<string> & result_vec)
{


	map<string, vector<AfterDictWord*> >::iterator iter = m_word_map.find(key);

	if(m_word_map.end() == iter)
		return ;

    //lout << "Find key = [" << key << "]  tgt_size = " << iter->second.size() << endl;
	for(size_t i=0; i<iter->second.size(); ++i)
	{
		AfterDictWord * p_word = iter->second.at(i);
        lout << "tgtword.is_active = " << p_word->is_active << endl;
		if(1 == p_word->is_active)
			result_vec.push_back(serialize_match_result_with_s_offset(src_in_sent, beg, end, *p_word));
	}
}

string AfterDictionary::serialize_match_result_with_s_offset(const string & src_in_sent, const size_t src_beg, const size_t src_end, const AfterDictWord & word)
{
	//解码器会在句子前面加上<s> 所以beg 和 end 值需要加1
	stringstream ss;
	ss << (src_beg+1) << ":" << (src_end+1) << " ||| " << src_in_sent << " ||| " << word.tgt << " ||| " << "10 10 10 10 ";

	return ss.str();
}

string AfterDictWord::Serialize(const size_t beg_pos, const size_t end_pos)
{
	stringstream ss;
	ss << beg_pos << ":" << end_pos << " ||| " << src_key << " ||| " << tgt << " ||| " << "10 10 10 10 ";

	return ss.str();
}

pair<pair<AfterDictionary::word_map_t::iterator, AfterDictWord*>, bool> 
	AfterDictionary::insert_word_without_wordid(AfterWordInfo & word_info)
{

	//生成src_key
	generate_src_key(word_info);

	word_map_t::iterator siter = m_word_map.find(word_info.src_key);

	if(m_word_map.end() == siter)
	{
		vector<AfterDictWord*> wvec;
		siter = m_word_map.insert(make_pair(word_info.src_key, wvec)).first;
	}
	
	vector<AfterDictWord*> & word_vec = siter->second;

	word_tgt_vec_t::iterator titer = word_vec.begin();
	for(; titer!=word_vec.end(); ++titer)
	{
		if((*titer)->tgt == word_info.tgt)
		{
			return make_pair(make_pair(siter, (AfterDictWord*)NULL), false);
		}
	}

	AfterDictWord * p_word = new AfterDictWord(word_info);

	word_vec.push_back(p_word);

	return make_pair(make_pair(siter,p_word), true);

}

void AfterDictionary::generate_src_key(AfterWordInfo & word_info)
{

	//todo 标准化KEY的生成
	//lout << "Input src = [" << word_info.src << "]" << endl;
	//去掉其中的回车换行符
	for(size_t i=0; i<word_info.src.size(); ++i)
	{
		if(word_info.src[i] == '\r' 
			|| word_info.src[i] == '\n' 
			|| word_info.src[i] == '\t')
		{
			word_info.src[i] = ' ';
		}
	}

	if(LANGUAGE_ENGLISH == m_dict_info.domain_info.second.first)
	{
		CLanguage::EnToken(word_info.src, word_info.src_key);
	}else if(LANGUAGE_UYGHUR == m_dict_info.domain_info.second.first)
	{
		//识别是否是拉丁
		string key = word_info.src;
		if(false == CLanguage::IsUyghurLatin(key))
		{
			CLanguage::UyghurOld2Latin(key);
		}

		CLanguage::UyToken(key, word_info.src_key);

	}else
	{
		word_info.src_key = word_info.src;
	}

	filter_head_tail(word_info.src_key);

	//lout << "Output src_key = [" << word_info.src_key << "]" << endl;
}



int AfterDictionary::InsertWord(AfterWordInfo & word_info)
{
	pair<pair<word_map_t::iterator, AfterDictWord*>, bool> res = insert_word_without_wordid(word_info);

	if(false == res.second)
		return ERR_DICTWORD_INSERT_SAME_WORD;

	m_global_wordid++;
	word_info.word_id = m_global_wordid;

	//持久化 获得DICT word id
	int ret = AfterTreatPersistent::InsertWord(m_dict_info.dict_id, word_info);

	if(SUCCESS != ret)
	{
		delete res.first.second;
		remove_word_in_tgt_vec(res.first.first->second, res.first.second);
		
		return ret;
	}

	AfterDictWord * p_word = res.first.second;
	p_word->word_id = word_info.word_id;
	m_wordid_map.insert(make_pair(word_info.word_id, p_word));
	return SUCCESS;

}

int AfterDictionary::ModifyWord(AfterWordInfo & word_info)
{
	word_info.is_deleted = 0;

	map<AfterWordID, AfterDictWord* >::iterator iter = m_wordid_map.find(word_info.word_id);

	if(m_wordid_map.end() == iter)
	{
		return ERR_DICTWORD_NOT_FIND_WORD_ID;
	}

	map<string, vector<AfterDictWord*> >::iterator iter2 = m_word_map.find(iter->second->src_key);

	if(m_word_map.end() == iter2)
	{
		lwrn << "Find dict word by wordid failed. word_id = " << word_info.word_id << " word_key = [" << iter->second->src_key << "]" << endl;
		return ERR_DICTWORD_NOT_FIND_WORD_BY_WORD_ID;
	}

	//原词条
	AfterDictWord * p_old_word = iter->second;
	vector<AfterDictWord*> & dict_vec = iter2->second;

	//为新的word_info生成key
	generate_src_key(word_info);

	//判断src_key是否改变
	if(p_old_word->src_key != word_info.src_key)
	{
		//src_key改变，在wordmap中插入新词，再更新wordid索引
		pair<pair<word_map_t::iterator, AfterDictWord*>, bool> res = insert_word_without_wordid(word_info);
		
		if(false == res.second)
		{
			return ERR_WORD_MODIFY_SAMEWORD_WITH_SRC_OR_TGT;
		}

		AfterDictWord * p_new_word = res.first.second;

		p_new_word->word_id = word_info.word_id;

		//删除原wordmap中的WordDict
		for(vector<AfterDictWord*>::iterator viter = dict_vec.begin(); viter!=dict_vec.end(); ++viter)
		{
			if(word_info.word_id == (*viter)->word_id)
			{
				dict_vec.erase(viter);
				break;
			}
		}

		if(dict_vec.size() == 0)
			m_word_map.erase(iter2);

		delete p_old_word;

		//更新wordid索引
		iter->second = p_new_word;

	}else
	{
		//判断tgt是否变化
		if(p_old_word->tgt != word_info.tgt)
		{
			//tgt改变
			for(vector<AfterDictWord*>::iterator viter = dict_vec.begin(); viter!=dict_vec.end(); ++viter)
			{
				if(word_info.tgt == (*viter)->tgt)
				{
					return ERR_WORD_MODIFY_SAMEWORD_WITH_SRC_OR_TGT;
				}
			}

			//没有相同的tgt  直接修改
			p_old_word->tgt = word_info.tgt;
			p_old_word->is_active = word_info.is_active;
		}else
		{
			//判断激活状态
			if(p_old_word->is_active == word_info.is_active)
				return SUCCESS;
			else
				p_old_word->is_active = word_info.is_active;
		}
	}


	//在持久化层中删除
	return AfterTreatPersistent::ModifyWord(word_info);

	 
}


int AfterDictionary::ModifyDictInfo(const AfterDictInfo & dict_info)
{
	bool is_modify = false;
	if(dict_info.dict_name != m_dict_info.dict_name)
	{
		m_dict_info.dict_name = dict_info.dict_name;
		is_modify = true;
	}

	if(dict_info.description != m_dict_info.description)
	{
		m_dict_info.description = dict_info.description;
		is_modify = true;
	}

	lout << "old domain = " << m_dict_info.domain_info.first << endl;
	lout << "old srcl = " << m_dict_info.domain_info.second.first << endl;
	lout << "old tgtl = " << m_dict_info.domain_info.second.second << endl;

	lout << "new domain = " << dict_info.domain_info.first << endl;
	lout << "new srcl = " << dict_info.domain_info.second.first << endl;
	lout << "new tgtl = " << dict_info.domain_info.second.second << endl;

	if(dict_info.domain_info != m_dict_info.domain_info)
	{
		m_dict_info.domain_info = dict_info.domain_info;

		lout << "mdf domain = " << m_dict_info.domain_info.first << endl;
		lout << "mdf srcl = " << m_dict_info.domain_info.second.first << endl;
		lout << "mdf tgtl = " << m_dict_info.domain_info.second.second << endl;

		is_modify = true;
	}

	if(dict_info.is_active != m_dict_info.is_active)
	{
		m_dict_info.is_active = dict_info.is_active;
		is_modify = true;
	}

	if(is_modify)
		return AfterTreatPersistent::ModifyDict(dict_info);


	return SUCCESS;

}

int AfterDictionary::DeleteWord(const AfterWordID & word_id)
{
	map<AfterWordID, AfterDictWord* >::iterator iter = m_wordid_map.find(word_id);

	if(m_wordid_map.end() == iter)
	{
		return ERR_DICTWORD_NOT_FIND_WORD_ID;
	}

	map<string, vector<AfterDictWord*> >::iterator iter2 = m_word_map.find(iter->second->src_key);

	if(m_word_map.end() == iter2)
	{
		lwrn << "Find dict word by wordid failed. word_id = " << word_id << " word_key = [" << iter->second->src_key << "]" << endl;
		return ERR_DICTWORD_NOT_FIND_WORD_BY_WORD_ID;
	}

	vector<AfterDictWord*> & word_vec = iter2->second;

	bool is_find = false;
	for(vector<AfterDictWord*>::iterator iter3=word_vec.begin(); iter3<word_vec.end(); ++iter3)
	{
		if(word_id == (*iter3)->word_id)
		{
			delete (*iter3);
			word_vec.erase(iter3);

			m_wordid_map.erase(iter);

			if(word_vec.size() == 0)
				m_word_map.erase(iter2);

			is_find = true;
		}
	}

	if(false == is_find)
		return ERR_DICTWORD_NOT_FIND_WORD_BY_WORD_ID_IN_VEC;

	//todo 在持久化层中删除
	AfterTreatPersistent::DeleteWord(word_id);


	return SUCCESS;
}

AfterDictionary::AfterDictionary(const AfterDictInfo & dict_info): m_dict_info(dict_info)
{
}


AfterDictionary::~AfterDictionary(void)
{
	map<AfterWordID, AfterDictWord* >::iterator iter = m_wordid_map.begin();

	for(; iter != m_wordid_map.end(); ++iter)
	{
		delete iter->second;
	}
}

AfterDictionary * AfterDictionary::New(AfterDictInfo & dict_info)
{
	if(SUCCESS != AfterTreatPersistent::CreateDict(dict_info))
		return NULL;

	AfterDictionary * p_dict = new AfterDictionary(dict_info);

	p_dict->m_dict_info.dict_id = dict_info.dict_id;

	return p_dict;
}

void AfterDictionary::Destroy(AfterDictionary * p_dict)
{
	if(NULL == p_dict)
		return;

	AfterDictInfo dict_info;
	dict_info.dict_id = p_dict->m_dict_info.dict_id;

	AfterTreatPersistent::DeleteDict(dict_info);

	delete p_dict;
}

void AfterDictionary::Delete(AfterDictionary * p_dict)
{
	if(NULL == p_dict)
		return;

	delete p_dict;
}

bool AfterDictionary::LoadAll(vector<AfterDictionary *> & dict_vec)
{
	vector<AfterDictInfo> dict_info_vec;
	if(SUCCESS != AfterTreatPersistent::LoadAllDict(dict_info_vec))
		return false;

	for(size_t i=0; i<dict_info_vec.size(); ++i)
	{
		dict_vec.push_back(new AfterDictionary(dict_info_vec[i]));
	}

	int max_id = 0;
	//读取词条数据
	for(size_t i=0; i<dict_vec.size(); ++i)
	{
		vector<AfterWordInfo> word_info_vec;
		if(SUCCESS != AfterTreatPersistent::LoadWord(dict_vec[i]->GetInfo(), word_info_vec))
		{
			lerr << "Cant read word with dict id = " << dict_vec[i]->GetInfo().dict_id << endl;
			continue;
		}

		for(size_t k=0; k<word_info_vec.size(); ++k)
		{

			dict_vec[i]->mount_word(word_info_vec[k]);

			if(max_id < word_info_vec[k].word_id)
			{
				max_id = word_info_vec[k].word_id;
			}
		}
	}

	m_global_wordid = max_id;

	cout << "Now db the largest afterword id is: " <<  m_global_wordid << endl;
	return true;
}

AfterDictWord * AfterDictionary::mount_word(AfterWordInfo & word_info)
{
	pair<pair<word_map_t::iterator, AfterDictWord*>, bool> res = insert_word_without_wordid(word_info);

	if(false == res.second)
		return NULL;

	AfterDictWord * p_word = res.first.second;
	p_word->word_id = word_info.word_id;
	m_wordid_map.insert(make_pair(word_info.word_id, p_word));
	return p_word;
}


int AfterDictionary::InsertWordGroup(vector<AfterWordInfo> & word_info_vec)
{
	vector<pair<pair<word_map_t::iterator, AfterDictWord*>, bool> > res_vec;
	vector<bool> filter_vec;

	for(size_t i=0; i<word_info_vec.size(); ++i)
	{
		pair<pair<word_map_t::iterator, AfterDictWord*>, bool> res = insert_word_without_wordid(word_info_vec[i]);
		res_vec.push_back( res );
		
		if(true == res.second)
		{
			m_global_wordid++;
			word_info_vec[i].word_id = m_global_wordid;
		}
		filter_vec.push_back(res.second);
	}


	//持久化  必须全部成功才算成功
	int ret = AfterTreatPersistent::InsertWordGroup(m_dict_info.dict_id, filter_vec, word_info_vec);
	if(SUCCESS != ret)
	{
		lerr << "Error when insert_wordgroup_in_persistent." << endl;

		for(size_t i=0; i<res_vec.size(); ++i)
		{
			pair<pair<word_map_t::iterator, AfterDictWord*>, bool> & res = res_vec[i];

			if(true == res.second)
			{
				delete res.first.second;
				remove_word_in_tgt_vec(res.first.first->second, res.first.second);
			}
		}

		return ret;
	}

	for(size_t i=0; i<res_vec.size(); ++i)
	{
		pair<pair<word_map_t::iterator, AfterDictWord*>, bool> & res = res_vec[i];

		if(true == res.second)
		{
			AfterDictWord * p_word = res.first.second;
			p_word->word_id = word_info_vec[i].word_id;
			m_wordid_map.insert(make_pair(word_info_vec[i].word_id, p_word));
			
		}
	}

	return SUCCESS;

}

void AfterDictionary::remove_word_in_tgt_vec(vector<AfterDictWord*> & dict_vec, AfterDictWord * p_word)
{
	for(vector<AfterDictWord*>::iterator iter = dict_vec.begin(); iter != dict_vec.end(); ++iter)
	{
		if(p_word == *iter)
		{
			dict_vec.erase(iter);
			return;
		}
	}
}


//
void AfterDictionary::Find(const string & key, vector<string> & result_vec)
{
	map<string, vector<AfterDictWord*> >::iterator iter = m_word_map.find(key);

	if(m_word_map.end() == iter)
		return ;

	for(size_t i=0; i<iter->second.size(); ++i)
	{
		AfterDictWord * p_word = iter->second.at(i);
        lout << "tgtword.is_active = " << p_word->is_active << endl;
		if(1 == p_word->is_active)
		{
			result_vec.push_back(p_word->tgt);
			result_vec.push_back(p_word->src);
		}
	}
}

int AfterDictionary::RecoverWord(const AfterWordID & word_id)
{
	AfterWordInfo word_info;

	AfterTreatPersistent::RecoverWord(word_id);
	AfterTreatPersistent::GetRecoverWordInfo(word_id, word_info);

	//dict_word.word_id = word_info.word_id;
	//dict_word.src = word_info.src;
	//dict_word.tgt = word_info.tgt;
	//dict_word.is_deleted = word_info.is_deleted;
	//dict_word.is_active = word_info.is_active;
	//dict_word.is_checked = word_info.is_checked;

	pair<pair<word_map_t::iterator, AfterDictWord*>, bool> res = insert_word_without_wordid(word_info);

	if(false == res.second)
		return ERR_DICTWORD_INSERT_SAME_WORD;

	AfterDictWord * p_word = res.first.second;
	p_word->word_id = word_info.word_id;
	m_wordid_map.insert(make_pair(word_info.word_id, p_word));
	return SUCCESS;
}

AfterDictionary * AfterDictionary::Recover(AfterDictInfo & dict_info)
{
	AfterDictionary * p_dict = new AfterDictionary(dict_info);

	p_dict->m_dict_info.dict_id = dict_info.dict_id;

	return p_dict;
}

int AfterDictionary::RecoverWordGroup(vector<AfterWordInfo> & word_info_vec)
{
	vector<pair<pair<word_map_t::iterator, AfterDictWord*>, bool> > res_vec;
	vector<bool> filter_vec;

	for(size_t i=0; i<word_info_vec.size(); ++i)
	{
		pair<pair<word_map_t::iterator, AfterDictWord*>, bool> res = insert_word_without_wordid(word_info_vec[i]);
		res_vec.push_back( res );

		filter_vec.push_back(res.second);
	}

	for(size_t i=0; i<res_vec.size(); ++i)
	{
		pair<pair<word_map_t::iterator, AfterDictWord*>, bool> & res = res_vec[i];

		if(true == res.second)
		{
			AfterDictWord * p_word = res.first.second;
			p_word->word_id = word_info_vec[i].word_id;
			m_wordid_map.insert(make_pair(word_info_vec[i].word_id, p_word));
			
		}
	}

	return SUCCESS;

}
