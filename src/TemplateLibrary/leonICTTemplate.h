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

		bool flag; //���Ժ���

		int pri;

	}Pattern; //�������

	typedef struct
	{
		vector<int> start; //�����ַ���ʼλ��
		vector<int> end; //�����ַ�����λ��

		vector<int> start_token; //token��ʼλ��
		vector<int> end_token; //token����λ��

		Pattern pattern; //ƥ�����õ�pattern
	}myMatchResult; //ƥ����

	class ICTtemplate
	{
		public:
			Pattern pattern; //��������ʽ
		public:
			ICTtemplate();
			~ICTtemplate();

			//bool remove(const Pattern _pattern);
			//bool add(const Pattern _pattern);
			//bool init(const string _filename, string _op);

			bool Reset(string& _pattern,string op);

			//bool getToken(const string _source, vector<string>& _strToken);
	};
	//bool changeC(string& _pattern); //������

	bool changePunction(string& _pattern, string from, string to);

	bool delspace(string& _sourceStr);

	bool ReCalculateEn(const string _sourceStr, std::vector< std::pair<int, int> > result, myMatchResult& _mr); //����Ӣ��token���

	bool ReCalculateCh(const string _sourceStr, std::vector< std::pair<int, int> > result, myMatchResult& _mr); //��������token���

	bool match(Pattern _pattern, const string _sourceStr, myMatchResult& _mr, string _flag); //sourceStr��Ҫ�����ľ��ӣ�index�ܹ�ƥ��ı�ţ�flag����ʹ��E����C

	bool match_all(ICTtemplate _icttemplate, const string _sourceStr, NeedResult& _nr, string _flag);

	bool FormatOut(NeedResult & _nr, myMatchResult _mr,const string _sourceStr, string _op);

	bool changeVariableFlag(string& _sourceStr, vector<std::pair<int, int> > _matchgroup, vector<std::pair<int ,int> > _matchgrouptoken, string _op);
}

#endif
