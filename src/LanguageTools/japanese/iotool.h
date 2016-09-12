#pragma once
#include "configure.h"
struct DATA_NODE{
    vector <string> data;
    DATA_NODE(const vector <string>& _data)
    {
        data.clear();
		for(int i=0;i<_data.size();i++) data.push_back(_data[i]);
    }
};


struct HEAD_NODE{
    vector <string> data;       //数据存储
    vector <DATA_NODE> son;
    HEAD_NODE()
    {
        data.clear();
        son.clear();
    }
};
class iotool
{
public:
	iotool(void);
	~iotool(void);
	bool read_tree();
    bool file_open(string filename);
    bool file_close();
    HEAD_NODE* get_tree_head();
    void Debug(HEAD_NODE* node);
private:
	ifstream fin;
    HEAD_NODE* p_data;
    void clean_comment(string &line);   //消除注释
    void clean_empty(string &line);     //获取行头空格
    void getLineInfo(string &line,vector <string> &data);
    void getVector(const string& line,vector <string> &data);
    int LineNo;
};

