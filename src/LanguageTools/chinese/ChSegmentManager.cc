#include "ChSegmentManager.h"
#include "DataBase/DBOperation.h"
#include "Common/ICUConverter.h"
#include "Common/f_utility.h"
#include "Common/ICUConverter.h"


ChSegmentManager ChSegmentManager::ms_instance;

#define CH_SEGMENT_RESOURCE_PATH "mdata/chinese/pbclas/data/"

ChSegmentManager::ChSegmentManager()
{
}
 
ChSegmentManager::~ChSegmentManager()
{
}

bool ChSegmentManager::Initialize(const string & upload_file_path)
{
	m_upload_file_path = upload_file_path;

	//读取数据库
	vector<seg_word_share_ptr> sp_word_vec;
	DBOperation::LoadSegWord(sp_word_vec);

	WriteLockGuard lock_guard(m_rw_lock);


#ifdef CH_SEG_WITH_TAG
	if(false == m_ch_pbclas.Initialize(CH_SEGMENT_RESOURCE_PATH, "y")) //true表示带词性标记
#else
	if(false == m_ch_pbclas.Initialize(CH_SEGMENT_RESOURCE_PATH, "n")) //false表示不带词性标记
#endif
	{
		return false;
	}


	for(size_t i=0; i<sp_word_vec.size(); ++i)
	{
		seg_word_share_ptr sp_word = sp_word_vec[i];

		//插入内存
		if(false == m_word_id_map.insert(make_pair(sp_word->word_id, sp_word)).second)
		{
			
			continue;
		}

		if(false == m_word_map.insert(make_pair(sp_word->key, sp_word)).second)
		{
			m_word_id_map.erase(sp_word->word_id);
			//lout << "same word id = " << sp_word->word_id << " word = " << sp_word->key << endl;
			continue;
		}

		//插入pbclas

		if(1 == sp_word->is_active)
		{
			string gbk_word = sp_word->key;
			ICUConverter::Convert("UTF-8", "GB18030", gbk_word);
			m_ch_pbclas.InsertEntry(gbk_word);
		}
	}

	return true;
}

size_t ChSegmentManager::RecoverySegWord(const vector<int> & word_id_vec)
{
	//读取数据库
	vector<seg_word_share_ptr> sp_word_vec;
	DBOperation::RecoverySegWord(word_id_vec, sp_word_vec);

	WriteLockGuard lock_guard(m_rw_lock);

	size_t success_num = 0;

	for(size_t i=0; i<sp_word_vec.size(); ++i)
	{

		if(m_word_map.find(sp_word_vec[i]->key) != m_word_map.end() 
			|| m_word_id_map.find(sp_word_vec[i]->word_id) != m_word_id_map.end())
		{
			lerr << "Same word when recovery , id = " << sp_word_vec[i]->word_id << " , word = " << sp_word_vec[i]->key << endl;
			continue;
		}else
		{
			m_word_id_map[sp_word_vec[i]->word_id] = sp_word_vec[i];
			m_word_map[sp_word_vec[i]->key] = sp_word_vec[i];
		}

		if(1 == sp_word_vec[i]->is_active)
		{
			string gbk_word = sp_word_vec[i]->key;

			ICUConverter::Convert("UTF-8", "GB18030", gbk_word);

			m_ch_pbclas.InsertEntry(gbk_word);
		}

		success_num++;
	}

	return success_num++;

}

void ChSegmentManager::Segment(const string & src, string & tgt)
{
	ReadLockGuard lock_guard(m_rw_lock);

	string gbk_src = src;

	ICUConverter::Convert("UTF-8", "GB18030", gbk_src);
	
#ifdef CH_SEG_WITH_TAG
	m_ch_pbclas.INTGenerate(gbk_src, tgt, "y"); 
#else
	m_ch_pbclas.INTGenerate(gbk_src, tgt, "n");
#endif
	
	ICUConverter::Convert("GB18030", "UTF-8", tgt);
}

void ChSegmentManager::Segment(const string & src, string & tgt, const set<string> & force_dict)
{
	ReadLockGuard lock_guard(m_rw_lock);

	set<string> gbk_dict;

	for(set<string>::iterator iter = force_dict.begin(); iter != force_dict.end(); ++iter)
	{
		lout << "Fseg word : " << *iter << endl;

		string gbk_word(*iter);
		ICUConverter::Convert("UTF-8", "GB18030", gbk_word);
		gbk_dict.insert(gbk_word);
	}

	string gbk_src = src;
	ICUConverter::Convert("UTF-8", "GB18030", gbk_src);
	
#ifdef CH_SEG_WITH_TAG
	m_ch_pbclas.INTGenerate(gbk_src, tgt, "y", gbk_dict); 
#else
	m_ch_pbclas.INTGenerate(gbk_src, tgt, "n", gbk_dict);
#endif
	
	ICUConverter::Convert("GB18030", "UTF-8", tgt);
}


