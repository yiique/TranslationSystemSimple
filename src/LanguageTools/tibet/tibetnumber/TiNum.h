#pragma once
#include "utility.h"
#include <limits.h>
#include "splitutf.h"
#include "NewUyNERec.h"
#include <functional>
#include <algorithm>
#include <cassert>

using namespace UtilityTool;
using namespace std;
namespace TiNumber
{


class chArb
{
public:
	string ch;
	int arb;
	chArb(string &chstr,int arbnum):ch(chstr),arb(arbnum){}
	chArb(){}
};



//class NewUyNERec;
class scorecmp
{
public:
	bool operator () (const transPair &a,const transPair &b) const
	{

		return a.beg < b.beg;
	}
};

class TiNum
{
public:
	TiNum(void);
	bool initialize(const string &path);
	void process(const string &src,string &tgt,vector<transPair> &tps,vector<transPair> &ctps,string &pseudoSen,string &medSen);
	void basicRec(string &tgt);
	void numRec(vector<string> &sen,vector<transPair> &tps);//�������ģ���ҵ������ִʶ��滻��ͳһ�ı�־num��������һ��ģ��ֱ����������ʽ
	void ordinalTimeRec(string &tgt);
	void tokenSen(const string &src,vector<string> &sen,vector<transPair> &tps,string &pseudoSen);
	void shortPath(vector<string> &slm,vector<string> &sen);
	string Vector2String(vector<string> &slm,int i,int j);
	void composedNum(string &pseudoSen,string &medStr,string &tgt,vector<transPair> &tps,vector<transPair> &ctps);
	void mergeTps(vector<transPair> &tps,vector<transPair> &ctps, string &pseudoSen);
	void extraRex(string &pseudoSen,vector<transPair> &tps);
	void post(string &tgt);
	void addBlank(string &tgt);
	void EraseTps(vector<transPair> &tps);
	void IsTenTps(vector<transPair> &tps);

	set<string> getDic()
	{
		return dicNum;
	}
	~TiNum(void);
private:
	map<string,chArb> sl;
	map<string,int> js;
	map<string,int> pre;
	set<string> suffix;//�������ʺ�����ܻ��к�׺
	set<string> dot;
	set<string> linker;
	set<string> noNum;
	map<string,string> ti2ch;//��������̰߳�ȫ��,��¼���г��ֵ����ʺ��䷭��
	set<string> dicNum;
	set<string> dicTerm;
	string sdot;
	map<string,int> bsc;
	string sep;
	int maxSlm;//�������ֵ�Ԫ�����������Ŀ

	NewUyNERec rex;

	string extraStr;

	splitutf spf;//for token


	

};

}
