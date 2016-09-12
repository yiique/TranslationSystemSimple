#pragma once
#include "configure.h"
class Connect
{
public:
	Connect(void);
	~Connect(void);
	void read_connect_table(const string& filename);
    void read_connect_matrix(const string& filename);
    int check_table_for_undef(const int& hinsi,const int& bunrui);
    int check_matrix(const int& postcon,const int& precon);
    bool check_matrix_left(const int& precon);
    bool check_matrix_right(const int& postcon);
private:
	struct RENSETU_PAIR{
        int i_pos;
        int j_pos;
        int hinsi;      //一级词性
        int bunrui;     //二级词性
        int type;
        int form;
        string goi;     //平假名
    };
    int TBL_NUM;
    int I_NUM;
    int J_NUM;
    vector <RENSETU_PAIR> rensetu_tbl;
    string rensetu_mtr;
    void read_table(const string& filename);
    void read_matrix(const string& filename);
};

