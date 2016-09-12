#pragma once

#include <unordered_map>
#include <utility>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
using namespace std;
using namespace std::tr1;
class TrieTree
{
public:
	TrieTree(void);
	~TrieTree(void);
	vector <string> query(const string& str);
	void insert_file(const string& filename,const string& token);
	void insert(const string& str,const string& info);
private:
	#define SPEARTE "\t"
	/*�ṹ��*/
	//�ֵ����м�ڵ�
	struct TrieNode
	{
		vector <vector<string> > info;
		vector <TrieNode> node;
		unordered_map <string,int> NodeMap;
		TrieNode(){
			info.clear();
			NodeMap.clear();
			node.clear();
		}
	};
	/*���ݽṹ*/
	TrieNode TrieHead; //�ֵ���ͷ�ڵ�
	/*����*/
	inline int word_length(const char p);
	void insert_inner(TrieNode& NowNode,const int& str_pos,const string& str,const string& info);
	void query_inner(TrieNode& NowNode,const int& str_pos,const string& str,vector <string>& ans);
};

