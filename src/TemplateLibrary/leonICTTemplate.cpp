#include "leonICTTemplate.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <strstream>
#include <sstream>
#include <boost/regex.hpp>
#include <sys/types.h>

using namespace std;
using namespace leonICTTemplate;
using namespace boost;

ICTtemplate::ICTtemplate()
{
}
ICTtemplate::~ICTtemplate()
{
}
/*bool ICTtemplate::init(const string _filename, string _op)
{
	ifstream ifs(_filename);

	string data;

	while(getline(ifs,data))
	{
		data= data.substr(data.find_first_not_of(" "),data.find_last_not_of(" ")-data.find_first_not_of(" ")+1);

		Pattern temp_p;
		
		temp_p.pri = atoi((data.substr(0,1)).c_str());

		int sep = data.find("|||",2);

		if(_op == "1")
		{
			temp_p.e_pattern = data.substr(2,sep-2);

			temp_p.c_pattern = data.substr(sep+3,data.length()-sep-3);
		}
		else
		{
			temp_p.e_pattern = data.substr(2,sep-2);

			temp_p.c_pattern = data.substr(sep+3,data.length()-sep-3);
		}

		pattern_list.push_back(temp_p);
	}

	return true;
}*/

/*
 * 初始化模板对象
 *
 * _pattern为模板，op为控制参数1-e2c,0-c2e
 *
 * 
*/

bool ICTtemplate::Reset(string& _pattern, string op)
{
	pattern.pri = atoi((_pattern.substr(0,1)).c_str());
	
	int p = _pattern.find("|||",2);
	
	string str2 = _pattern.substr(2,p-2);
	
	pattern.c_pattern = _pattern.substr(p+3,_pattern.length()-p-3);
	
	pattern.e_pattern = str2;
	
	if(op == "0") swap(pattern.c_pattern,pattern.e_pattern);

	return true;
}

/*bool ICTtemplate::remove(const Pattern _pattern)
{
	list<Pattern>::iterator ite = pattern_list.begin();

	bool re = false;

	while(ite!=pattern_list.end())
	{
		if(ite->e_pattern == _pattern.e_pattern && ite->c_pattern == _pattern.c_pattern)
		{
			ite = pattern_list.erase(ite);
			re = true;
		}
		else
		{
			ite++;
		}
	}
	return re;
}*/

/*bool ICTtemplate::add(const Pattern _pattern)
{
	pattern_list.push_back(_pattern);

	return true;
}*/

/*bool ICTtemplate::getToken(const string sourceStr, vector<string>& strToken)
{
	istrstream istr(sourceStr.c_str());

	string seg;

	while(istr>>seg)
	{ 
		strToken.push_back(seg); 
	}
	return true;
}*/

/*
 * 针对汉语计算token序号
 * 
 * _sourceStr 为原token句子，result为需计算的位置信息，myMatchResult为计算后返回的结果
 *
 * 返回值为false表示由于token匹配错误，返回值为true表示无token错误
 *
*/

bool leonICTTemplate::ReCalculateCh(const string _sourceStr, std::vector< std::pair<int, int> > result, myMatchResult & _mr)
{
	istrstream iss(_sourceStr.c_str());

	string seg;

	vector<string>token;

	while(iss >> seg)
	{
		token.push_back(seg);
	}

	int base;
	int cnt;
	int re = true;

	
	for(int j = 0; j < result.size(); j ++)
	{
		base = 0;
		for(int i = 0; i < token.size(); i ++)
		{	
			if(result[j].first == base)
			{
				_mr.start_token.push_back(i);
				break;
			}
			else if(result[j].first < base)
			{
				_mr.end_token.push_back(i);
				re = false;
				break;
			}
			
			base += token[i].length();

		}
	

		base = 0;

		for(int i = 0; i < token.size(); i ++)
		{
			if(result[j].second  > base) base += token[i].length();

			if(result[j].second  == base)
			{
				_mr.end_token.push_back(i);
				break;
			}
			else if(result[j].second  < base)
			{
				_mr.end_token.push_back(i);
				re = false;
				break;
			}
		}
	}

	while(_mr.start_token.size() < _mr.end_token.size()) _mr.start_token.push_back(token.size());
	while(_mr.start_token.size() > _mr.end_token.size()) _mr.end_token.push_back(token.size());
	
	return re;
}

/*
 * 针对英语计算token序号
 * 
 * _sourceStr 为原token句子，result为需计算的位置信息，myMatchResult为计算后返回的结果
 *
 * 
 *
*/

bool leonICTTemplate::ReCalculateEn(const string _sourceStr, std::vector< std::pair<int, int> > result, myMatchResult & _mr)
{
	string seg;

	int cnt;
	
	string pre;

	for(int i = 0; i < result.size(); i ++)
	{
		pre = _sourceStr.substr(0,result[i].first);

		istrstream iss_start(pre.c_str());

		cnt = 0;
		
		while(iss_start>>seg)
		{
			cnt++;
		}

		_mr.start_token.push_back(cnt);

		iss_start.clear();

		pre = _sourceStr.substr(0,result[i].second + 1);

		istrstream iss_end(pre.c_str());

		cnt = 0;

		while(iss_end>>seg)
		{
			cnt++;
		}

		_mr.end_token.push_back(cnt-1);
	}

	return true;
}

