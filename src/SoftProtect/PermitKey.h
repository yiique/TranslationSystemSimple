#ifndef PERMIT_KEY_H
#define PERMIT_KEY_H

#include <string>

using namespace std;

class PermitKey
{
public:

	static void Generate(const string & key, string & pin);
	static bool Check(const string & key, const string & pin);

private:

	static string ms_master_key;

};

#endif //PERMIT_KEY_H

