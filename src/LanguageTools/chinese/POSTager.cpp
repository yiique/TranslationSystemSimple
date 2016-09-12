#include <iostream>
#include <fstream>
#include <strstream>
#include <cmath>
#include "POSTager.h"

CPOSTager::CPOSTager()
{
}

CPOSTager::~CPOSTager()
{
}

bool CPOSTager::Initialize(const string &POSFile, const string &WordPOSFile, const string &ModelFlie, const int &histo)
{
	m_nHistogram = histo;

	return LoadPOS(POSFile) 
		&& LoadWordPOS(WordPOSFile) 
	//	&& m_cChType.Initialize("data/punc", "data/alph", "data/num")
		&& m_pModel.Initialize(ModelFlie);
}

bool CPOSTager::LoadPOS(const string &POSFile)
{
	ifstream fin(POSFile.c_str());
	
	if (!fin)
	{
		cerr << "Can not open file: " << POSFile << endl;
		return false;
	}

	string line;

	while (getline(fin, line))
	{
		if (!line.empty())
		{
			m_sPOS.insert(line);
		}
	}

	fin.close();
	return true;
}

bool CPOSTager::LoadWordPOS(const string &WordPOSFile)
{	
	ifstream fin(WordPOSFile.c_str());
	
	if (!fin)
	{
		cerr << "Can not open file: " << WordPOSFile << endl;
		return false;
	}

	string line;

	while (getline(fin, line))
	{
		istrstream is(line.c_str());
		string word;
		string POS;

		is >> word;

		while (is >> POS)
		{
			m_mWordPOS[word].insert(POS);
		}
	}

	fin.close();
	return true;
}

void CPOSTager::Decode(const string &sent, string &result)
{
	m_mItemBuf.clear();
	GenWordSeq(sent);
	Search();
	TraceBack(result);
}

void CPOSTager::TraceBack(string &result)
{
	result = "";

	int to = int(m_vWord.size()) - 3;
	int i;
	int best = 0;
	
	for (i = to; i >= 2; --i)
	{
		result = m_vWord[i] + "/" + m_vvPOSTag[i][best].POS + " " + result;
		best = m_vvPOSTag[i][best].prior;
	}
}

void CPOSTager::GenWordSeq(const string &sent)
{
	m_vWord.clear();
	m_vWord.push_back("<s>");
	m_vWord.push_back("<s>");

	istrstream is(sent.c_str());
	string word;
	
	while (is >> word)
	{
		m_vWord.push_back(word);
	}

	m_vWord.push_back("</s>");
	m_vWord.push_back("</s>");
}

double CPOSTager::EvalPOSTag(const int &idx,
							const CTagNode &preNode,
							const string &curPOS)
{
	const string &w_2 = m_vWord[idx - 2];
	const string &w_1 = m_vWord[idx - 1];
	const string &w = m_vWord[idx];
	const string &w1 = m_vWord[idx + 1];
	const string &w2 = m_vWord[idx + 2];
	const string &t_2 = preNode.POS_1;
	const string &t_1 = preNode.POS;
	const string &t = curPOS;
	
	/*string pre1 = Prefix(w, 1); 
	string pre2 = Prefix(w, 2); 
	string pre3 = Prefix(w, 3); 
	string pre4 = Prefix(w, 4);
	string suf1 = Suffix(w, 1);
	string suf2 = Suffix(w, 2);
	string suf3 = Suffix(w, 3);
	string suf4 = Suffix(w, 4);*/

//	string num = HasNumber(w) ? "Y" : "N";
//	string upp = HasUpper(w) ? "Y" : "N";
//	string hyp = HasHyphen(w) ? "Y" : "N";

/*	string allnum = IsAllNum(w)? "Y" : "N";
	string allletter = IsAllLetter(w)? "Y" : "N";
	string sinCharWord = SingleCharWord(w)? "y" : "N";
	string start = getStartChar(w);
	string end = getEndChar(w);
*/
	double score = 0.0;

	score += ItemScore("w_2t="+w_2+"|"+t);
	score += ItemScore("w_1t="+w_1+"|"+t);
	score += ItemScore("wt="+w+"|"+t);
	score += ItemScore("w1t="+w1+"|"+t);
	score += ItemScore("w2t="+w2+"|"+t);
	score += ItemScore("t_1t="+t_1+"|"+t);
	score += ItemScore("t_2t_1t="+t_2+"|"+t_1+"|"+t);
	/*score += ItemScore("pre1t=" + pre1 + "|" + t);
	score += ItemScore("pre2t=" + pre2 + "|" + t);
	score += ItemScore("pre3t=" + pre3 + "|" + t);
	score += ItemScore("pre4t=" + pre4 + "|" + t);
	score += ItemScore("suf1t=" + suf1 + "|" + t);
	score += ItemScore("suf2t=" + suf2 + "|" + t);
	score += ItemScore("suf3t=" + suf3 + "|" + t);
	score += ItemScore("suf4t=" + suf4 + "|" + t);*/

/*	score += ItemScore("allnum=" + allnum + "|" + t);
	score += ItemScore("allletter=" + allletter + "|" + t);
	score += ItemScore("scw=" + sinCharWord + "|" + t);
	score += ItemScore("start=" + start + "|" + t);
	score += ItemScore("end=" + end + "|" + t);
*/	

//	score += ItemScore("numt=" + num + "|" + t);
//	score += ItemScore("uppt=" + upp + "|" + t);
//	score += ItemScore("hypt=" + hyp + "|" + t);

	return score;
}

double CPOSTager::ItemScore(const string &item)
{
	map<string, double>::const_iterator it = m_mItemBuf.find(item);

	if (it != m_mItemBuf.end())
	{
		return it->second;
	}

	double score;

	score = m_pModel.PredWeight(item);

	m_mItemBuf[item] = score;

	return score;
}

