#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include "../../deelx.h"
#include "NewUyNERec.h"
#include "TiNum.h"
#include <sstream>

using namespace  std;


namespace TiNumber
{




class NumberPair
{
public:
	bool Initialize(const string &path)
	{
		return	m_tiNum.initialize(path);
	}
	/*string SetBlank( string &line,vector<int> &tb)//line是只用一个空格隔开的句子，返回无空格的句子
	{
		stringstream ss;
		ss<<line;
		string part;
		int beg = 0;
		string rst;
		line = "";
		while (ss>>part)
		{
			line +=part+" " ;
			rst +=part;
			tb[beg] = part.size();
			beg +=part.size();
		}
		line = line.erase(line.size() - 1);
		return rst;
	}*/

	/*bool GetSrc(const string &line,string &src,const vector<int> &tb )
	{
		bool flag = false;
		size_t beg = line.find(src);
		if(beg==string::npos)
			return false;
		string ori = src;
		src = "";
		int size=0;
		while((beg<line.size() )&& tb[beg] && (size +tb[beg] <=ori.size()))
		{
			src +=line.substr(beg,tb[beg]) + " ";
			beg +=tb[beg];
			size +=tb[beg];
		}

		if(size !=ori.size() )
			return false;
		src = src.erase(src.size() - 1);
		return true;
	}*/
	/*bool GetSrc(const string &line,string &src,const vector<int> &tb )
	{
		bool flag = false;
		size_t beg = line.find(src);
		if(beg==string::npos)
			return false;
		string ori = src;
		src = "";
		int size=0;
		while((beg<line.size() )&& tb[beg] && (size +tb[beg] <=ori.size()))
		{
			src +=line.substr(beg,tb[beg]) + " ";
			size +=tb[beg];
			beg +=tb[beg];

		}

		if(size !=ori.size() )
			return false;
		src = src.erase(src.size() - 1);
		return true;
	}*/

string MInt2Str(int s)
{
	stringstream ss;
	ss<<s;
	string rst;
	ss>>rst;
	return rst;
}
string SetBlank(string &line,vector<pair<int,int> > &tb)
{
	stringstream ss;
	ss<<line;
	string part;
	int beg = 0;
	string rst;
	line = "";
	int i =0;
	while (ss>>part)
	{
		/*tb[beg] = line.size();
		line +=part + " ";
		rst +=part;
		
		int end =beg + part.size();
		tb[end -1] = tb[beg] + part.size() - 1;
		beg = end;*/
		line +=part+" " ;
		rst +=part;
		tb[beg].first = part.size();
		tb[beg].second = i++;
		beg +=part.size();
	}

	line = line.erase(line.size() - 1);
	return rst;
}

bool GetSrc(const string &line,string &src,const vector<pair<int,int> > &tb )
{
	bool flag = false;
	size_t beg = line.find(src);
	if(beg==string::npos)
		return false;
	string ori = src;
	src = "";
	int size=0;
	int endInx = 0;
	int begInx = beg;
	while((beg<line.size() )&& tb[beg].first && (size +tb[beg].first<=ori.size()))
	{
		src +=line.substr(beg,tb[beg].first) + " ";
		size +=tb[beg].first;
		endInx = tb[beg].second;
		beg +=tb[beg].first;
	}
	
	if(size !=ori.size() )
		return false;
	/*int b = tb[beg];
	int e = tb[beg+src.size()-1];
	if(b ==0 || e ==0)
		return false;
	src = lineWithBlk.substr(b,e - b+1);*/
	src = src.erase(src.size() - 1);
	begInx = tb[begInx].second;
	src =MInt2Str(begInx+1)+":"+MInt2Str(endInx+1)+" ||| " +src;
	return true;
}
	void GetTransPair(string &line,vector<string> &dict_rule)//line是分过词的，但是可能词与词之间多于一个空格，在此函数处理之后，词之间只有一个空格
	{
		vector<pair<int,int> > tb(line.size(),make_pair(0,0));
		string sentForNum = SetBlank(line,tb);
		dict_rule.clear();
		string tgt;
		vector<transPair> tps;//基本数词
		vector<transPair> ctps;//复杂数词，可能包含一部分的基本数词
		string pseudoSen;
		string medStr;
		
		m_tiNum.process(sentForNum,tgt,tps,ctps,pseudoSen,medStr);
		for(int i=0;i<ctps.size();++i)
		{
			if (GetSrc(sentForNum,ctps[i].ti,tb))
			{
				string rule = ctps[i].ti +" ||| "+ctps[i].ch+" ||| "+"1.0 1.0 1.0 1.0";
				dict_rule.push_back(rule);
			}
		}

	}
private:
	TiNum m_tiNum;
};

}