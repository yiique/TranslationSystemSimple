#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include <vector>
#include <fstream>
#include "Perceptron.h"
#include "ChType.h"
#include "DataStruct.h"
#include "Dictionary.h"
#include "POSTager.h"

using namespace std;

class CPBCLAS
{
public:
	CPBCLAS();
	~CPBCLAS();
	bool Initialize(const string & path, const string &tag, const double ddict = 0.0);
    void Generate(const string &src, 
                  string &best, 
                  const string &tag,
    			  const set<string> &useDict);
    bool InsertEntry(const string &s, set<string> &useDict);
    bool DeleteEntry(const string &s, set<string> &useDict);

private:
    void GenerateSplit(const string &src, string &best,const string &tag);
    void SplitByUserDict(const vector<string> &vsrc, vector<pair<string,int> > &vsrcproc, const set<string> &useDict);
    void SplitByRule(vector<pair<string,int> > &vsrcproc);
    void procSNP(const string &src, vector<pair<string,int> > &temp);
    void InitBounds();
	void Split(const string &src, vector<CTagElem> &elems);
    void Split(const string &src, vector<string> &elems);
	void BestEdges(vector<CTagElem> &elems, 
                   vector<map<string, double> > &scores, 
				   const map<string,int> &sClass);
	string TraceSeg(vector<CTagElem> &elems, vector<map<string, double> > &scores);
	void MarkBnd(vector<CTagElem> &elems, const int &to, const int &len);
	void GenFeats(const vector<CTagElem> &elems, const int &cur,  vector<pair<string,int> > &feats);
	double FeatsPred(const vector<pair<string,int> > &feats, const int &tag);
	void GetScores(const vector<vector<pair<string,int> > > &featSets, vector<map<string, double> > &scores);
	void AtomSeg(const vector<CTagElem> &elems,map<string,int> &sClass,vector<CTagElem> &elemnew);
	int charType(const string &s);
	int InsClass(const string &s, const map<string,int> &sClass);
	bool isCHNum(const string &s);
	bool isNumCi(const string &s);
	string SegPostProc(const string &str);

	int m_nScanLen;
	int m_nBndCnt;
	int m_nPOSCnt;

	CPerceptron m_cPerceptron;
	CPerceptronPOS m_cPerceptronPos;
	CChType m_cChType;
	vector<string> m_vPOS;
	vector<string> m_vBound;
	CDictionary m_Dict;
   // CDictionary m_userDict;
    set<string> nAlph; //1-9 a-z A-Z

	CPOSTager posTager;
	double dictWeight;
};

#endif



