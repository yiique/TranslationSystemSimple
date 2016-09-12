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
    vector <string> data;       //���ݴ洢
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
    void clean_comment(string &line);   //����ע��
    void clean_empty(string &line);     //��ȡ��ͷ�ո�
    void getLineInfo(string &line,vector <string> &data);
    void getVector(const string& line,vector <string> &data);
    int LineNo;
};

