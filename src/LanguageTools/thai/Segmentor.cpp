#include <strstream>
#include <fstream>
#include <iostream>
#include <strstream>
#include "Segmentor.h"

using  namespace thaiSeg;
CSegmentor::CSegmentor()
{
}

CSegmentor::~CSegmentor()
{
}

inline string trim(const string &s)
{
	string t = "";
	size_t beg = s.find_first_not_of(" \t\n\r");

	if(beg != string::npos)
	{
		size_t end = s.find_last_not_of(" \t\n\r");
		t = s.substr(beg,end+1-beg);
	}
	return t;
}
//初始化
bool CSegmentor::Initialize(const string& path )
{
	if (!m_cChType.Initialize(path+"/data/punc",path+ "/data/alph",path+ "/data/date", path+"/data/num"))
	{
		return false;
	}

	if (!m_cPerceptron.Initialize(path + "/data/model"))
	{
		return false;
	}

	//最大可能词长
	m_nScanLen = 30;
	
	//bisplit.Initialize();

	return true;
}

//切分
void CSegmentor::Segment(const string &src, string &tgt)
{
	tgt = "";

	string sub;
	istrstream is(src.c_str());

	while(is >> sub)
	{
		string subBest;

		SubSegment(sub,subBest);
		tgt += subBest + " ";
	}

	tgt = trim(tgt);
}

//切分子句
void CSegmentor::SubSegment(const string &src, string &tgt)
{
	tgt.clear();

	vector<CTagElem> elems;
	
	Split(src, elems);

	
	//生成特征
	vector<vector<string> > featSets;
	vector<string> nullFeats;
	featSets.push_back(nullFeats);
	featSets.push_back(nullFeats);
    featSets.push_back(nullFeats);
    featSets.push_back(nullFeats);

	int len = (int)elems.size();
	int i;
	int from = 4;
	int to = len - 5;

	for (i = from; i <= to; ++i)
	{
		vector<string> feats;
		GenFeats(elems, i, feats);
		featSets.push_back(feats);
	}

    featSets.push_back(nullFeats);
    featSets.push_back(nullFeats);
	featSets.push_back(nullFeats);
	featSets.push_back(nullFeats);

	//求取所需得分
	vector<map<string, double> > scores;
	GetScores(featSets, scores);

	//规划最佳路径
	BestPath(elems, scores);

	//回溯切分结果
	TraceSeg(elems, tgt);
}

//切分为单字序列
void CSegmentor::Split(const string &src, vector<CTagElem> &elems)
{
	elems.clear();

	int i;
	for (i = 0; i < 4; ++i)
	{
		CTagElem tip("<s>", -1);
		elems.push_back(tip);
	}
	
	string ssp, ssrc = src;
	//ssp = bisplit.MoSplit(ssrc);
    ssp = m_tcc.tok(ssrc);
	istrstream is(ssp.c_str());
	string word;

	while (is >> word)
	{
		int type = m_cChType.GetType(word);
		CTagElem elem(word, type);

		elems.push_back(elem);
	}

	for (i = 0; i < 4; ++i)
	{
		CTagElem tip("</s>", -1);
		elems.push_back(tip);
	}
}

/*
void CSegmentor::Split(const string &src, vector<CTagElem> &elems)
{
	elems.clear();

	int i;
	for (i = 0; i < 2; ++i)
	{
		CTagElem tip("<s>", -1);
		elems.push_back(tip);
	}

	istrstream is(src.c_str());
	string ssrc;
	bool first = true;

	while(is >> ssrc)
	{
		if(first == false)
		{
			string word = " ";
			int type = m_cChType.GetType(word);
			CTagElem elem(word, type);

			elems.push_back(elem);
		}

		first = false;

		string ssp = bisplit.MoSplit(ssrc);

		istrstream iss(ssp.c_str());
		string word;

		while (iss >> word)
		{
			int type = m_cChType.GetType(word);
			CTagElem elem(word, type);

			elems.push_back(elem);
		}
	}

	for (i = 0; i < 2; ++i)
	{
		CTagElem tip("</s>", -1);
		elems.push_back(tip);
	}
}
*/

