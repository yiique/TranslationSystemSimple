#ifndef NENUM2CHN_H
#define NENUM2CHN_H
#include "NEInterface.h"
#include "../../deelx.h"
//#include <sstream>
#include <cstddef>
#undef max
#undef min
#include <sstream>
using namespace std;
namespace UYNE
{

// 将阿拉伯数字变为汉语数字
class NENum2Chn : public NEInterface
{
public:
	NENum2Chn(){}
	NEResult GetRecResult(const string& sent, const NEResult& regdata)
	{
		NEResult recres;
		return recres;
	}
	string GetTransResult(const NEResult& res, const NEResult& regdata,const bool& b_Zero = false)
	{
		NEResult Res = res;
		int num =  atoi(Res.str.c_str());
		string strChNum = GetChStrNum(num);
		return strChNum;		
	}
private:
	string GetChStrNum(const unsigned long long& num, const bool& bZero = false)
	{
		if (num == 0)
			return "";
		if (num > 1000000000)
		{
			stringstream ss;
			ss << num;
			//string num_out;
			//iss >> num_out;
			return ss.str();
		}
		string strChNum;
		for (list<pair<unsigned long long, string> >::reverse_iterator ritr = mChNum.rbegin(); ritr != mChNum.rend(); ritr++)
		{
			unsigned long long quotient = num / ritr->first; // 商
			unsigned long long remaider = num % ritr->first; // 余数
			if (quotient != 0)
			{
				if (quotient == 1 && ritr->first < 10)
				{
					if (bZero == false)
						return ritr->second;
					else
						return mChNum.begin()->second + ritr->second;
				}
				if (bZero == true) // 如果需要加零
					strChNum += mChNum.begin()->second + GetChStrNum(quotient) + ritr->second;
				else
					strChNum += GetChStrNum(quotient) + ritr->second;
				if (remaider == 0)
					return strChNum;
				if (remaider*10 < ritr->first)
					strChNum += GetChStrNum(remaider, true);
				else
					strChNum += GetChStrNum(remaider, false);
				return strChNum;
			}
		}
		return strChNum;
	}
public:
	list<pair<unsigned long long, string> > mChNum;
};
}
#endif
