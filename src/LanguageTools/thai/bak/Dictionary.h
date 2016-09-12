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
	bool Load(const string &sFileName);  /* 载入词典 */
	bool IsDictElem(const string &ch);   /* 判断是否为词典中词条 */

private:
	set<string> m_mCore; /* 存储词典中词条 */
	
};

}

#endif

