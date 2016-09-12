#ifndef TIRE_NODE
#define TIRE_NODE

#include <map>
#include <vector>

using namespace std;

template <typename _Key, typename _Val>
class TireNode
{
public:
	TireNode<typename _Key, typename _Val>* insert(const _Key& key) 
	{
		pair<map<_Key, TireNode<_Key, _Val> >::iterator, bool> prItr = m_SubNode.insert(make_pair(key, TireNode<_Key, _Val>()));
		return &prItr.first->second;
	}

	TireNode<typename _Key, typename _Val>* find(const _Key& key) 
	{
		map<_Key, TireNode<_Key, _Val> >::iterator itr = m_SubNode.find(key);
		if (itr != m_SubNode.end())
			return &itr->second;
		return NULL;
	}
public:
	map<_Key, TireNode<_Key, _Val> > m_SubNode;
	_Val m_Val;
};

template <class _Key, class _Val>
class TireTree
{
public:
	TireNode<typename _Key, typename _Val>* insert(const vector<_Key>& vKeys, const _Val& val)
	{
		TireNode<_Key, _Val>* pNode = &m_TireHead;
		for (vector<typename _Key>::const_iterator itr = vKeys.begin(); itr != vKeys.end(); itr++)
		{
			pNode = pNode->insert(*itr);
		}
		pNode->m_Val = val;
		return pNode;
	}

	TireNode<typename _Key, typename _Val>* find(const vector<_Key>& vKeys)
	{
		TireNode<typename _Key, typename _Val>* pNode = GetHead();
		for (vector<typename _Key>::const_iterator itr = vKeys.begin(); itr != vKeys.end(); itr++)
		{
			pNode = pNode->find(*itr);
			if (pNode == NULL)
				return NULL;
		}
		return pNode;
	}

	TireNode<typename _Key, typename _Val>* GetHead(void) { return &m_TireHead; }
protected:
	TireNode<typename _Key, typename _Val> m_TireHead;
};


#endif