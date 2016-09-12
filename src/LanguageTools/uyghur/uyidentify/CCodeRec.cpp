#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "LangModel.h"
#include "CCodeRec.h"
/**
    *initialize CCodeRec and set ngram
	*/
CCodeRec::CCodeRec()
{
	ngram=2;
}
   
/**
    *compare three scores and return corresponding language type
	*@param chscore
	*	 the chscore
    *@param uscore
	*     the ucscore
    *@param latinscore
	*      the latinscore
    *@return the language type
    */
string CCodeRec::findMax(double chscore, double ucscore,double latinscore)
{
	
	if(chscore>=ucscore)
	{
		if(chscore>=latinscore) return("ch");
		else
		{
			if(latinscore>-1.0) return("latin");
			else return("unknown");
		}
	}
	else 
	{
		if(ucscore>=latinscore) return("uc");
		else
		{
			if(latinscore>-1.0) return("latin");
			else return("unknown");
		}
	}
}

/**
   *load language models to initialize chLang,ucLang and latinLang
   *@param path
   *     load language models ch.gz,uc.gz,latin.gz from this path
   *@return whether Loading has been done
   */
bool CCodeRec::Load(const string &path)
{
	
	chLang.Initialize(path+"/ch",ngram);
	ucLang.Initialize(path+"/uc",ngram);
	latinLang.Initialize(path+"/latin",ngram);
	return(true);
}

/**
   *return the language type of input string
   *@inputstr
   *     the input string
   *@return language type
   */
string CCodeRec::RecCodeStr(const string &inputstr)
{
	double chscore=chLang.SeqLMProb(inputstr)/(inputstr.length()*2.0);
	double ucscore=ucLang.SeqLMProb(inputstr)/(inputstr.length()*2.0);
	double latinscore=latinLang.SeqLMProb(inputstr)/(inputstr.length()*2.0);
	return(findMax(chscore,ucscore,latinscore));
}

/**
   *return the language type of file
   *@file path
   *    the file path
   *@return language type
   */
string CCodeRec::RecCodeFile(const string &filepath)
{
	string tmpstr;
	double chSum=0.0;
    double ucSum=0.0;
	double latinSum=0.0;
	ifstream infile(filepath.c_str());
	
	if(!infile)
	{
		cout<<"Open file failure!"<<endl;
	}
	while(getline(infile,tmpstr))
	{
		ucSum+=ucLang.SeqLMProb(tmpstr);
		chSum+=chLang.SeqLMProb(tmpstr);
		latinSum+=latinLang.SeqLMProb(tmpstr);
	}
	chSum/=(tmpstr.length()*2.0);
	ucSum/=(tmpstr.length()*2.0);
	latinSum/=(tmpstr.length()*2.0);
	return(findMax(chSum,ucSum,latinSum));
}

/**
   *return the language type of input string(only for latin and uc)
   *@inputstr
   *     the input string
   *@return language type
   */
string CCodeRec::RecCodeStr2(const string &inputstr)
{
	//cout << "RecCodeStr2:input = " << inputstr << endl;
	double ucscore=ucLang.SeqLMProb(inputstr)/(inputstr.length()*2.0);
	double latinscore=latinLang.SeqLMProb(inputstr)/(inputstr.length()*2.0);

	//cout << "ucscore = " << ucscore << endl;
	//cout << "latinscore = " << latinscore << endl;
	return(ucscore>latinscore? "uc":"latin");
}

/**
   *return the language type of file(only for latin and uc)
   *@file path
   *    the file path
   *@return language type
   */
string CCodeRec::RecCodeFile2(const string &filepath)
{
	string tmpstr;
	double chSum=0.0;
    double ucSum=0.0;
	double latinSum=0.0;
	ifstream infile(filepath.c_str());
	
	if(!infile)
	{
		cout<<"Open file failure!"<<endl;
	}
	while(getline(infile,tmpstr))
	{
		ucSum+=ucLang.SeqLMProb(tmpstr);
		latinSum+=latinLang.SeqLMProb(tmpstr);
	}
	ucSum/=(tmpstr.length()*2.0);
	latinSum/=(tmpstr.length()*2.0);
	return(ucSum>latinSum?"uc":"latin");
}
