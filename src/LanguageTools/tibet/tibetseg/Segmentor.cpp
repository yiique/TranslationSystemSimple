#include <strstream>
#include <fstream>
#include <iostream>
#include <strstream>
#include <queue>
#include "Segmentor.h"

using namespace titoken;

CSegmentor::CSegmentor()
{
}

CSegmentor::~CSegmentor()
{
}

void CSegmentor::token(const string &src,string &tgt)
{
	string line = strtrim(src);
	if(line.empty())
	{
		tgt = "\n";
		return;
	}
	string seg;
	vector<pair<string,bool> > blocklist;
	sp.preSplit(line,blocklist);
	
	
	for(int i=0;i<blocklist.size();)
	{

		if(blocklist[i].second)
		{
			seg +=" "+blocklist[i].first;
			++i;
		}
		else
		{
			string block,temp;
			vector<string> vword;
			while(i<blocklist.size() && !blocklist[i].second)
			{
				vword.push_back(blocklist[i].first);
				++i;
			}
			Segment(vword, temp);
			seg +=" "+temp;

		}


	}
	string rst;
	tgt= trim(seg);

	
}

string  CSegmentor::strtrim(const string &s)
{
		string tgt="";
        size_t beg = s.find_first_not_of(" \r\t\n");
        if(beg == string::npos)
			tgt = "";
        //s = "";
        else
        {
             size_t end = s.find_last_not_of(" \r\t\n");
             tgt = s.substr(beg,end+1-beg);
        }

        return tgt;
}

//初始化
bool CSegmentor::Initialize(const string &path)
{
	/*if (!m_cChType.Initialize("data/punc", "data/alph", "data/date", "data/num"))
	{
		return false;
	}*/
	
	string p = path;
	string pathmode = p+"/data/model";
	string pathdata = p+"/data/data.txt";
	if (!m_cPerceptron.Initialize(pathmode))
	{
		return false;
	}

	ifstream fin(pathdata.c_str());

	string line;
	while(getline(fin,line))
	{
		dict.insert(trim(line));
	}
	fin.close();
	//最大可能词长
	m_nScanLen = 10;//34

    indegree = 5;
	sp.Initialize(path);

	return true;
}
string & CSegmentor::trim(string &s)
{
	size_t beg = s.find_first_not_of(" \t\n\r");
	if(beg == string::npos)
	{
		s = "";
		return s;
	}

	size_t end = s.find_last_not_of(" \t\n\r");
	s = s.substr(beg,end-beg+1);
	return s;
}
//切分
void CSegmentor::Segment(const vector<string> &src, string &tgt)
{
	tgt.clear();

	vector<CTagElem> elems;
	Split(src, elems);
	
	//生成特征
	vector<vector<string> > featSets;
	vector<string> nullFeats;
	featSets.push_back(nullFeats);
	featSets.push_back(nullFeats);
	featSets.push_back(nullFeats);//feat
	featSets.push_back(nullFeats);//feat

	int len = (int)elems.size();
	int i;
	int from = 4;//feat
	int to = len - 5;//feat

	for (i = from; i <= to; ++i)
	{
		vector<string> feats;
		GenFeats(elems, i, feats);
		featSets.push_back(feats);
	}

	featSets.push_back(nullFeats);
	featSets.push_back(nullFeats);
	featSets.push_back(nullFeats);//feat
	featSets.push_back(nullFeats);//feat

	//
	//ofstream fo("log");

	//for(i =0;i<featSets.size();++i)
	//{
	//	for(int j=0;j<featSets[i].size();++j)
	//		fo<<featSets[i][j]<<endl;
	//}
	//fo.close();
	//

	//求取所需得分
	vector<map<string, double> > scores;
	GetScores(featSets, scores);
	//
	//ofstream fo1("score");
	//for(i = 0;i<scores.size();++i)
	//{
	//	for(map<string,double>::iterator it = scores[i].begin();it!=scores[i].end();++it)
	//		fo1<<it->first<<"---"<<it->second<<endl;
	//}
	//fo1.close();
	//

	//规划最佳路径
	BestPath(elems, scores,tgt);

	//回溯切分结果
	//TraceSeg(elems, tgt);
}

