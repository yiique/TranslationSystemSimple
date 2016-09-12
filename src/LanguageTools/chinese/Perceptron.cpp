#include <fstream>
#include <iostream>
#include <strstream>
#include <cstdlib>
#include "Perceptron.h"

CPerceptron::CPerceptron()
{
}

CPerceptron::~CPerceptron()
{
}

//初始化
bool CPerceptron::Initialize(const string &model)
{
	ifstream fin(model.c_str());
	if (!fin)
	{
		cout << "Can not open model file: " << model << endl;
		return false;
	}

	string line;

	while (getline(fin, line))
	{
		istrstream is(line.c_str());
		string pred,word;

		is >> pred;

		double weight;
		vector<double> weightVal;

		while(is >> word)
		{
			weight = atof(word.c_str());
			weightVal.push_back(weight);
		}

		char *ch = (char *)malloc((int(pred.size())+1)*sizeof(char));
		strcpy(ch,pred.c_str());
		m_mPredWeight.insert(map<char*, vector<double>, ptrCmp>::value_type(ch, weightVal));
	
	}

	fin.close();
	return true;
}

//获取该推断的分值
void CPerceptron::PredWeight(const vector<pair<string,int> > &feats, const int &tag, double &score)
{
	int len;

	len = int(feats.size());

	for(int i=0; i<len; i++)
	{
		char *ch = (char *)malloc((int(feats[i].first.size())+1)*sizeof(char));
		strcpy(ch,feats[i].first.c_str());

		map<char*, vector<double>, ptrCmp>::iterator it = m_mPredWeight.find(ch);

 		if (it != m_mPredWeight.end())
		{
			score += it->second[tag + 4 * feats[i].second];			
		}

		free(ch);
	}
}