/*bool leonICTTemplate::changeC(string& _pattern)
{
	stringstream str;

	string::size_type pos = _pattern.find("#");
	if(pos==string::npos)  return true;

	string from = "##";
	string to = "(.+)";

	while(pos!=string::npos)
	{
		_pattern.replace(pos,from.size()+1,to);
		pos=_pattern.find(from,pos);
	}

	return true;
}*/

/*
 * 将string中的所有子串替换为另一个子串
 *
 * _pattern 为需要处理的字符串，from为需要被替换的子串，to为替换子串
 *
*/

bool leonICTTemplate::changePunction(string& _pattern, string from, string to)
{
	string::size_type pos = _pattern.find(from);

	if(pos==string::npos) return true;

	while(pos!=string::npos)
	{
		_pattern.replace(pos,from.size(),to);
		pos=_pattern.find(from,pos);
	}
	return true;
}
/*
 * 去除String中所有的空格
 *
 * _sourceStr 为需要处理的字符串
 *
*/
bool leonICTTemplate::delspace(string& _sourceStr)
{
	_sourceStr.erase(std::remove(_sourceStr.begin(),_sourceStr.end(),' '),_sourceStr.end());
	return true;
}
/*
 * 格式化匹配结果
 *
 * _sourceStr为token原字符串，_nr为格式化的结果，_mr为未格式化的结果，_op为控制标签1-e2c,0-c2e
 *
*/

void changeIndex(string &outpattern)
{
	boost::regex expression("##([0-9]+?)");

	std::string::const_iterator start, end, pstart;

	start = outpattern.begin();
	end = outpattern.end(); 
	pstart = outpattern.begin();
	
	boost::match_results<std::string::const_iterator> what;

	vector< pair<int , int> > index;

	vector<int> number;

	int l,r;
	while(boost::regex_search(pstart, end, what, expression))
	{
		if(what[0].matched)
		{
			l = what[0].first - start;
			r= what[0].second - start;
			
			istringstream iss(outpattern.substr(what[1].first - start,what[1].second-what[1].first));

			//cout<<outpattern.substr(what[1].first - start,what[1].second-what[1].first)<<endl;
			int num;

			iss>>num;

			number.push_back(num);

			//cout<<l<<" "<<r<<endl;

			index.push_back(make_pair(l,r));

			pstart = what[0].second;
		}
	}

	stringstream ss;

	ss<<outpattern.substr(0,index[0].first);

	for(int i = 0; i < index.size()-1; i ++)
	{
		ss<<"#X"<<number[i]<<"#"<<outpattern.substr(index[i].second, index[i+1].first - index[i].second);
	}

	ss<<"#X"<<number[number.size() - 1]<<"#"<<outpattern.substr(index[index.size() - 1].second);

	outpattern = ss.str();

}


bool leonICTTemplate::FormatOut(NeedResult & _nr, myMatchResult _mr,const string _sourceStr,string _op)
{
	_nr.formatResult.clear();

	_nr.matchgroup.clear();

	std::vector< std::pair<int ,int > > ogroup;

	for(int j = 0; j < _mr.start_token.size(); j ++)
	{
		std::pair<int,int> r;
		std::pair<int,int> myor;

		r.first = _mr.start_token[j];
		r.second = _mr.end_token[j];

		myor.first = _mr.start[j];
		myor.second = _mr.end[j];

		_nr.matchgroup.push_back(r);

		ogroup.push_back(myor);
	}

	_nr.flag = _mr.pattern.flag;
	
	_nr.sourceStr = _sourceStr;

	string sourceStr = _sourceStr;

	changeVariableFlag(sourceStr, ogroup,_nr.matchgroup, _op);
	//1:8 ||| do you know where #X1# is ? ||| do you know where (.+) is #X1# you know where (.+) is \? ||| 200 200 200 200  ||| 5 6 0
	stringstream str;

	string outpattern="";
	if(_op == "1")
		outpattern = _mr.pattern.c_pattern;
	else
		outpattern = _mr.pattern.e_pattern;

	changeIndex(outpattern);
	str<<(_mr.start_token[0]+1)<<":"<<(_mr.end_token[0]+1)<<" ||| "<< sourceStr <<" ||| "<<outpattern<< " ||| 200 200 200 200 ||| ";

	

	for(int i = 1;i < _nr.matchgroup.size(); i ++)
	{
		str<<(_nr.matchgroup[i].first+1)<<" "<<(_nr.matchgroup[i].second+1)<<" 0 ";
	}

	_nr.formatResult = str.str();
	return true;

}

/*
 * 总匹配函数
 *
 * _icttemplate 匹配的模板对象，_sourceStr 需要匹配的字符串，_nr 格式化后的匹配结果，_flag控制标记1-e2c,0-c2e
 *
*/

