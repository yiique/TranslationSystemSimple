#ifndef DECODER_H
#define DECODER_H

#include <string>
#include <vector>
#include <map>
#include "DataStruct.h"
#include "ChType.h"
#include "PerceptronPOS.h"

using namespace std;

class CPOSTager
{
public:
	CPOSTager();
	~CPOSTager();
	bool Initialize(const string &POSFile, const string &WordPOSFile, const string &ModelFlie, const int &histo);
	void Decode(const string &sent, string &result);

private:
	bool LoadPOS(const string &POSFile);
	bool LoadWordPOS(const string &WordPOSFile);
	void GenWordSeq(const string &sent);
	void GenViterbiSeq();
	void EnumPOSTag(const int &idx, vector<CTagNode> &tagNodes);
	double EvalPOSTag(const int &idx,
					  const CTagNode &preNode,
					  const string &curPOS);
	double ItemScore(const string &item);
	void Search();
	void SortInsert(const CTagNode &cand, vector<CTagNode> &cands);
	void TraceBack(string &result);
	string Prefix(const string &word, const int &len);
	string Suffix(const string &word, const int &len);
	bool HasNumber(const string &word);
	bool HasUpper(const string &word);
	bool HasHyphen(const string &word);
	int getWordLen(const string &str);
	bool SingleCharWord(const string &str);
	string getStartChar(const string &str);
	string getEndChar(const string &str);
	void Split(const string &src, vector<string> &elems);
	bool IsAllNum(const string &str);
	bool IsAllLetter(const string &str);
	set<string> *GetIncPOSs(const string &word);

	vector<string> m_vWord;
	vector<vector<CTagNode> > m_vvPOSTag;
	map<string, double> m_mItemBuf;
	int m_nHistogram;
	CPerceptronPOS m_pModel;
	set<string> m_sPOS;
	map<string, set<string> > m_mWordPOS;
	CChType m_cChType;
};

#endif
