#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <map>
#include "Ptrcmp.h"

using namespace std;

class CDictionary
{
public:
	CDictionary();
	~CDictionary();
	bool Initialize(const string &sFileName);
	bool Load(const string &sFileName);
	bool InSet(const string &ch);
    bool DInsertEntry(const string &sword);
	bool DDeleteEntry(const string &sword);
    long GetDictSize();
    bool InitializeUser(); 

protected:

private:
	map<char*, int, ptrCmp> m_mCore;
};

#endif