bool leonICTTemplate::match_all(ICTtemplate _icttemplate, const string _sourceStr, NeedResult& _nr, string _flag)
{
	bool re = false;

	myMatchResult _mr;

	if(match(_icttemplate.pattern,_sourceStr, _mr, _flag) && _mr.pattern.flag == true)
	
	    FormatOut(_nr,_mr,_sourceStr,_flag);
    else
        _nr.flag = false;

	return re;
}

/*
 * 匹配函数
 *
 * _pattern 匹配的模板对象，_sourceStr 需要匹配的字符串，_mr 匹配结果，_flag控制标记1-e2c,0-c2e
 *
*/
bool leonICTTemplate::match(Pattern _pattern, const string _sourceStr, myMatchResult& _mr, string _flag)
{
	//MatchResult result;
	boost::cmatch ma;

	string pattern = _pattern.e_pattern;

	string sourceStr = _sourceStr;

	if(_flag == "0")
	{
		pattern = _pattern.c_pattern;
		delspace(pattern);
		//changeC(pattern);
		//changePunction(pattern,"?","？");
	    //changePunction(sourceStr,"?","？");
		delspace(sourceStr);

	}
	//CRegexpT <char> regexp(pattern.c_str());
	//result = regexp.Match(sourceStr.c_str());

	//ljm 2012-04-26 异常处理
	try
	{
		boost::regex expression(pattern);

		std::string::const_iterator start, end;
	
		start = sourceStr.begin();
		end = sourceStr.end();   

		boost::match_results<std::string::const_iterator> what;

		std::vector< std::pair<int, int> > result;

		if(boost::regex_search(start, end, what, expression))
		{
			std::pair<int, int> r;

			for(int i = 0; i < what.size();i++)
			{
				if(what[i].matched)
				{
					r.first = what[i].first - start;
					r.second = what[i].second - start;
					result.push_back(r);
					_mr.start.push_back(r.first);
					_mr.end.push_back(r.second);
				}
				else break;
			}

			_mr.pattern = _pattern;

			_mr.pattern.flag =true;

			if(_flag == "1")
			{
				if(!ReCalculateEn(_sourceStr, result, _mr))
					_mr.pattern.flag  = false;
		
			}
			else
			{
				if(!ReCalculateCh(_sourceStr, result, _mr))
					_mr.pattern.flag = false;
			}

			return true;
		}
	}catch(bad_expression & e)
	{
		return false;
	}

	return false;
}
/*
 * 匹配变量转换
 *
 * _sourceStr 需要匹配原字符串，_matchgroup 匹配结果，_flag控制标记1-e2c,0-c2e
 *
*/
bool leonICTTemplate::changeVariableFlag(string& _sourceStr, vector<std::pair<int, int> > _matchgroup, vector<std::pair<int ,int> > _matchgrouptoken, string _op)
{
	if(_op == "1")
	{
		_sourceStr = _sourceStr.substr(_matchgroup[0].first,_matchgroup[0].second - _matchgroup[0].first);
		int temp = 0;
		for(int i = 1; i < _matchgroup.size(); i ++)
		{
			stringstream strstr;

			//cout<<_matchgroup[i].first<<" "<<_matchgroup[i].second<<endl;
		
			strstr<<"#X"<<i<<"#";

			_sourceStr.replace(_matchgroup[i].first - temp , _matchgroup[i].second - _matchgroup[i].first ,strstr.str());

			temp += (_matchgroup[i].second - _matchgroup[i].first) - strstr.str().length();

			strstr.clear();
		}
	}
	else if(_op == "0")
	{
		vector <string>token_source;

		istrstream iss(_sourceStr.c_str());

		string seg;

		while(iss >> seg)
		{
			token_source.push_back(seg);
		}

		stringstream ss;

		int match_index = 1;

		int start = _matchgrouptoken[0].first;

		int end = _matchgrouptoken[0].second;

		for(int i = start; i <= end; i ++)
		{
			if(match_index < _matchgrouptoken.size() && i == _matchgrouptoken[match_index].first)
			{
				ss<<"#X"<<match_index<<"# ";

				if(i == _matchgrouptoken[match_index].second) match_index++;
			}
			else if(match_index < _matchgrouptoken.size() && i == _matchgrouptoken[match_index].second)
			{
				match_index++;
			}
			else if(match_index < _matchgrouptoken.size() && i > _matchgrouptoken[match_index].first && i < _matchgrouptoken[match_index].second)
			{
				continue;
			}
			else
			{
				ss<<token_source[i]<<" ";
			}
		}

		_sourceStr = ss.str();

		/*int temp = 0;
		for(int i = 1; i < _matchgroup.size(); i ++)
		{
			stringstream strstr;

			//cout<<_matchgroup[i].first<<" "<<_matchgroup[i].second<<endl;
		
			strstr<<"#X"<<i<<"#";

			_sourceStr.replace(_matchgroup[i].first - temp , _matchgroup[i].second - _matchgroup[i].first ,strstr.str());

			temp += (_matchgroup[i].second - _matchgroup[i].first) - strstr.str().length();

			strstr.clear();
		}*/
	}

	return true;
}