//切分为单字序列
void CSegmentor::Split(const vector<string> &src, vector<CTagElem> &elems)
{
	elems.clear();

	int i;
	for (i = 0; i < 4; ++i)//feat
	{
		CTagElem tip("<s>", -1);
		elems.push_back(tip);
	}

	for(i =0;i<src.size();++i)
	{
		

		int type = 0;
		CTagElem elem(src[i], type);

		elems.push_back(elem);
		
	}

	for (i = 0; i < 4; ++i)//feat
	{
		CTagElem tip("</s>", -1);
		elems.push_back(tip);
	}
}

//生成该环境下的特征
void CSegmentor::GenFeats(const vector<CTagElem> &elems, const int &cur, vector<string> &feats)
{
	/*feats.clear();

	char feat[32];
	int n;

	//Cn
	for (n = -2; n <= 2; ++n)
	{
		sprintf(feat, "C%d==%s", n, elems[cur + n].ch.c_str());
		feats.push_back(feat);
	}

	//CnCn+1
	for (n = -2; n <= 1; ++n)
	{
		sprintf(feat, "C%dC%d==%s%s", n, n + 1, elems[cur + n].ch.c_str(), elems[cur + n + 1].ch.c_str());
		feats.push_back(feat);
	}

	//C-1C1
	sprintf(feat, "C-1C1==%s%s", elems[cur - 1].ch.c_str(), elems[cur + 1].ch.c_str());
	feats.push_back(feat);

	//Pu(C0)
	sprintf(feat, "PuC0==%d", elems[cur].type == 0 ? 1 : 0);
	feats.push_back(feat);

	//T(C-2)T(C-1)T(C0)T(C1)T(C2)
	sprintf(feat, "TC-22==%d%d%d%d%d", elems[cur - 2].type, elems[cur - 1].type, elems[cur].type, elems[cur + 1].type, elems[cur + 2].type);
	feats.push_back(feat);*/
	int n;
	feats.clear();
	int maxlen=0;
	for(n= -4;n<=4;++n)
	{
		maxlen +=elems[cur+n].ch.size();

	}
	maxlen +=elems[cur].ch.size();
	maxlen +=50;
	char *feat = new char[maxlen];
	//memset(feat, 0, maxlen);


	//char feat[10000];
	//cout<<++nn<<" maxlen "<<maxlen<<endl;

	//Cn
	for (n = -4; n <= 4; ++n)//feat
	{
		sprintf(feat, "C%d==%s", n, elems[cur + n].ch.c_str());
		feats.push_back(feat);
	}

	//CnCn+1
	for (n = -2; n <= 1; ++n)
	{
		sprintf(feat, "C%dC%d==%s%s", n, n + 1, elems[cur + n].ch.c_str(), elems[cur + n + 1].ch.c_str());
		feats.push_back(feat);
	}
	
	//C-1C1
	sprintf(feat, "C-1C1==%s%s", elems[cur - 1].ch.c_str(), elems[cur + 1].ch.c_str());
	feats.push_back(feat);

	
	//CnCn+1Cn+2Cn+3
	for(n= -4;n<=1;++n)
	{
		sprintf(feat, "C%dC%dC%dC%d==%s%s%s%s", n, n + 1,n+2,n+3, elems[cur + n].ch.c_str(), elems[cur + n + 1].ch.c_str(),elems[cur + n + 2].ch.c_str(),elems[cur + n + 3].ch.c_str());
		feats.push_back(feat);
	}

	sprintf(feat, "C-2C-1C1C2==%s%s%s%s", elems[cur - 2].ch.c_str(),elems[cur - 1].ch.c_str(), elems[cur + 1].ch.c_str(), elems[cur + 2].ch.c_str());
	/////////////////////////////Co

	//CoCn
	for (n = -4; n <= 4; ++n)//feat
	{
		sprintf(feat, "C0C%d==%s%s", n,elems[cur].ch.c_str(), elems[cur + n].ch.c_str());
		feats.push_back(feat);
	}

	//CoCnCn+1
	for (n = -2; n <= 1; ++n)
	{
		sprintf(feat, "C0C%dC%d==%s%s%s", n, n + 1,elems[cur].ch.c_str(), elems[cur + n].ch.c_str(), elems[cur + n + 1].ch.c_str());
		feats.push_back(feat);
	}
	
	//CoC-1C1
	sprintf(feat, "C0C-1C1==%s%s%s",elems[cur].ch.c_str() ,elems[cur - 1].ch.c_str(), elems[cur + 1].ch.c_str());
	feats.push_back(feat);

	//CoCnCn+1Cn+2Cn+3
	for(n= -4;n<=1;++n)
	{
		sprintf(feat, "C0C%dC%dC%dC%d==%s%s%s%s%s", n, n + 1,n+2,n+3,elems[cur].ch.c_str() ,elems[cur + n].ch.c_str(), elems[cur + n + 1].ch.c_str(),elems[cur + n + 2].ch.c_str(),elems[cur + n + 3].ch.c_str());
		//cout<<" "<<strlen(feat)<<" ";
		feats.push_back(feat);

	}
	//cout<<endl;

	sprintf(feat, "C0C-2C-1C1C2==%s%s%s%s%s", elems[cur].ch.c_str(),elems[cur - 2].ch.c_str(),elems[cur - 1].ch.c_str(), elems[cur + 1].ch.c_str(), elems[cur + 2].ch.c_str());


	//feat
	feats.push_back(feat);
	


	delete []feat;
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
	int lb = 4;//feat
	int rb = len - 5;//feat
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
void CSegmentor::BestPath(vector<CTagElem> &elems, vector<map<string, double> > &scores,string &tgt)
{
	//准备规划
	/*elems[3].best = 0.0;//feat

	//规划非首尾部分
	int lb = 4;//feat
	int rb = (int)elems.size() - 5;//feat
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
	}*/


	elems[3].best = 0.0;//feat

	//规划非首尾部分
	int lb = 4;//feat
	int rb = (int)elems.size() - 5;//feat
	int i;

	int Len = rb - lb+1;
	vector<vector<cand > > lattice;
	lattice.resize(Len);

	for (i = lb; i <= rb; ++i)
	{
		bool first = true;
		int dis;
		
		priority_queue<cand,vector<cand>, cmp> cands;


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

			cand c1;
			c1.from = i-dis-4;
			c1.to = i-4;
			c1.score = score;
			c1.spanScore = score - elems[i - dis - 1].best;

			string dicterm;

			for(int qq=i-dis;qq<=i;++qq)
			{
				dicterm +=elems[qq].ch;
			}

			char dotchar[4]={0xe0,0xbc,0x8b,0};
			string dot = string(dotchar);
			if(dicterm.size()>=3 && dicterm.substr(dicterm.size()-3,3)==dot)
				dicterm = dicterm.substr(0,dicterm.size()-3);
			if(dict.find(dicterm)!=dict.end())
			{
				//cout<<"find"<<endl;
				c1.path = 1;
			}
			else
			{
				//cout<<"no"<<endl;
				c1.path = 7;
			}

			if(cands.size()>=indegree)
			{
				if(c1.score>cands.top().score)
				{
					cands.pop();
					cands.push(c1);
				}
			}
			else
			{
				cands.push(c1);
			}

			

			//更新最佳值
			if (first || score > elems[i].best)
			{
				elems[i].best = score;
				elems[i].length = dis + 1;
			}

			first = false;
		}//for (dis = 0; dis < m_nScanLen && i - dis >= lb; ++dis)
		int num = cands.size();
		lattice[i-4].resize(num);
		int j = num-1;
		if(j<0)
		{
			cerr<<"no nbest"<<endl;
			exit(1);
		}
		for(;j>=0;--j)
		{
			lattice[i-4][j] = cands.top();
			cands.pop();
		}
		



	}

	vector<pair<int,int> > path;
	path.resize(Len);

	for(int i=0;i<Len;++i)
	{
		path[i].first = 100000;
		for(int j=0;j<lattice[i].size();++j)
		{
			int pre = lattice[i][j].from-1;
			int score;
			if(pre >=0)
				score = lattice[i][j].path+path[pre].first;
			else
				score = lattice[i][j].path;
			if(score <path[i].first)
			{
				path[i].first = score;
				path[i].second = lattice[i][j].to - lattice[i][j].from +1;
			}
		}
	}

	int pre = Len - path[Len-1].second -1;
	tgt = "";
	int cur = Len-1;
	while(pre>=0)
	{
		tgt = " "+tgt;
		for(int i=cur;i>=pre+1;--i)
			tgt = elems[i+4].ch+tgt;

		cur = pre;
		pre = cur - path[cur].second;
	}

	tgt = " "+tgt;
	for(int i=cur;i>=0;--i)
		tgt = elems[i+4].ch+tgt;





}

//根据最佳路径回溯tag序列
void CSegmentor::TraceSeg(vector<CTagElem> &elems, string &seg)
{
	seg.clear();

	int from = 4;//feat
	int p = static_cast<int>(elems.size() - 5);//feat

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
			seg = "  " + seg;
		}
	}
}
