#include <fstream>
#include <iostream>
#include <cstdlib>
#include "Dictionary.h"

CDictionary::CDictionary()
{

}

CDictionary::~CDictionary()
{

}

bool CDictionary::InitializeUser()
{
    string str="";
    char *ch = (char *)malloc((int(str.size())+1)*sizeof(char));
    strcpy(ch,str.c_str());
    m_mCore.insert(map<char*, int, ptrCmp>::value_type(ch,0));
    return true;
}

bool CDictionary::Initialize(const string &sFileName)
{
	if (!Load(sFileName))
	{
		cout << "Load Dictionary fail: " << sFileName << endl;
		return false;
	}
	return true;
}

bool CDictionary::Load(const string &sFileName)
{
	ifstream fin(sFileName.c_str());
	if (!fin)
	{
		cout << "Can not open dictionary file: " << sFileName << endl;
		return false;
	}

	string line;
	int i=0;
	while (getline(fin, line))
	{
		char *ch = (char *)malloc((int(line.size())+1)*sizeof(char));
		strcpy(ch,line.c_str());
		m_mCore.insert(map<char*, int, ptrCmp>::value_type(ch,i));
	}
	fin.close();
	return true;
}

bool CDictionary::InSet(const string &ch)
{
	char *c =(char *)malloc((int(ch.size())+1)*sizeof(char));
	strcpy(c,ch.c_str());
	map<char*, int, ptrCmp>::const_iterator it = m_mCore.find(c);
	free(c);
	return it != m_mCore.end();
}

bool CDictionary::DInsertEntry(const string &sword)
{
    char *c =(char *)malloc((int(sword.size())+1)*sizeof(char));
    strcpy(c,sword.c_str());
    m_mCore.insert(map<char*, int, ptrCmp>::value_type(c,1));
    return true;
}

bool CDictionary::DDeleteEntry(const string &sword)
{
    char *c =(char *)malloc((int(sword.size())+1)*sizeof(char));
    strcpy(c,sword.c_str());
    
    map<char*, int, ptrCmp>::iterator it = m_mCore.find(c);
    
    if(it != m_mCore.end())
    	m_mCore.erase(it);
    
    return true;
}

long CDictionary::GetDictSize()
{
    return (long)m_mCore.size();
}