//生成该环境下的特征
void CSegmentor::GenFeats(const vector<CTagElem> &elems, const int &cur, vector<string> &feats)
{
	feats.clear();

	int maxlen=0;
	for(int n= -4;n<=4;++n)
	{
		maxlen +=elems[cur+n].ch.size();

	}
	maxlen +=elems[cur].ch.size();
	maxlen +=50;
	char *feat = new char[maxlen];

	//char feat[500];
	int n;

	//Cn
	for (n = -4; n <= 4; ++n)
	{
		sprintf(feat, "C%d==%s", n, elems[cur + n].ch.c_str());
		feats.push_back(feat);
	}

	//CnCn+1Cn+2Cn+3Cn+4
	for (n = -4; n <= 0; ++n)
	{
		sprintf(feat, "C%dC%dC%dC%dC%d==%s%s%s%s%s", n, n + 1, n + 2, n + 3, n + 4, 
                elems[cur + n].ch.c_str(), elems[cur + n + 1].ch.c_str(),
                elems[cur + n + 2].ch.c_str(), elems[cur + n + 3].ch.c_str(),
                elems[cur + n + 4].ch.c_str());
		feats.push_back(feat);
	}

    //n CnCn+1
    for(n = -4; n <=3 ;++n){
        sprintf(feat, "C%dC%d==%s%s", n, n + 1, elems[cur + n].ch.c_str(), elems[cur + n + 1].ch.c_str());
        feats.push_back(feat);
    }

    
	//n C-1C1
	sprintf(feat,"C-1C1==%s%s",elems[cur-1].ch.c_str(),elems[cur+1].ch.c_str());
	feats.push_back(feat);
    
    //n C-2C-1C1C2
    sprintf(feat, "C-2C-1C1C2==%s%s%s%s", elems[cur - 2].ch.c_str(),elems[cur - 1].ch.c_str(), elems[cur + 1].ch.c_str(), elems[cur + 2].ch.c_str());
    feats.push_back(feat);

    //n C0Cn
    for (n = -4; n <= 4; ++n)//feat
    {
        sprintf(feat, "C0C%d==%s%s", n,elems[cur].ch.c_str(), elems[cur + n].ch.c_str());
        feats.push_back(feat);
    }

    //n CoCnCn+1
    for (n = -2; n <= 1; ++n)
    {
        sprintf(feat, "C0C%dC%d==%s%s%s", n, n + 1,elems[cur].ch.c_str(), elems[cur + n].ch.c_str(), elems[cur + n + 1].ch.c_str());
        feats.push_back(feat);
    }

    //n CoC-1C1
    sprintf(feat, "C0C-1C1==%s%s%s",elems[cur].ch.c_str() ,elems[cur - 1].ch.c_str(), elems[cur + 1].ch.c_str());
    feats.push_back(feat);

	//Pu(C0)
	sprintf(feat, "PuC0==%d", elems[cur].type == 0 ? 1 : 0);
	feats.push_back(feat);

	//T(C-4)...T(C4)
	sprintf(feat, "TC-44==%d%d%d%d%d%d%d%d%d", elems[cur - 4].type, elems[cur - 3].type, 
            elems[cur - 2].type, elems[cur - 1].type, elems[cur].type, elems[cur + 1].type, 
            elems[cur + 2].type, elems[cur + 3].type, elems[cur + 4].type);
	feats.push_back(feat);

	delete [] feat;
}

//求该特征集映射到tag的得分
double CSegmentor::FeatsPred(const vector<string> &feats, const string &tag)
{
	double score = 0.0;
	int cnt = (int)feats.size();
	int k;

	for (k = 0; k < cnt; ++k)
	{
		string pred = feats[k] + "=>" + tag;
		score += m_cPerceptron.PredWeight(pred);
	}

	return score;
}

//求各个Span的分数
void CSegmentor::GetScores(const vector<vector<string> > &featSets, vector<map<string, double> > &scores)
{
	scores.clear();

	int len = (int)featSets.size();
	int lb = 4;
	int rb = len - 5;
	int i;

	scores.resize(len);

	for (i = lb; i <= rb; ++i)
	{
		scores[i].insert(map<string, double>::value_type("b", FeatsPred(featSets[i], "b")));
		scores[i].insert(map<string, double>::value_type("m", FeatsPred(featSets[i], "m")));
		scores[i].insert(map<string, double>::value_type("e", FeatsPred(featSets[i], "e")));
		scores[i].insert(map<string, double>::value_type("s", FeatsPred(featSets[i], "s")));
	}
}

//标注以to结尾长度len的词
void CSegmentor::TagWord(vector<CTagElem> &elems, const int &to, const int &len)
{
	int from = to - len + 1;
	int p;

	//标注该词段
	for (p = from; p <= to; ++p)
	{
		if (p == from)
		{
			if (len == 1)
			{
				elems[p].tag = "s";
			}
			else
			{
				elems[p].tag = "b";
			}
		}
		else
		{
			if (p == to)
			{
				elems[p].tag = "e";
			}
			else
			{
				elems[p].tag = "m";
			}
		}
	}
}

//动态规划最佳路径
void CSegmentor::BestPath(vector<CTagElem> &elems, const vector<map<string, double> > &scores)
{
	//准备规划
	elems[3].best = 0.0;

	//规划非首尾部分
	int lb = 4;
	int rb = (int)elems.size() - 5;
	int i;

	for (i = lb; i <= rb; ++i)
	{
		bool first = true;
		int dis;

		//[i - dis, i]的串
		for (dis = 0; dis < m_nScanLen && i - dis >= lb; ++dis)
		{

			//标注该词段
			TagWord(elems, i, dis + 1);

			//该方案得分
			double score = elems[i - dis - 1].best;
			int p;

			for (p = i - dis; p <= i; ++p)
			{
				map<string, double>::const_iterator it  = scores[p].find(elems[p].tag);
				score += it->second;
			}

			//更新最佳值
			if (first || score > elems[i].best)
			{
				elems[i].best = score;
				elems[i].length = dis + 1;
			}

			first = false;
		}
	}
}

//根据最佳路径回溯tag序列
void CSegmentor::TraceSeg(vector<CTagElem> &elems, string &seg)
{
	seg.clear();

	int from = 4;
	int p = static_cast<int>(elems.size() - 5);

	while (p >= from)
	{
		int len = elems[p].length;
		int k;

		for (k = 0; k < len; ++k)
		{
			seg = elems[p].ch + seg;
			--p;
		}
	
		if (p >= from)
		{
			seg = " " + seg;
		}
	}
}
