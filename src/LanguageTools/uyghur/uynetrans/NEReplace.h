#ifndef NEREPLACE_H
#define NEREPLACE_H
#include "NEInterface.h"
#include "../../deelx.h"
using namespace std;
namespace UYNE
{

// “ª∞„NEÃÊªª¿‡
class NEReplace : public NEInterface
{
public:
	NEReplace(){}
	NEResult GetRecResult(const string& sent, const NEResult& regdata)
	{
		NEResult res;
		string temp = sent;
		for (unsigned int i = 0; i< vMapRegexp.size(); i++)
		{
			MatchResult result = vMapRegexp[i]->Match(temp.c_str());
			if (result.IsMatched() /*== true*/)
			{
				res.regxid = i ;
				res.span = make_pair(result.GetStart(),result.GetEnd());
				res.str = temp.substr(result.GetStart(), result.GetEnd()-result.GetStart());	

				string tostring = vRexStr[i].second;
				temp = vMapRegexp[i]->Replace(temp.c_str(),tostring.c_str());
				res.SentOut = temp;

				string wordtemp = res.str;
				wordtemp = vMapRegexp[i]->Replace(wordtemp.c_str(),tostring.c_str());
				res.WordOut = wordtemp;	
			}
		}
		if(res.SentOut != "")
			return res;
		else
		{
			res.regxid = -1;
			return res;
		/*	res.SentOut = sent;*/
		}
	}
	string GetTransResult(const NEResult& res, const NEResult& regdata,const bool& rec_log /*= true*/)
	{
		string temp;
		return temp;
		
	}
	bool PreProc(string &strTemp)
	{
		static const CRegexpT<char> RexSpaceAdd("[\\s]+\\+");
		if (RexSpaceAdd.Match(strTemp.c_str()).IsMatched() /*== true*/)
		{
			strTemp = RexSpaceAdd.Replace(strTemp.c_str(), "+");
			return true;
		}
		return false;
	}
	void PostProc(string& strTemp, const bool& replace_flag)
	{
		if (replace_flag == true)
			strTemp = Tools::StrTool::replace_all(strTemp, "+", " +");
		const char di[] = {0xE7, 0xAC, 0xAC, 0};
		const char yi[] = {0xE4, 0xB8, 0x80, 0};
		const char shi[] = {0xE5, 0x8D, 0x81, 0};
		string org = string(di)+string(yi)+string(shi);
		string tgt = string(di)+string(shi);
		strTemp = Tools::StrTool::replace_all(strTemp, org, tgt);
		string org2 = " "+string(yi)+string(shi);
		string tgt2 = " "+string(shi);
		strTemp = Tools::StrTool::replace_all(strTemp, org2, tgt2);
	}

public:
	
	map<string, string> mPlaceDict, mNameDict;
};
}
#endif
