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
	string type; // �������
	int regxid; // ��Ӧ������ʽID , -1Ϊδ��ID
	pair<int, int> span; // ���滻�ַ�����λ��span��Ϣ
	string str; // �������ַ���
	vector<NEResult> vSub; // ��Ҫ����ģ�鷭����Ӵ�
	pair<int, int> sub_span;
	string sub_str;
	bool IsEmpty(void) { return regxid == -1; } // û��ƥ����idΪ-1
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
	virtual string GetTransResult(const NEResult& res, const NEResult& regdata, const bool& rec_log) = 0; // ���潫�������ַ������벢���أ����滻NEResults��str

public:
	string m_Name;
	vector<NEResult> vNumRex;
	vector<pair<string,string> > vRexStr;  //(regex, trans)
	vector<CRegexpT<char>* > vMapRegexp;   //(regex)
};
}
#endif
