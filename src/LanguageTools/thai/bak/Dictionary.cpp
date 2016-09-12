#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "Dictionary.h"
using namespace thaiSeg;

CDictionary::CDictionary()
{
}

CDictionary::~CDictionary()
{
}

 /* ��ʼ�� */
bool CDictionary::Initialize(const string &sFileName)
{
	if (!Load(sFileName))
	{
		cout << "Load Dictionary fail" << endl;
		return false;
	}
	return true;
}

 /* ����ʵ� */
bool CDictionary::Load(const string &sFileName)
{
	ifstream fin(sFileName.c_str());
	if (!fin)
	{
		cout << "Can not open dictionary file: " << sFileName << endl;
		return false;
	}

	string line;
	int i=1;

	while (getline(fin, line))
	{
		m_mCore.insert(line);
	}

	fin.close();
	return true;
}

 /* �ж��Ƿ�Ϊ�ʵ��д��� */
bool CDictionary::IsDictElem(const string &ch)
{
	set<string>::iterator it = m_mCore.find(ch);

	return it!=m_mCore.end();
}
