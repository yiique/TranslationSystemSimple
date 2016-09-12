#include "stdafx.h"
#include "Dictionary.h"


Dictionary::Dictionary(void)
{
}


Dictionary::~Dictionary(void)
{
}

void Dictionary::init()
{
    dicnow = 0;
    dicnum = 0;
    DIC.clear();
}

void Dictionary::changedic(const int& num)
{
    dicnow = num;
}

void Dictionary::add_dictionary(const string& root)
{
    if(dicnum>=MAX_DIC_NUMBER)
    {
        cerr<<"Too many dictionary files."<<endl;
        exit(-1);
    }
    dicnum++;
    dicnow = dicnum - 1;
    DIC.push_back(TrieTree());
    DIC[dicnow].insert_file(root,"\t");
}

void Dictionary::pat_search(const string& want,vector <string>& info)
{
    info.clear();
    info = DIC[dicnow].query(want);
}

int Dictionary::getDicNum()
{
    return dicnum;
}

string Dictionary::numeral_encode(const int& num)
{
    string l;
    l.clear();
    if (num == atoi(RENGO_ID)) l += ((unsigned char)(0xff));
    else if (num < 0xf0-0x20) l += ((unsigned char)(num+0x20));
    else
    {
        l += ((unsigned char)(num/(0xf0-0x20)+0xf0));
        l += ((unsigned char)(num%(0xf0-0x20)+0x20));
    }
    return l;
}

string Dictionary::numeral_encode2(const int& num)
{
    string l;
    l.clear();
    l += ((unsigned char)((num+1)/(0x100-0x20)+0x20));
    l += ((unsigned char)((num+1)%(0x100-0x20)+0x20));
    return l;
}

string Dictionary::hiragana_encode(const string& str)
{
    string l;
    l.clear();
    if(str.find("@")!=0) l = str;
    l += (unsigned char)0x20;
    return l;
}

int Dictionary::numeral_decode2(string &str)
{
    unsigned char a ,b;
    a = str[0];
    b = str[1];
    str = str.substr(2);
    return ((a-0x20)*(0x100-0x20)+b-0x20-1);
}

void Dictionary::hiragana_decode(string& info, string& yomi)
{
    int i=0;
    while(info[i]!=0x20) i++;
    yomi = info.substr(0,i);
    info = info.substr(i+1);
}

int Dictionary::numeral_decode(string& str)
{
    unsigned char num;
    if(str[0]<0xf0)
    {
        num = str[0]-0x20;
        str = str.substr(1);
    }
    else if(str[0]==0xff)
    {
        num  = atoi(RENGO_ID);
        str = str.substr(1);
    }
    else
    {
        num = ((str[0]-0xf0)*(0xf0-0x20)+str[1]-0x20);
        str = str.substr(2);
    }
    return num;
}