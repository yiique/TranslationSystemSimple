#ifndef STRINGHASH_H
#define STRINGHASH_H

#include <string>
#include <vector>
#include <map>
#include <cstring>
using namespace std;

struct ptrCmp
{
	bool operator()(const char* src1, const char* src2) const
	{
		return strcmp(src1,src2) < 0;
	}
};

#endif
