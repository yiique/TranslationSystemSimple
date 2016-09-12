#include "WordIndex.h"

using namespace dictionary;
//bool WordIndex::Search(const string & key, list<word_ptr> & word_list)
//{
//	word_map_t::iterator iter = m_word_map.find(key);
//
//	if(iter == m_word_map.end())
//		return false;
//
//	set<word_ptr> & sp_word_set = iter->second;
//
//	for(set<word_ptr>::iterator iter = sp_word_set.begin(); iter != sp_word_set.end(); ++iter)
//	{
//		word_list.push_back(*iter);
//	}
//
//	return true;
//}
//
//word_ptr WordIndex::Insert(word_ptr & sp_word)
//{
//	set<word_ptr> tmp_set;
//	pair<word_map_t::iterator, bool> res = m_word_map.insert(make_pair(sp_word->key, tmp_set));
//
//	if(res.second == true)
//	{
//		res.first->second.insert(sp_word);
//		return sp_word;
//	}else
//	{
//		set<word_ptr> & sp_word_set = res.first->second;
//
//		for(set<word_ptr>::iterator iter = sp_word_set.begin(); iter != sp_word_set.end(); ++iter)
//		{
//			if((*iter)->tgt == sp_word->tgt)
//			{
//				return *iter;
//			}
//		}
//
//		sp_word_set.insert(sp_word);
//
//		return sp_word;
//	}
//}
//
//void WordIndex::Erase(word_ptr & sp_word)
//{
//	word_map_t::iterator iter = m_word_map.find(sp_word->key);
//
//	if(iter == m_word_map.end())
//		return ;
//
//	iter->second.erase(sp_word);
//
//	if(iter->second.size() == 0)
//		m_word_map.erase(iter);
//}
