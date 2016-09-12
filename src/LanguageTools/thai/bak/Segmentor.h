#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "Perceptron.h"
#include "DataStruct.h"
#include "ChType.h"
#include "BiSplite.h"

using namespace std;

namespace thaiSeg{
class CSegmentor
{
public:
	CSegmentor();
	~CSegmentor();
	bool Initialize(const string& p);
	void Segment(const string &src, string &tgt);
	void SubSegment(const string &src, string &tgt);
private:
	void Split(const string &src, vector<CTagElem> &elems);
	void BestPath(vector<CTagElem> &elems, const vector<map<string, double> > &scores);
	void TagWord(vector<CTagElem> &elems, const int &to, const int &len);
	void TraceSeg(vector<CTagElem> &elems, string &seg);
	void GenFeats(const vector<CTagElem> &elems, const int &cur, vector<string> &feats);
	double FeatsPred(const vector<string> &feats, const string &tag);
	void GetScores(const vector<vector<string> > &featSets, vector<map<string, double> > &scores);

	int m_nScanLen;
	
	BiSplite bisplit;

	CPerceptron m_cPerceptron;
	CChType m_cChType;
};
}


