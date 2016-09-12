#ifndef TRAINER
#define TRAINER

#include <string>
#include <vector>
#include <fstream>
#include <set>
#include "Perceptron.h"
#include "DataStruct.h"
#include "ChType.h"
#include "splitutf.h"

using namespace std;

namespace titoken{

class CSegmentor
{
public:
	CSegmentor();
	~CSegmentor();
	bool Initialize(const string &path);
	void token(const string &src,string &tgt);
	
	string strtrim(const string &s);

private:
	void Split(const vector<string> &src, vector<CTagElem> &elems);
	void BestPath(vector<CTagElem> &elems, vector<map<string, double> > &scores,string &tgt);
	void TagWord(vector<CTagElem> &elems, const int &to, const int &len);
	void TraceSeg(vector<CTagElem> &elems, string &seg);
	void GenFeats(const vector<CTagElem> &elems, const int &cur, vector<string> &feats);
	double FeatsPred(const vector<string> &feats, const string &tag);
	void GetScores(const vector<vector<string> > &featSets, vector<map<string, double> > &scores);
	string &trim(string &s);
	
	void Segment(const vector<string> &src, string &tgt);

	int m_nScanLen;

	set<string> dict;
	int indegree;

	CPerceptron m_cPerceptron;
	CChType m_cChType;

	splitutf sp;
};
}
#endif

