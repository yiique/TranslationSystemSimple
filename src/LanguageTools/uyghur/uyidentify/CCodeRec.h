#include <iostream>
#include <fstream>
#include <string>
#include "LangModel.h"
// Add By LIUKAI
//#include "../Base//BaseMutex.h"

using namespace std;

class CCodeRec
{
public:
      	CLangModel chLang;
		CLangModel ucLang;
		CLangModel latinLang;
		int ngram;
		
public:
	    CCodeRec();
		bool Load(const string &path);//
		string RecCodeStr(const string &inputstr);//
		string RecCodeFile(const string &filepath);//
		string RecCodeStr2(const string &inputstr);
		string RecCodeFile2(const string &filepath);
private:
		string findMax(double chscore, double ucscore, double latinscore);  //FCL CHANGE TO PRIVATE

		// Add By LIUKAI
		//MutexLock m_mutex_lock;
};
	
