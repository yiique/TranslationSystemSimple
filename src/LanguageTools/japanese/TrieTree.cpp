#include "stdafx.h"
#include "TrieTree.h"


TrieTree::TrieTree(void)
{
	TrieHead = TrieNode();
}


TrieTree::~TrieTree(void)
{
}

inline int TrieTree::word_length(const char p)
{
    unsigned char c = p;
    if (c > 0xfb) return 6;
    else if (c > 0xf7) return 5;
    else if (c > 0xef) return 4;
    else if (c > 0xdf) return 3;
    else if (c > 0x7f) return 2;
    else return 1;
}

void TrieTree::insert_file(const string& filename,const string& token)
{
    string line;
    ifstream fin(filename.c_str());
	int cnt=0;
    while(getline(fin,line))
    {
//		++cnt;
//		if(cnt%10000==0) cerr<<cnt<<endl;
        size_t f = line.find(token);
        insert(line.substr(0,f),line.substr(f+token.size()));
    }
}

void TrieTree::insert(const string& str,const string& info)
{
    insert_inner(TrieHead,0,str,info);
}

void TrieTree::insert_inner(TrieNode& NowNode,const int& str_pos,const string& str,const string& info)
{
    int now_len = word_length(str[str_pos]);
    string now = str.substr(str_pos,now_len);
    if(NowNode.NodeMap.find(now)==NowNode.NodeMap.end())
    {
        NowNode.NodeMap[now] = NowNode.node.size();
        NowNode.node.push_back(TrieNode());
        vector <string> tmp;
        tmp.clear();
        NowNode.info.push_back(tmp);
    }
    int index = NowNode.NodeMap[now];
    if(str_pos+now_len==str.size())
    {
        NowNode.info[index].push_back(info);
        return ;
    }
    insert_inner(NowNode.node[index],str_pos+now_len,str,info);
}

vector <string> TrieTree::query(const string& str)
{
    vector <string> ans;
    ans.clear();
    query_inner(TrieHead,0,str,ans);
    return ans;
}

void TrieTree::query_inner(TrieNode& NowNode,const int& str_pos,const string& str,vector <string>& ans)
{
    int now_len = word_length(str[str_pos]);
    string now = str.substr(str_pos,now_len);
    if(NowNode.NodeMap.find(now)==NowNode.NodeMap.end()) return ;
    int index = NowNode.NodeMap[now];
    string sstr = str.substr(0,str_pos+now_len);
    if(NowNode.info[index].size()!=0)
    {
        for(int i=0;i<NowNode.info[index].size();++i)
        {
            ans.push_back(sstr+SPEARTE+NowNode.info[index][i]);
        }
    }
    query_inner(NowNode.node[index],str_pos+now_len,str,ans);
}
