#ifndef DICTIONARY
#define DICTIONARY

#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <map>

using namespace std;
namespace thaiSeg {

class CDictionary
{
public:
	CDictionary();
	~CDictionary();
	bool Initialize(const string &sFileName);
	bool Load(const string &sFileName);  /* ����ʵ� */
	bool IsDictElem(const string &ch);   /* �ж��Ƿ�Ϊ�ʵ��д��� */

private:
	set<string> m_mCore; /* �洢�ʵ��д��� */
	
};

}

#endif

