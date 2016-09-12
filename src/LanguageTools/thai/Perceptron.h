#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdlib.h>

using namespace std;
 namespace thaiSeg{
class CPerceptron
{
public:
	CPerceptron();
	~CPerceptron();
	bool Initialize(const string &model);
	double PredWeight(const string &pred);

private:
	map<string, double> m_mPredWeight;

};
}


