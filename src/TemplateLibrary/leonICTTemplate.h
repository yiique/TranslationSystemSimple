#ifndef LEONICTTEMPLATEE2C_H
#define LEONICTTEMPLATEE2C_H

#include <iostream>
#include <list>
#include <vector>
using namespace std;

namespace leonICTTemplate
{
	typedef struct
	{
		string formatResult;

		string sourceStr;

		bool flag;

		vector<std::pair<int, int> > matchgroup;

	}NeedResult;

	typedef struct
	{
		string e_pattern;
		string c_pattern;

		bool flag; //可以忽视

		int pri;

	}Pattern; //存放正则

	typedef struct
	{
		vector<int> start; //分组字符开始位置
		vector<int> end; //分组字符结束位置

		vector<int> start_token; //token开始位置
		vector<int> end_token; //token结束位置

		Pattern pattern; //匹配所用的pattern
	}myMatchResult; //匹配结果

	class ICTtemplate
	{
		public:
			Pattern pattern; //保存正则式
		public:
			ICTtemplate();
			~ICTtemplate();

			//bool remove(const Pattern _pattern);
			//bool add(const Pattern _pattern);
			//bool init(const string _filename, string _op);

			bool Reset(string& _pattern,string op);

			//bool getToken(const string _source, vector<string>& _strToken);
	};
	//bool changeC(string& _pattern); //测试用

	bool changePunction(string& _pattern, string from, string to);

	bool delspace(string& _sourceStr);

	bool ReCalculateEn(const string _sourceStr, std::vector< std::pair<int, int> > result, myMatchResult& _mr); //处理英文token标记

	bool ReCalculateCh(const string _sourceStr, std::vector< std::pair<int, int> > result, myMatchResult& _mr); //处理中文token标记

	bool match(Pattern _pattern, const string _sourceStr, myMatchResult& _mr, string _flag); //sourceStr需要分析的句子，index能够匹配的标号，flag控制使用E或者C

	bool match_all(ICTtemplate _icttemplate, const string _sourceStr, NeedResult& _nr, string _flag);

	bool FormatOut(NeedResult & _nr, myMatchResult _mr,const string _sourceStr, string _op);

	bool changeVariableFlag(string& _sourceStr, vector<std::pair<int, int> > _matchgroup, vector<std::pair<int ,int> > _matchgrouptoken, string _op);
}

#endif
