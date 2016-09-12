#include "AfterTreatManager.h"
#include "Common/f_utility.h"
#include "Common/Utf8Stream.h"
#include <fstream>
#include "DataBase/DBOperation.h"
#include "Common/ICUConverter.h"

#define ALL_DOMAIN_NAME "all" //表示通用领域

AfterTreatManager AfterTreatManager::ms_instance;

AfterTreatManager * AfterTreatManager::GetInstance()
{
	return &ms_instance;
}

bool AfterTreatManager::Initialize(const string & upload_file_path)
{
	WriteLockGuard guard(m_rw_lock);

	m_upload_file_path = upload_file_path;

	if(false == AfterDictionary::Initialize())
	{
		lerr << "AfterDictionary init failed." << endl;
		return false;
	}

	vector<AfterDictionary*> dict_vec;
	if(false == AfterDictionary::LoadAll(dict_vec))
		return false;

	for(size_t i=0; i<dict_vec.size(); ++i)
	{

		AfterDictionary * p_dict = dict_vec[i];
	
		map<UsrID, UsrAfterDict*>::iterator iter = m_usrdict_map.find(p_dict->GetInfo().usr_id);

		if(m_usrdict_map.end() == iter)
		{
			iter = m_usrdict_map.insert(make_pair(p_dict->GetInfo().usr_id, new UsrAfterDict(p_dict->GetInfo().usr_id))).first;
		}

		if(false == iter->second->MountDict(p_dict))
			AfterDictionary::Delete(p_dict);
		else
			m_dict_info_map.insert(make_pair(p_dict->GetInfo().dict_id, p_dict->GetInfo()));
	}

	return true;

}

