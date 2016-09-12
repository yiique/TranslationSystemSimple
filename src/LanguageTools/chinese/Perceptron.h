#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <string>
#include <vector>
#include <map>
#include "Ptrcmp.h"

using namespace std;

class CPerceptron
{
public:
	CPerceptron();
	~CPerceptron();
	bool Initialize(const string &model);
	void PredWeight(const vector<pair<string,int> > &feats, const int &tag, double &score);

protected:
	 
private:
	map<char*, vector<double>, ptrCmp> m_mPredWeight;
};

#endif
