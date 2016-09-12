#ifndef PERCEPTRON
#define PERCEPTRON

#include <string>
#include <vector>
#include <map>
#include "Ptrcmp.h"

using namespace std;

class CPerceptronPOS
{
public:
	CPerceptronPOS();
	~CPerceptronPOS();
	bool Initialize(const string &model);
	double PredWeight(const string &pred);

private:
	map<char*, double, ptrCmp> m_mPredWeight;
};

#endif
