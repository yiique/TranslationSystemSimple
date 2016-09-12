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

	
	// ��һ����word���������а���ĸ�п�
    void token(const string& word, string& output);
	// ��һ���Ǳ�׼����ά�Ĵ�word���������а���ĸ�п�
    void tokenNotStdLad(const string& word, string& output);
    string pro(const string& word);//����һ�����Ƿ�Ҫת��Сд

	int fun(int m);
	
	// Add By LIUKAI
	//MutexLock m_mutex_lock;

public:
	COldLadConv();
    ~COldLadConv();
    
	
	bool LoadMapTab(const string &file);//���ļ���ȡ��ͳά��������ά��ת����ӳ��� 1
	
	
	bool LoadToStdMap(const string &file);//���ļ���ȡ�Ǳ�׼����ά�����׼����ά��ת����ӳ���

	
	string Old2Lad(const string &seq);// ����ͳά��ת��Ϊ����ά��//�޸� 2

    string Lad2Old(const string &seq);// ����׼����ά��ת��Ϊ��ͳά��

	string NoStdLad2StdLad(const string &seq);//������ʦ�ķǱ�׼����ά��ת��Ϊ��׼����ά��

	set<string> Lad2Old_new(const string &word);

	bool isLattinString(string& letter);//�ж��Ƿ�Ϊ����ά��

	
	

    
	//string COldLadConv::Replace(string&   str,const   string&   old_value,const   string&   new_value);
	




};

#endif 
