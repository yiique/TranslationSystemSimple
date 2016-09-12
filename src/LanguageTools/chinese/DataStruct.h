#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <string>
#include <vector>
#include <set>

using namespace std;

//±ê×¢ÔªËØ
class CTagElem
{
public:
    string ch;
    int type;
    string bnd;
	string pos;
	double bestpos;
    double best;
    int length;
	int elemsClass;

    CTagElem(const string &chVal, const int &typeVal, const int sclass)
    {
        ch = chVal;
        type = typeVal;
		elemsClass = sclass;
    }
};

class CTagNode
{
public:
	string POS_1;
	string POS;
	double score;
	int prior;

	CTagNode(const string &t_1 = "",
					   const string &t = "",
					   const double &s = -1000000.0,
					   const int &p = -1)
	{
		POS_1 = t_1;
		POS = t;
		score = s;
		prior = p;
	}

	bool operator < (const CTagNode &node2) const
	{
		return score < node2.score;
	}

	bool Equival(const CTagNode &node2) const
	{
		return POS_1 == node2.POS_1 && POS == node2.POS;
	}
};

class CInst
{
public:
	vector<string> words;
	vector<string> POSs;
};

#endif

