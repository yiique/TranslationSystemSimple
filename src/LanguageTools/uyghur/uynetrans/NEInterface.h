#ifndef NEINTERFACE_H
#define NEINTERFACE_H
#include <string>
#include <vector>
#include <map>
#include <list>
#include "../../deelx.h"

using namespace std;
namespace UYNE
{

enum type {MIN2L, L2CH, RexSub, COM};

struct NEResult 
{
	string type; // 结果类型
	int regxid; // 对应正则表达式ID , -1为未定ID
	pair<int, int> span; // 待替换字符串的位置span信息
	string str; // 待处理字符串
	vector<NEResult> vSub; // 需要其他模块翻译的子串
	pair<int, int> sub_span;
	string sub_str;
	bool IsEmpty(void) { return regxid == -1; } // 没有匹配上id为-1
	CRegexpT<char>* regall;
	CRegexpT<char>* regsub;
	unsigned short repnum;
	bool trans_ch;
	string SentOut;
	string WordOut;
};

class NEInterface
{
public:
	virtual NEResult GetRecResult(const string& sent, const NEResult& regdata) = 0;
	virtual string GetTransResult(const NEResult& res, const NEResult& regdata, const bool& rec_log) = 0; // 里面将待处理字符串翻译并返回，且替换NEResults中str

public:
	string m_Name;
	vector<NEResult> vNumRex;
	vector<pair<string,string> > vRexStr;  //(regex, trans)
	vector<CRegexpT<char>* > vMapRegexp;   //(regex)
};
}
#endif
