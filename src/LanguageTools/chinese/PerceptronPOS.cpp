#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "PerceptronPOS.h"

CPerceptronPOS::CPerceptronPOS()
{
}

CPerceptronPOS::~CPerceptronPOS()
{
}

//初始化
bool CPerceptronPOS::Initialize(const string &model)
{
	ifstream fin(model.c_str());

	if (!fin)
	{
		cout << "Can not open file: " << model << endl;
		return false;
	}

	string line;

	while (getline(fin, line))
	{
		size_t split = line.find('\t');
		string pred = line.substr(0, split);
		string weight = line.substr(split + 1, line.length() - split - 1);
		double weightVal = atof(weight.c_str());

		char *ch = (char *)malloc((int(pred.size())+1)*sizeof(char));
		strcpy(ch,pred.c_str());
		m_mPredWeight.insert(map<char*, double, ptrCmp>::value_type(ch, weightVal));
	}

	fin.close();
	return true;
}

//获取该推断的分值
double CPerceptronPOS::PredWeight(const string &pred)
{
	char *ch = (char *)malloc((int(pred.size())+1)*sizeof(char));
	strcpy(ch,pred.c_str());
	map<char *, double, ptrCmp>::iterator it = m_mPredWeight.find(ch);

	if (it != m_mPredWeight.end())
	{
		return it->second;
	}
	else
	{
		return 0.0;
	}
	
	free(ch);
}