bool ChSegmentManager::InsertSegWord(seg_word_share_ptr sp_word)
{
	WriteLockGuard lock_guard(m_rw_lock);

	if(m_word_map.find(sp_word->key) == m_word_map.end())
	{
		return false;
	}

	//插入数据库  获得word_id
	if(SUCCESS != DBOperation::InsertSegWord(sp_word))
	{
		return false;
	}

	//插入内存
	m_word_id_map[sp_word->word_id] = sp_word;
	m_word_map[sp_word->key] = sp_word;

	//插入pbclas
	if(1 == sp_word->is_active)
	{
		string gbk_word = sp_word->key;
		//lout << "sp_word->key = " << sp_word->key << endl;
		ICUConverter::Convert("UTF-8", "GB18030", gbk_word);
		//lout << "gbk_word = " << gbk_word << endl;
		m_ch_pbclas.InsertEntry(gbk_word);
	}

	return true;
}

bool ChSegmentManager::DeleteSegWord(const int word_id)
{
	WriteLockGuard lock_guard(m_rw_lock);

	map<int, seg_word_share_ptr>::iterator iter_id = m_word_id_map.find(word_id);

	if(iter_id == m_word_id_map.end())
	{
		lerr << "Not find id : " << word_id << endl;
		return false;
	}

	map<string, seg_word_share_ptr>::iterator iter_word = m_word_map.find(iter_id->second->key);

	if(iter_word == m_word_map.end())
	{
		lerr << "Not find key : " << iter_id->second->key << endl;
		return false;
	}


	//delete in db
	if(SUCCESS != DBOperation::DeleteSegWord(iter_id->second->word_id))
	{
		return false;
	}

	//delete in pbclas
	lout << "Delete id : " << word_id << endl;
	lout << "is active : " << iter_id->second->is_active << endl;
	if(1 == iter_id->second->is_active)
	{
		
		string gbk_word = iter_id->second->key;
		ICUConverter::Convert("UTF-8", "GB18030", gbk_word);
		m_ch_pbclas.DeleteEntry(gbk_word);
		lout << "Delete word : " << gbk_word << endl;
	}

	//delete in mem
	m_word_id_map.erase(iter_id);
	m_word_map.erase(iter_word);

	return true;
}

pair<bool, size_t> ChSegmentManager::InsertSegWord(vector<seg_word_share_ptr> & sp_word_vec)
{
	if(sp_word_vec.size() == 0)
		return make_pair(true, 0);

	vector<bool> res_vec(sp_word_vec.size(), true);

	set<string> filter_set;

	for(size_t i=0; i<sp_word_vec.size(); ++i)
	{
		res_vec[i] = filter_set.insert(sp_word_vec[i]->key).second;
	}


	WriteLockGuard lock_guard(m_rw_lock);

	for(size_t i=0; i<sp_word_vec.size(); ++i)
	{
		if(true == res_vec[i] && m_word_map.find(sp_word_vec[i]->key) != m_word_map.end())
		{
			res_vec[i] = false;

			//lout << "Same word = " << sp_word_vec[i]->key << endl;
		}
	}

	//insert_db
	DBOperation::InsertSegWord(sp_word_vec, res_vec);

	for(size_t i=0; i<sp_word_vec.size(); ++i)
	{
		//lout << "word = " << sp_word_vec[i]->key << " , res = " << res_vec[i] << " , is_active = " << sp_word_vec[i]->is_active << endl;

		if(true == res_vec[i])
		{
			m_word_id_map[sp_word_vec[i]->word_id] = sp_word_vec[i];
			m_word_map[sp_word_vec[i]->key] = sp_word_vec[i];
		}

		if(true == res_vec[i] && 1 == sp_word_vec[i]->is_active)
		{
			string gbk_word = sp_word_vec[i]->key;

			ICUConverter::Convert("UTF-8", "GB18030", gbk_word);

			m_ch_pbclas.InsertEntry(gbk_word);
		}
	}

	size_t suc_num = 0;

	for(size_t i=0; i<res_vec[i]; ++i)
	{
		if(true == res_vec[i])
			++suc_num;
	}

	return make_pair(true, suc_num);
}



