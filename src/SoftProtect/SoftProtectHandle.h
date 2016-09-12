#ifndef SOFT_PROTECT_HANDLE_H

#include <string>
using namespace std;

namespace softprotect
{

class SoftProtectHandle
{
public:
	static int Reset();
	static int CheckHost();
	static int CheckUseCnt();
	static int AddUseCnt(const long cnt = 1);
	static void SetUpdateBufSize(const long num);
	static long GetCurrUseCnt();
	static long GetRemainUseCnt();

	static int GetRunningRecordFileContent(string & content);
	static int GetLicenseFileContent(const string & path, string & content);
	static int GetRegisterFileContent(const string & path, string & content);
};

}

#endif //SOFT_PROTECT_HANDLE_H