void CPOSTager::Search()
{
	m_vvPOSTag.clear();
	m_vvPOSTag.resize(m_vWord.size());
	m_vvPOSTag[1].push_back(CTagNode("<s>", "<s>", 0.0, -1));

	int from = 2;
	int to = int(m_vWord.size()) - 3;
	int i;

	for (i = from; i <= to; ++i)
	{
		set<string> *pIncPOSs = GetIncPOSs(m_vWord[i]);
		const vector<CTagNode> &preTags = m_vvPOSTag[i - 1];
		vector<CTagNode> &curTags = m_vvPOSTag[i];
		int p;

		for (p = 0; p < int(preTags.size()); ++p)
		{
			const CTagNode &preNode = preTags[p];
			set<string>::const_iterator q;

			for (q = pIncPOSs->begin(); q != pIncPOSs->end(); ++q)
			{
				const string &curPOS = *q;
				double score = preNode.score + EvalPOSTag(i, preNode, curPOS);
				CTagNode curNode(preNode.POS, curPOS, score, p);

				SortInsert(curNode, curTags);
			}
		}
	}
}

set<string> *CPOSTager::GetIncPOSs(const string &word)
{
	map<string, set<string> >::iterator it = m_mWordPOS.find(word);

	if (it != m_mWordPOS.end())
	{
		return &(it->second);
	}
	else
	{
		return &m_sPOS; // 未登陆词的词性是所有可能的词性集合
	}
}

void CPOSTager::SortInsert(const CTagNode &cand, vector<CTagNode> &cands)
{
	if (cands.empty())
	{
		cands.push_back(cand);
		return;
	}

	int sum = int(cands.size());
	int p = sum - 1;

	if (sum == m_nHistogram && cand.score <= cands[p].score)
	{
		return;
	}

	while (p >= 0 && !cand.Equival(cands[p]))
	{
		--p;
	}

	if (p >= 0)
	{
		if (cand.score <= cands[p].score)
		{
			return;
		}
		else
		{
			--p;
		}
	}
	else
	{
		p = sum - 1;

		CTagNode spc;

		cands.push_back(spc);
	}

	while (p >= 0 && cand.score > cands[p].score)
	{
		cands[p + 1] = cands[p];
		--p;
	}

	++p;
	cands[p] = cand;

	while (int(cands.size()) > m_nHistogram)
	{
		cands.pop_back();
	}
}

string CPOSTager::Prefix(const string &word, const int &len)
{
	int maxLen = min(int(word.length()), len);

	return word.substr(0, maxLen);
}

string CPOSTager::Suffix(const string &word, const int &len)
{
	int maxLen = min(int(word.length()), len);

	return word.substr(int(word.length()) - maxLen, maxLen);
}

bool CPOSTager::HasNumber(const string &word)
{
	size_t i;

	for (i = 0; i < word.length(); ++i)
	{
		if (word[i] >= '0' && word[i] <= '9')
		{
			return true;
		}
	}

	return false;
}

bool CPOSTager::HasUpper(const string &word)
{
	size_t i;

	for (i = 0; i < word.length(); ++i)
	{
		if (word[i] >= 'A' && word[i] <= 'Z')
		{
			return true;
		}
	}

	return false;
}

bool CPOSTager::HasHyphen(const string &word)
{
	size_t i;

	for (i = 0; i < word.length(); ++i)
	{
		if (word[i] == '-')
		{
			return true;
		}
	}

	return false;
}

int CPOSTager::getWordLen(const string &str)
{
	int len = (int)str.size();
	int i = 0;
	int length = 0;
	
	while (i < len)
	{
		if (str[i] < 0)
		{
			i += 2;
			++length;
		}
		else
		{
			++i;
			++length;
		}
	}

	return length;
}

bool CPOSTager::SingleCharWord(const string &str)
{
	if(str == "<s>" || str == "</s>")
		return true;

	else if(str.size() == 1)
		return true;

	else if(str.size() == 2)
	{
		if(str[0] < 0 && str[1] < 0)
			return true;
	}

	return false;
}

string CPOSTager::getStartChar(const string &str)
{
	string start="";

	if(str == "<s>" || str == "</s>")
		start = str;
	else
	{
		start = str[0];

		if(str[0] < 0)
			start += str[1];
	}

	return start;
}

string CPOSTager::getEndChar(const string &str)
{
	string end="";

	if(str == "<s>" || str == "</s>")
		end = str;
	else
	{
		end = str[str.length()-1];

		if(str[str.length()-1] < 0)
			end = str[str.length()-2] + end;
	}

	return end;
}

//切分为单字序列
void CPOSTager::Split(const string &src, vector<string> &elems)
{
	elems.clear();

	int len = (int)src.size();
	int i = 0;

	while (i < len)
	{
		string ch;

		if (src[i] < 0)
		{
			ch = src[i];
			ch += src[i + 1];
			i += 2;
		}
		else
		{
			ch = src[i];
			++i;
		}

		elems.push_back(ch);
	}
}

bool CPOSTager::IsAllNum(const string &str)
{
	vector<string> elems;
	Split(str,elems);

	int len = (int)elems.size();
	int i;

	for(i=0;i<len;i++)
	{
		if(m_cChType.GetType(elems[i]) != 2)
			return false;
	}

	return true;
} 

bool CPOSTager::IsAllLetter(const string &str)
{
	vector<string> elems;
	Split(str,elems);

	int len = (int)elems.size();
	int i;

	for(i=0;i<len;i++)
	{
		if(m_cChType.GetType(elems[i]) != 1)
			return false;
	}

	return true;
}

