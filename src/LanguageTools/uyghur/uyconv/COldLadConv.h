#ifndef COLDLADCONV_H
#define COLDLADCONV_H


#include <string>
#include <fstream>
#include <iostream>
#include<stdio.h>
#include<map>
#include <set>
#include <strstream>
#include<vector>
#include <sstream>

#include "Common/Utf8Stream.h"
//#include "MultiThreadPack.h"



using namespace std;



class COldLadConv
{

private:
	map<string,string> lowercase_map;
	map<string,string> uppercase_map;
	map<string,string> vowelDict;
	map<string,string> lettersDict;
	map<string,string> vowelDictToArabic;
	map<string,string> lettersDictToArabic;
	map<string,string> stdcase_map1;
	map<string,string> stdcase_map2;

	
	// 给一个词word，对它进行按字母切开
    void token(const string& word, string& output);
	// 给一个非标准拉丁维文词word，对它进行按字母切开
    void tokenNotStdLad(const string& word, string& output);
    string pro(const string& word);//处理一个词是否要转成小写

	int fun(int m);
	
	// Add By LIUKAI
	//MutexLock m_mutex_lock;

public:
	COldLadConv();
    ~COldLadConv();
    
	
	bool LoadMapTab(const string &file);//从文件读取传统维文与拉丁维文转换的映射表 1
	
	
	bool LoadToStdMap(const string &file);//从文件读取非标准拉丁维文与标准拉丁维文转换的映射表

	
	string Old2Lad(const string &seq);// 将传统维文转化为拉丁维文//修改 2

    string Lad2Old(const string &seq);// 将标准拉丁维文转化为传统维文

	string NoStdLad2StdLad(const string &seq);//将麦老师的非标准拉丁维文转换为标准拉丁维文

	set<string> Lad2Old_new(const string &word);

	bool isLattinString(string& letter);//判断是否为拉丁维文

	
	

    
	//string COldLadConv::Replace(string&   str,const   string&   old_value,const   string&   new_value);
	




};

#endif 
