#pragma once
#include "configure.h"
#include "TrieTree.h"
class Dictionary
{
public:
	Dictionary(void);
	~Dictionary(void);
	void add_dictionary(const string& root);
    void init();
    void changedic(const int& num);
    int getDicNum(void);
    void pat_search(const string& want,vector <string> &info);     //返回搜索到的信息
    int numeral_decode(string &str);
    int numeral_decode2(string &str);
    void hiragana_decode(string &info,string &yomi);
    string numeral_encode(const int& now);
    string numeral_encode2(const int& now);
    string hiragana_encode(const string& str);
private:
	vector <TrieTree> DIC;
    int dicnum,dicnow;
};

