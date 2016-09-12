#pragma once

#include "configure.h"
#include "iotool.h"

class Class
{
public:
	Class(void);
	~Class(void);
	//形态素词类分类(含两类)
    struct CLASS{
        string id;      //类名      
        int cost;       //词类花费
        bool kt;         //是否存在活用
    };
    void read_grammar_file(const string& filename);
    void read_cost(const vector <DATA_NODE>& pinci);
    int get_hinsi_id(const string& name);
    int get_bunrui_id(const string& name,const int& hinsi);
    void Debug();
    bool kt(const int& hinsi,const int& bunrui);
    int getcost(const int& hinsi,const int& bunrui);
    CLASS Classify[CLASSIFY_MAX+1][CLASSIFY_MAX+1];
private:
	void init_class();
    void read_class(const string& filename);
};