size_t ChSegmentManager::DeleteSegWord(const vector<int> & word_id_vec)
{
	WriteLockGuard lock_guard(m_rw_lock);

	//lout << "word id size = " << word_id_vec.size() << endl;

	vector<bool> res_vec(word_id_vec.size(), true);
	vector<pair<map<int, seg_word_share_ptr>::iterator, map<string, seg_word_share_ptr>::iterator> > iter_vec(word_id_vec.size(), make_pair(m_word_id_map.end(), m_word_map.end()));

	for(size_t i=0; i<word_id_vec.size(); ++i)
	{
		map<int, seg_word_share_ptr>::iterator & iter_id = iter_vec[i].first;

		iter_id = m_word_id_map.find(word_id_vec[i]);

		if(iter_id == m_word_id_map.end())
		{
			lout << "not find word id = " << word_id_vec[i] << endl;
			res_vec[i] = false;
			continue;
		}

		map<string, seg_word_share_ptr>::iterator & iter_word = iter_vec[i].second;
		
		iter_word = m_word_map.find(iter_id->second->key);

		if(iter_word == m_word_map.end())
		{
			lout << "not find word , word = " << iter_id->second->key << " , id = " << word_id_vec[i] << endl;
			res_vec[i] = false;
			continue;
		}

	}

	//delete in db
	DBOperation::DeleteSegWord(word_id_vec, res_vec);
	
	//delete in pbclas
	for(size_t i=0; i<res_vec.size(); ++i)
	{
		lout << "word_id = " << word_id_vec[i] << " res = " << res_vec[i] << " isactive = " << iter_vec[i].first->second->is_active << endl;
		if(res_vec[i] == true && 1 == iter_vec[i].first->second->is_active)
		{
			string gbk_word = iter_vec[i].first->second->key;
			ICUConverter::Convert("UTF-8", "GB18030", gbk_word);
			m_ch_pbclas.DeleteEntry(gbk_word);
		}
	}

	//delete in mem
	for(size_t i=0; i<res_vec.size(); ++i)
	{
		if(res_vec[i] == true)
		{
			m_word_id_map.erase(iter_vec[i].first);
			m_word_map.erase(iter_vec[i].second);
		}
	}

	size_t suc_num = 0;

	for(size_t i=0; i<res_vec[i]; ++i)
	{
		if(true == res_vec[i])
			++suc_num;
	}

	return suc_num;
}


bool ChSegmentManager::ModifySegWord(seg_word_share_ptr sp_word)
{

	WriteLockGuard lock_guard(m_rw_lock);

	map<int, seg_word_share_ptr>::iterator iter_id = m_word_id_map.find(sp_word->word_id);

	if(iter_id == m_word_id_map.end())
	{
		lerr << "Not find this id . tid = " << sp_word->word_id << endl;
		return false;
	}

	map<string, seg_word_share_ptr>::iterator iter_word = m_word_map.find(iter_id->second->key);

	if(iter_word == m_word_map.end())
	{
		lerr << "Not find this key . tid = " << sp_word->word_id << "  key = " << iter_id->second->key << endl;
		assert(false);
		return false;
	}

	if(sp_word->word == iter_id->second->word && sp_word->is_active == iter_id->second->is_active)
	{
		return true;
	}

	//update in db
	
	if(SUCCESS != DBOperation::UpdateSegWord(sp_word))
	{
		return false;
	}

	//update
	string old_gbk_word = iter_id->second->key;
	ICUConverter::Convert("UTF-8", "GB18030", old_gbk_word);
	m_ch_pbclas.DeleteEntry(old_gbk_word);
	
	if(1 == sp_word->is_active)
	{
		string new_gbk_word = sp_word->key;
		ICUConverter::Convert("UTF-8", "GB18030", new_gbk_word);
		m_ch_pbclas.InsertEntry(new_gbk_word);
	}

	iter_id->second = sp_word;
	m_word_map.erase(iter_word);
	m_word_map[sp_word->key] = sp_word;

	return true;
}

pair<bool, size_t> ChSegmentManager::InsertSegWord(const string & file_name, const int is_active, const bool is_utf8)
{
	string file_path = m_upload_file_path + "/" + file_name;

	ifstream file(file_path.c_str());

	if(!file.good())
		return make_pair(false, 0);

	vector<seg_word_share_ptr> sp_word_vec;

	string line;
	while(getline(file, line))
	{
		
		filter_head_tail(line);

		string word = line;

		if(false == is_utf8)
			ICUConverter::Convert("GB18030", "UTF-8", word);

		sp_word_vec.push_back( seg_word_share_ptr(new ChSegWord(word, is_active)) );
	}

	file.close();

	return InsertSegWord(sp_word_vec);
}