void UsrAfterDict::Print()
{
	stringstream ss;

	size_t idx = 0;
	for(map<DomainType, map<AfterDictID, AfterDictionary*> >::iterator iter =  m_dict_map.begin();
		iter != m_dict_map.end(); ++iter)
	{
		ss << "           [" << idx++ << "]  domain : [" << iter->first.first << " : " << iter->first.second.first << " : " << iter->first.second.second << "]" << endl;

		for(map<AfterDictID, AfterDictionary*>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
		{
			ss << "                 dict_id = " << iter2->first << endl;
			iter2->second->Print();
		}
	}

	lout << "      Print Usr: " << m_usr_id << "'s Map: " << endl << ss.str() << endl;

}

void AfterTreatManager::PrintAllDict() 
{
	WriteLockGuard guard(m_rw_lock);

	stringstream ss;


	size_t idx=0;
	for(map<AfterDictID, AfterDictInfo>::iterator iter = m_dict_info_map.begin(); iter != m_dict_info_map.end(); ++iter)
	{
		ss << "   [" << idx++ << "]  dict_id = " << iter->first << endl; 
	}

	lout << "Print DictID Map: " << endl << ss.str() << endl;


	idx = 0;

	lout << "Print UsrID Map: " << endl;
	for(map<UsrID, UsrAfterDict*>::iterator iter = m_usrdict_map.begin(); iter != m_usrdict_map.end(); ++iter)
	{
		lout << "   [" << idx++ << "]  usr_id = " << iter->first << endl; 
		iter->second->Print();
	}

}

bool UsrAfterDict::MountDict(AfterDictionary * p_dict)
{
	if(NULL == p_dict)
	{
		return false;
	}

	map<AfterDictID, AfterDictionary*> & domain_dict_map = m_dict_map[p_dict->GetInfo().domain_info];

	map<AfterDictID, AfterDictionary*>::iterator iter = domain_dict_map.find(p_dict->GetInfo().dict_id);

	if(domain_dict_map.end() == iter)
	{
		domain_dict_map.insert(make_pair(p_dict->GetInfo().dict_id, p_dict));
	}else
	{
		lerr << "Same id in UsrAfterDict: dict id = " << p_dict->GetInfo().dict_id << endl;
		return false;
	}

	return true;

}

int AfterTreatManager::CreateDict(AfterDictInfo & dict_info)
{
	WriteLockGuard guard(m_rw_lock);

	map<UsrID, UsrAfterDict*>::iterator miter = m_usrdict_map.find(dict_info.usr_id);

	if(m_usrdict_map.end() == miter)
	{
		miter = m_usrdict_map.insert(make_pair(dict_info.usr_id, new UsrAfterDict(dict_info.usr_id))).first;
	}

	int result = miter->second->CreateDict(dict_info);

	m_dict_info_map.insert(make_pair(dict_info.dict_id, dict_info));

	return result;

}

int AfterTreatManager::ModifyDict(const AfterDictInfo & dict_info)
{
	WriteLockGuard guard(m_rw_lock);

	map<AfterDictID, AfterDictInfo>::iterator info_iter = m_dict_info_map.find(dict_info.dict_id);

	if(m_dict_info_map.end() == info_iter)
	{
		return ERR_DICT_NOT_FIND_IN_INFOMAP;
	}

	map<UsrID, UsrAfterDict*>::iterator usr_iter = m_usrdict_map.find(info_iter->second.usr_id);

	if(m_usrdict_map.end() == usr_iter)
	{
		return ERR_DICT_NOT_FIND_IN_USRMAP;
	}

	int ret = usr_iter->second->ModifyDict(info_iter->second.domain_info, dict_info);
	//lout << "ret = " << ret << endl;

	if(SUCCESS == ret )
	{
		//lout << "rest info_iter" << endl;
		info_iter->second.description = dict_info.description;
		info_iter->second.dict_name = dict_info.dict_name;
		info_iter->second.domain_info = dict_info.domain_info;
		info_iter->second.is_active = dict_info.is_active;
		info_iter->second.is_deleted = 0;
	}

	return ret;
}

int AfterTreatManager::DeleteDict(const AfterDictID dict_id)
{
	WriteLockGuard guard(m_rw_lock);

	map<AfterDictID, AfterDictInfo>::iterator info_iter = m_dict_info_map.find(dict_id);

	if(m_dict_info_map.end() == info_iter)
	{
		return ERR_DICT_NOT_FIND_IN_INFOMAP;
	}

	map<UsrID, UsrAfterDict*>::iterator usr_iter = m_usrdict_map.find(info_iter->second.usr_id);

	if(m_usrdict_map.end() == usr_iter)
	{
		return ERR_DICT_NOT_FIND_IN_USRMAP;
	}

	int result = usr_iter->second->DeleteDict(info_iter->second.domain_info, dict_id);


	//lout << "DeleteDict  res = " <<  result << " dict_id = " << dict_id << endl;

	if(SUCCESS == result)
		m_dict_info_map.erase(info_iter);


	return result;

}



int AfterTreatManager::LoadWordFile(const AfterDictID dict_id, const string & word_file_path, const int is_active, const bool is_utf8)
{
	WriteLockGuard guard(m_rw_lock);

	string full_file_path = m_upload_file_path + "/" + word_file_path;

	AfterDictInfo * p_dict_info = find_dict_info(dict_id);

	if(NULL == p_dict_info)
		return ERR_DICT_NOT_FIND_IN_INFOMAP;

	UsrAfterDict * p_usr_dict = find_usr_dict(p_dict_info->usr_id);

	vector<AfterWordInfo> word_info_vec;
	if(NULL == p_usr_dict)
	{
		return ERR_DICT_NOT_FIND_IN_USRMAP;
	}
	//读取文件
	if(false == load_word_file(full_file_path, is_utf8, word_info_vec))
		return ERR_DICT_LOAD_WORD_FILE_FAILED;

	for(size_t i=0; i<word_info_vec.size(); ++i)
	{
		word_info_vec[i].is_active = is_active;
	}

	return p_usr_dict->InsertWordGroup(p_dict_info->domain_info, dict_id, word_info_vec);
}

int AfterTreatManager::InsertWord(const AfterDictID dict_id, AfterWordInfo & word_info)
{
	WriteLockGuard guard(m_rw_lock);

	AfterDictInfo * p_dict_info = find_dict_info(dict_id);

	if(NULL == p_dict_info)
		return ERR_DICT_NOT_FIND_IN_INFOMAP;

	UsrAfterDict * p_usr_dict = find_usr_dict(p_dict_info->usr_id);

	if(NULL == p_usr_dict)
	{
		return ERR_DICT_NOT_FIND_IN_USRMAP;
	}


	return p_usr_dict->InsertWord(p_dict_info->domain_info, dict_id, word_info);
}

bool AfterTreatManager::load_word_file(const string & file_path, const bool is_utf8, vector<AfterWordInfo> & word_info_vec)
{
	word_info_vec.clear();

	ifstream file(file_path.c_str());

	if(false == file.good())
		return false;

	string line;
	size_t idx = 0;
	while(getline(file, line))
	{	
		filter_head_tail(line);

		idx++;
		vector<string> tmp_vec;
		split_string_by_tag(line.c_str(), tmp_vec, '\t');

		if(tmp_vec.size() < 2)
		{
			lwrn << "line = [" << line << "]" << endl;
			lwrn << "tmp_vec.size() = " << tmp_vec.size() << endl;
			lwrn << "tmp_vec[0] = [" << tmp_vec[0] << "]" << endl;
			lwrn << "Illegal line : " << idx << endl;
			continue;
		}

		AfterWordInfo word_info;
		word_info.src = tmp_vec.at(0);
		word_info.tgt = tmp_vec.at(1);

		if(false == is_utf8)
		{
			ICUConverter::Convert("GB18030", "UTF-8", word_info.src);
			ICUConverter::Convert("GB18030", "UTF-8", word_info.tgt);
		}

		word_info_vec.push_back(word_info);
	}

	return true;
}

AfterDictInfo * AfterTreatManager::find_dict_info(const AfterDictID dict_id)
{
	map<AfterDictID, AfterDictInfo>::iterator iter = m_dict_info_map.find(dict_id);

	if(m_dict_info_map.end() == iter)
		return NULL;

	return &(iter->second);
}

UsrAfterDict * AfterTreatManager::find_usr_dict(const UsrID & usr_id)
{
	map<UsrID, UsrAfterDict*>::iterator iter = m_usrdict_map.find(usr_id);

	if(m_usrdict_map.end() == iter)
		return NULL;

	return iter->second;
}

int AfterTreatManager::ModifyWord(const AfterDictID dict_id, AfterWordInfo & word_info)
{
	WriteLockGuard guard(m_rw_lock);

	AfterDictInfo * p_dict_info = find_dict_info(dict_id);

	if(NULL == p_dict_info)
		return ERR_DICT_NOT_FIND_IN_INFOMAP;

	UsrAfterDict * p_usr_dict = find_usr_dict(p_dict_info->usr_id);

	if(NULL == p_usr_dict)
		return ERR_DICT_NOT_FIND_IN_USRMAP;

	return p_usr_dict->ModifyWord(p_dict_info->domain_info, dict_id, word_info);
}

int AfterTreatManager::DeleteWord(const AfterDictID dict_id, const AfterWordID word_id)
{
	WriteLockGuard guard(m_rw_lock);

	AfterDictInfo * p_dict_info = find_dict_info(dict_id);

	if(NULL == p_dict_info)
		return ERR_DICT_NOT_FIND_IN_INFOMAP;

	UsrAfterDict * p_usr_dict = find_usr_dict(p_dict_info->usr_id);

	if(NULL == p_usr_dict)
		return ERR_DICT_NOT_FIND_IN_USRMAP;

	return p_usr_dict->DeleteWord(p_dict_info->domain_info, dict_id, word_id);
}

int UsrAfterDict::CreateDict(AfterDictInfo & dict_info)
{
	map<AfterDictID, AfterDictionary*> & dict_group = m_dict_map[dict_info.domain_info];

	AfterDictionary * p_dict = AfterDictionary::New(dict_info);

	if(NULL == p_dict)
		return ERR_NEW_DICTIONARY;

	dict_group.insert(make_pair(dict_info.dict_id, p_dict));

	return SUCCESS;
}

int UsrAfterDict::ModifyDict(const DomainType & old_domain_info, const AfterDictInfo & dict_info)
{
	/*lout << "before modify dict  id = " << dict_info.dict_id << endl;

	lout << "old domain = " << old_domain_info.first << endl;
	lout << "old srcl = " << old_domain_info.second.first << endl;
	lout << "old tgtl = " << old_domain_info.second.second << endl;
	lout << "====================" << endl;
	lout << "new domain = " << dict_info.domain_info.first << endl;
	lout << "new srcl = " << dict_info.domain_info.second.first << endl;
	lout << "new tgtl = " << dict_info.domain_info.second.second << endl;*/

	map<DomainType, map<AfterDictID, AfterDictionary*> >::iterator iter = m_dict_map.find(old_domain_info);

	if(m_dict_map.end() == iter)
	{
		lerr << "ERR_DICTGROUP_NOT_FIND_IN_USRDICT" << endl;
		return ERR_DICTGROUP_NOT_FIND_IN_USRDICT;
	}

	map<AfterDictID, AfterDictionary*>::iterator iter2 = iter->second.find(dict_info.dict_id);

	if(iter->second.end() == iter2)
	{
		lerr << "ERR_DICT_NOT_FIND" << endl;
		return ERR_DICT_NOT_FIND;
	}

	AfterDictionary * p_dict = iter2->second;

	if(dict_info.domain_info != old_domain_info)
	{
		//在原位置删除
		iter->second.erase(iter2);

		//挂接到新位置
		if(false == m_dict_map[dict_info.domain_info].insert(make_pair(dict_info.dict_id, p_dict)).second ) 
		{
			iter->second.insert(make_pair(dict_info.dict_id, p_dict)); //放回原位置
			lerr << "ERR_DICT_MODIFY_SAME_DICTID" << endl;
			return ERR_DICT_MODIFY_SAME_DICTID;
		}


	}

	return p_dict->ModifyDictInfo(dict_info);

}


int UsrAfterDict::DeleteDict(const DomainType & domain_info, const AfterDictID dict_id)
{
	map<DomainType, map<AfterDictID, AfterDictionary*> >::iterator iter = m_dict_map.find(domain_info);

	if(m_dict_map.end() == iter)
	{
		return ERR_DICTGROUP_NOT_FIND_IN_USRDICT;
	}

	map<AfterDictID, AfterDictionary*>::iterator iter2 = iter->second.find(dict_id);

	if(iter->second.end() != iter2)
	{
		AfterDictionary::Destroy( iter2->second );
		iter->second.erase(iter2);
	}else
	{
		lerr << "Not find this dict in usrid = " << m_usr_id << " , domain_name = " << domain_info.first << " , srclanguage = " << domain_info.second.first << " , tgtlanguage = " << domain_info.second.second << " , dict_id = " << dict_id << endl;
	}


	return SUCCESS;
}

int UsrAfterDict::InsertWord(const DomainType & domain_info, const AfterDictID dict_id, AfterWordInfo & word_info)
{
	AfterDictionary * p_dict = find_dictionary(domain_info, dict_id);

	if(NULL == p_dict)
		return ERR_DICT_NOT_FIND;

	return p_dict->InsertWord(word_info);
}

int UsrAfterDict::InsertWordGroup(const DomainType & domain_info, const AfterDictID dict_id, vector<AfterWordInfo> & word_info_vec)
{
	AfterDictionary * p_dict = find_dictionary(domain_info, dict_id);

	if(NULL == p_dict)
		return ERR_DICT_NOT_FIND;

	return p_dict->InsertWordGroup(word_info_vec);
}

int UsrAfterDict::ModifyWord(const DomainType & domain_info, const AfterDictID dict_id, AfterWordInfo & word_info)
{
	AfterDictionary * p_dict = find_dictionary(domain_info, dict_id);

	if(NULL == p_dict)
		return ERR_DICT_NOT_FIND;

	return p_dict->ModifyWord(word_info);
}

int UsrAfterDict::DeleteWord(const DomainType & domain_info, const AfterDictID dict_id, const AfterWordID word_id)
{

	AfterDictionary * p_dict = find_dictionary(domain_info, dict_id);

	if(NULL == p_dict)
		return ERR_DICT_NOT_FIND;

	return p_dict->DeleteWord(word_id);

}

AfterDictionary * UsrAfterDict::find_dictionary(const DomainType & domain_info, const AfterDictID dict_id)
{
	map<DomainType, map<AfterDictID, AfterDictionary*> >::iterator iter = m_dict_map.find(domain_info);

	if(m_dict_map.end() == iter)
	{
		lerr << "errinfo :" <<  ERR_DICTGROUP_NOT_FIND_IN_USRDICT << endl;
		return NULL;
	}


	map<AfterDictID, AfterDictionary*>::iterator iter2 = iter->second.find(dict_id);

	if(iter->second.end() == iter2)
	{
		lerr << "errinfo :" <<   ERR_DICT_NOT_FIND_IN_USRDICT << endl;
		return NULL;
	}

	return iter2->second;
}

void UsrAfterDict::generate_key(const vector<string> & word_vec, const size_t beg, const size_t end, const bool is_key_with_blank, string & key)
{
	assert(end < word_vec.size() && beg <= end);

	key.clear();

	for(size_t end=beg; end<word_vec.size(); ++end)
	{
		if(is_key_with_blank && beg != end)
			key += " ";

		key += word_vec[end];
	}

}

bool AfterTreatManager::Check(const string & src)
{
	Utf8Stream check(src);
	string tmp_str;

	check.ReadOneCharacter(tmp_str);

	if('A'<= tmp_str.c_str()[0] && tmp_str.c_str()[0] <= 'Z')
	{
		return true;
	}

	else if('a'<= tmp_str.c_str()[0] && tmp_str.c_str()[0] <= 'z')
	{
		return true;
	}

	else
	{
		return false;
	}
}

string AfterTreatManager::en_vector2string(vector<string> & src, int len)
{
	string str;

	if(len == 0)
	{
		return " ";
	}

	if(len > src.size())
	{
		for(int i=0; i<src.size(); i++)
		{
			str += (src[i]+" ");
		}

		return str;
	}

	for(int i=0; i<len; i++)
	{
		str += (src[i]+" ");
	}

	return str;
}

string AfterTreatManager::ch_vector2string(vector<string> & src, int len)
{
	string str;

	if(len == 0)
	{
		return " ";
	}

	if(len > src.size())
	{
		for(int i=0; i<src.size(); i++)
		{
			str += src[i];
		}

		return str;
	}

	for(int i=0; i<len; i++)
	{
		str += src[i];
	}

	return str;
}

int UsrAfterDict::Find(const DomainType & domain_info, const string & src, vector<string> & result_vec)
{
	map<DomainType, map<AfterDictID, AfterDictionary*> >::iterator iter = m_dict_map.find(domain_info);

	if(m_dict_map.end() == iter)
	{
		return ERR_DICTGROUP_NOT_FIND_IN_USRDICT;
	}

	map<AfterDictID, AfterDictionary*> & dict_group = iter->second;

	map<AfterDictID, AfterDictionary*>::iterator iter2 = dict_group.begin();
	for(; iter2 != dict_group.end(); ++iter2)
	{
		AfterDictionary * p_dict = iter2->second;

		if(1 == p_dict->GetInfo().is_active)
			p_dict->Find(src, result_vec);
	}

	return SUCCESS;
}

int UsrAfterDict::Match(const DomainType & domain_info, vector<string> & src_vec, vector<string> & result_vec, bool is_key_with_blank)
{
	map<DomainType, map<AfterDictID, AfterDictionary*> >::iterator iter = m_dict_map.find(domain_info);

	if(m_dict_map.end() == iter)
	{
		return ERR_DICTGROUP_NOT_FIND_IN_USRDICT;
	}

	map<AfterDictID, AfterDictionary*> & dict_group = iter->second;

	if(src_vec.size() == 0)
		return SUCCESS;

	for(int i=src_vec.size(); i>0; i--)
	{
		string key;
		generate_key(src_vec, 0, i-1, is_key_with_blank, key);

		map<AfterDictID, AfterDictionary*>::iterator iter2 = dict_group.begin();
		for(; iter2 != dict_group.end(); ++iter2)
		{
			AfterDictionary * p_dict = iter2->second;

			if(1 == p_dict->GetInfo().is_active)
				p_dict->Find(key, result_vec);
		}
	}

	return SUCCESS;
}

string AfterTreatManager::Replace(const string & src, const DomainType & domain_info, const UsrID & usrid)
{
	ReadLockGuard guard(m_rw_lock);

	UsrAfterDict * p_dict = find_usr_dict(SYSMANAGER);

	if(!p_dict)
		return src;

	DomainType all_domain = domain_info;
	all_domain.first = ALL_DOMAIN_NAME;

	vector<string> src_vec;
	bool is_key_with_blank;

	if(domain_info.second.second == LANGUAGE_CHINESE)
	{
		Utf8Stream ss(src);
		string ch_str;
		while(ss.ReadOneCharacter(ch_str))
		{
			src_vec.push_back(ch_str);
		}

		is_key_with_blank = false;
	}else
	{
		split_string_by_blank(src.c_str(), src_vec);

		is_key_with_blank = true;
	}



	//search
	map<pair<size_t, size_t>, string> match_result_map;

	string key;
	for(size_t i=0; i<src_vec.size(); ++i)
	{
		key.clear();
		for(size_t k=i; k<src_vec.size(); ++k)
		{
			if(i!=k && is_key_with_blank)
				key += " ";
			
			key += src_vec[k];

			vector<string> match_vec;

			p_dict->Find(domain_info, key, match_vec);
			if(all_domain != domain_info)
					p_dict->Find(all_domain, key, match_vec);

			if(match_vec.size() > 0)
			{
				//lout << "Find : " << key << endl;
				//lout << "Find " << key << " : " << i << "," << k << endl;
				match_result_map[make_pair(i, k)] = match_vec[0];
			}else
			{

				//lout << "Not find : " << key << endl;
			}

		}

	}

	if(match_result_map.size() == 0)
		return src;


	//replace
	vector<pair<size_t, size_t> > replace_pos_vec;
	vector<string> replace_str_vec;
	for(size_t i=0; i<src_vec.size(); ++i)
	{
		for(size_t len=src_vec.size(); len > 0; --len)
		{
			map<pair<size_t, size_t>, string>::iterator iter = match_result_map.find(make_pair(i, i+len-1));

			if(iter != match_result_map.end())
			{
				i += len - 1;
				replace_pos_vec.push_back(iter->first);
				replace_str_vec.push_back(iter->second);

				//lout << "replace Find  : " << iter->first.first << "," << iter->first.second << endl;
				break;
			}
		}

	}


	if(replace_pos_vec.size() == 0)
		return src;

	size_t rep_idx = 0;
	string tgt;
	for(size_t i=0; i<src_vec.size(); ++i)
	{
		if(i!=0 && is_key_with_blank)
			tgt += " ";

		if(rep_idx < replace_pos_vec.size())
		{
			if(i >= replace_pos_vec[rep_idx].first)
			{
				tgt += replace_str_vec[rep_idx];
				i = replace_pos_vec[rep_idx].second;

				/*lout << "replace_pos_vec size = " << replace_pos_vec.size() << endl;
				lout << "replace_pos_vec idx = " << rep_idx << endl;
				lout << "replace : " << replace_pos_vec[rep_idx].first << "," << replace_pos_vec[rep_idx].second << endl;*/

				rep_idx++;
				continue;
			}
		}

		tgt += src_vec[i];
	}


	return tgt;
}


int AfterTreatManager::RecoverWord(const AfterDictID dict_id, const AfterWordID word_id)
{
	WriteLockGuard guard(m_rw_lock);

	AfterDictInfo * p_dict_info = find_dict_info(dict_id);

	if(NULL == p_dict_info)
		return ERR_DICT_NOT_FIND_IN_INFOMAP;

	UsrAfterDict * p_usr_dict = find_usr_dict(p_dict_info->usr_id);

	if(NULL == p_usr_dict)
		return ERR_DICT_NOT_FIND_IN_USRMAP;

	return p_usr_dict->RecoverWord(p_dict_info->domain_info, dict_id, word_id);
}

int AfterTreatManager::RecoverDict(const AfterDictID dict_id)
{
	WriteLockGuard guard(m_rw_lock);

	DBOperation::RecoverAfterDict(dict_id);

	AfterDictInfo dict_info;
	vector<AfterWordInfo> dict_word_info;

	DBOperation::GetRecoverAfterDictInfo(dict_id, dict_info);

	map<UsrID, UsrAfterDict*>::iterator miter = m_usrdict_map.find(dict_info.usr_id);

	if(m_usrdict_map.end() == miter)
	{
		miter = m_usrdict_map.insert(make_pair(dict_info.usr_id, new UsrAfterDict(dict_info.usr_id))).first;
	}

	miter->second->RecoverDict(dict_info);

	m_dict_info_map.insert(make_pair(dict_info.dict_id, dict_info));

	DBOperation::GetRecoverAfterDictWordInfo(dict_id, dict_word_info);

	AfterDictInfo * dict_info_t = find_dict_info(dict_id);

	if(NULL == dict_info_t)
	{
		return ERR_DICT_NOT_FIND_IN_INFOMAP;
	}

	UsrAfterDict * usr_dict_t = find_usr_dict(dict_info.usr_id);
	if(NULL == usr_dict_t)
	{
		return ERR_DICT_NOT_FIND_IN_USRMAP;
	}

	AfterDictionary * p_dict_t = usr_dict_t->find_dictionary(dict_info.domain_info, dict_id);
	if(NULL == p_dict_t)
	{
		return ERR_DICT_NOT_FIND;
	}

	return p_dict_t->RecoverWordGroup(dict_word_info);
}

int UsrAfterDict::RecoverDict(AfterDictInfo & dict_info)
{
	map<AfterDictID, AfterDictionary*> & dict_group = m_dict_map[dict_info.domain_info];

	AfterDictionary * p_dict = AfterDictionary::Recover(dict_info);

	if(NULL == p_dict)
		return ERR_NEW_DICTIONARY;

	dict_group.insert(make_pair(dict_info.dict_id, p_dict));

	return SUCCESS;
}

int UsrAfterDict::RecoverWord(const DomainType & domain_info, const AfterDictID dict_id, const AfterWordID word_id)
{
	AfterDictionary * p_dict = find_dictionary(domain_info, dict_id);

	if(NULL == p_dict)
		return ERR_DICT_NOT_FIND;

	return p_dict->RecoverWord(word_id);
}
