// KMA_DLL.cpp: implementation of the KMA_DLL class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx_engine.h"
#include "KMA_DLL.h"
#include "KMAnalyser.h"
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
KMAnalyser g_KMAnalyser;


KMA_DLL::KMA_DLL()
{

}

KMA_DLL::~KMA_DLL()
{

}

char* KMA_DLL::proc_sen(const char *sen)
{
	g_KMAnalyser.proc_sen(sen);
	return g_KMAnalyser.m_res_buff;
}

bool KMA_DLL::create(const char* ini_file)
{
	return g_KMAnalyser.create(ini_file);
}
