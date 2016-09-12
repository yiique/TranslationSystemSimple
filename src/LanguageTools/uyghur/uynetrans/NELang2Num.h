#ifndef NELANG2NUM_H
#define NELANG2NUM_H
#include "NEInterface.h"
#include "../../deelx.h"
#include "../../Tools2.h"
using namespace std;
namespace UYNE
{

// 将其他语言翻译成阿拉伯数字
class NELang2Num : public NEInterface
{
public:
	NELang2Num(){}
	string GetTransResult(const NEResult& res, const NEResult& regdata, const bool& rec_log = true)
	{
		string strTemp;
		string strUyNum = res.str;
		unsigned long long longNum = GetUyLongNum(strUyNum, regdata);
		stringstream iss;
		iss << longNum;
		string MinNum;
		iss >> MinNum;
		strTemp = MinNum;
		return strTemp;
	}
	NEResult GetRecResult(const string& sent, const NEResult& regdata)
	{
		NEResult res;
		string strTemp = sent;
		while(true)
		{
			MatchResult result = regdata.regall->Match(sent.c_str());
			if (result.IsMatched())
			{
				res.regxid = 1;
				int intStart = result.GetStart();
				int intEnd = result.GetEnd();
				res.span = make_pair(intStart, intEnd);
				res.str = strTemp.substr(intStart, intEnd-intStart);

				unsigned int rep_start = result.GetGroupStart(regdata.repnum);
				unsigned int rep_end = result.GetGroupEnd(regdata.repnum);
				res.sub_span = make_pair(rep_start, rep_end);
				res.sub_str = strTemp.substr(rep_start, rep_end - rep_start);
				break;
			}		
			else
				break;
		}
		if (res.regxid != 1)
		{				
			res.regxid = -1;
			return res;
		}
		return res;
	}
	unsigned long long GetUyLongNum(const string& str, const NEResult& regdata)
	{
		unsigned long long num = 0;
		int findStart = -1;
		MatchResult result = regdata.regsub->Match(str.c_str(), findStart);
		while (result.IsMatched()/* == true*/)
		{
			int intStart = result.GetStart();
			int intEnd = result.GetEnd();
			string strUyNumSub = str.substr(intStart, intEnd-intStart);
			num += GetUySubNum(strUyNumSub);
			result = regdata.regsub->Match(str.c_str(), intEnd);
		}
		return num;
	}
	unsigned long long GetUySubNum(const string& str)
	{
		float num = 0;
		stringstream sstream(str);
		sstream >> num;
		for (map<string, unsigned long long>::iterator itr = mMinNum.begin(); itr != mMinNum.end(); itr++)
		{
			string::size_type pos = str.find(itr->first);
			if (pos != string::npos)
				return num * itr->second;
		}
		return num;
	}
	
public:
	map<string, unsigned long long> mMinNum;

};
}
#endif
