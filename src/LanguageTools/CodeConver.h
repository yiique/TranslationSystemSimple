#ifndef CODE_CONVER_H
#define CODE_CONVER_H


#include <string>
#include <list>
#include <fstream>
#include <sstream>
#include "../../../Common/Utf8Stream.h"
#include "../../NewTools.h"
#include "TrieTree.h"

using namespace std;

enum CodeType
{
	CODE_TYPE_UTF8
};

class CodeChar
{
public:
	static void GetCharVec(const string& _line, vector<string>& _vChar, const CodeType& _type = CODE_TYPE_UTF8)
	{
		if (_type == CODE_TYPE_UTF8)
		{
			Utf8Stream u8sent(_line);
			string u8char;
			while (u8sent.ReadOneCharacter(u8char))
			{
				_vChar.push_back(u8char);
			}
		}
	}
};

typedef TireTree<string, string> TireStr;

class CodeMap : public TireStr
{
public:
	TireNode<string, string>* insertKey(const string& key, const string& val)
	{
		TireNode<string, string>* pNode = &m_TireHead;
		pNode = pNode->insert(key);
		pNode->m_Val = val;
		return pNode;
	}

	TireNode<string, string>* find(const vector<string>& vKeys, const unsigned int& uBeg, unsigned int& _in_length)
	{
// 		list<TireNode<string, string>* > listNode;
		TireNode<string, string>* pNode = GetHead();
		TireNode<string, string>* pTemp = NULL;
		unsigned int tmpLength = 0;
		for (unsigned int i = uBeg; i < vKeys.size(); i++)
		{
			pNode = pNode->find(vKeys[i]);
			if (pNode == NULL)
				return pTemp;
			tmpLength++;
			if (pNode->m_Val != "")
			{
				pTemp = pNode;
				_in_length = tmpLength;
			}
		}
		return pTemp;
	}
};


class CodeConver
{
public:
	CodeConver()
	{
	}

	bool InitMap(const string& _mapfile)
	{
		ifstream fin(_mapfile.c_str());
		string line;
		if (fin.good() == false)
			return false;
		while (getline(fin, line))
		{
			stringstream sstream(line);
			string src, tgt;
			sstream >> src >> tgt;
			m_CodeMap[src] = tgt;
		}
		return true;
	}

	bool InitTire(const string& _tirefile)
	{
		ifstream fin(_tirefile.c_str());
		if (fin.good() == false)
			return false;
		string line;
		while (getline(fin, line))
		{
			vector<string> vPair;
			LKTools::ParseWord(line, "\t", vPair);
			if (vPair.size() != 2)
				continue;
			vector<string> vu8char;
			Utf8Stream u8stream(vPair[0]);
			string u8char;
			while (u8stream.ReadOneCharacter(u8char)) vu8char.push_back(u8char);
			m_CodeTire.insert(vu8char, vPair[1]);
		}
		return true;
	}

	string ConverMap(const string& _strin)
	{
		string strres;
		Utf8Stream u8sent(_strin);
		string u8char;
		while (u8sent.ReadOneCharacter(u8char))
		{
			map<string, string>::iterator itr = m_CodeMap.find(u8char);
			if (itr != m_CodeMap.end())
				strres += itr->second;
			else
				strres += u8char;
		}
		return strres;
	}

	string ConverTire(const string& _strin)
	{
		string strres;
		vector<string> vChar;
		CodeChar::GetCharVec(_strin, vChar);
		for (unsigned int i = 0; i < vChar.size();)
		{
			unsigned int length = 0;
			const TireNode<string, string>* pNode = m_CodeTire.find(vChar, i, length);
			if (pNode == NULL || length == 0) // Ã»ÓÐ¸Ã±àÂë
			{
				strres += vChar[i];
				i++;
			}
			else
			{
				strres += pNode->m_Val;
				i += length;
			}
		}
		return strres;
	}

private:
	map<string, string> m_CodeMap;
	CodeMap m_CodeTire;
};

#endif //CODE_CONVER_H
