#include "stdafx.h"
#include "Connect.h"


Connect::Connect(void)
{
}


Connect::~Connect(void)
{
}

void Connect::read_connect_table(const string& filename)
{
	string file = filename+TABLEFILE;
    ifstream fin(file.c_str());
    if(!fin.is_open())
    {
        cerr<<"error in "<<TABLEFILE<<endl;
        exit(-1);
    }
    fin.close();
    read_table(file);
}

void Connect::read_table(const string& filename)
{
    int ret;
    RENSETU_PAIR tmp;
    ifstream fin(filename.c_str());
    fin>>TBL_NUM;
    vector <string> ll;
    string line,f;
    getline(fin,line);
    for(int i=0;i<TBL_NUM;i++)
    {
        getline(fin,line);
        istringstream stream(line);
        ll.clear();
        while(stream>>f) ll.push_back(f);
        if(ll.size()==7)
        {
            tmp.i_pos = atoi(ll[0].c_str());
            tmp.j_pos = atoi(ll[1].c_str());
            tmp.hinsi = atoi(ll[2].c_str());
            tmp.bunrui = atoi(ll[3].c_str());
            tmp.type = atoi(ll[4].c_str());
            tmp.form = atoi(ll[5].c_str());
            if(ll[6]=="*") tmp.goi.clear();
            else tmp.goi = ll[6];
        }
        else
        {
            tmp.i_pos = atoi(ll[0].c_str());
            tmp.j_pos = atoi(ll[1].c_str());
            tmp.hinsi = -1;
            tmp.goi = ll[2];
        }
        rensetu_tbl.push_back(tmp);
    }
    fin.close();
}

void Connect::read_connect_matrix(const string& filename)
{
	string file = filename+MATRIXFILE;
    ifstream fin(file.c_str());
    if(!fin.is_open())
    {
        cerr<<"error in "<<MATRIXFILE<<endl;
        exit(-1);
    }
    fin.close();
    read_matrix(file);
}

void Connect::read_matrix(const string& filename)
{
    int num;
    ifstream fin(filename.c_str());
    fin>>I_NUM>>J_NUM;
    for(int i=0;i<I_NUM;i++)
        for(int j=0;j<J_NUM;j++)
        {
            fin>>num;
            rensetu_mtr.push_back((char)num);
        }
    fin.close();
}

int Connect::check_table_for_undef(const int & hinsi, const int& bunrui)
{
    int i;
    for(i=0;i<TBL_NUM;i++)
    {
        if( rensetu_tbl[i].hinsi == hinsi && rensetu_tbl[i].bunrui == bunrui && rensetu_tbl[i].goi.empty())
            return i;
    }
    return -1;
}

int Connect::check_matrix(const int& postcon, const int& precon)
{
    if(postcon==-1||precon==-1) return DEFAULT_C_WEIGHT;
    return (int)rensetu_mtr[rensetu_tbl[postcon].i_pos*J_NUM+rensetu_tbl[precon].j_pos];
}

bool Connect::check_matrix_left(const int& precon)
 {
        int i;
        if(precon==-1) return false;
        for(i=0;i<I_NUM;i++)
            if((int)rensetu_mtr[i*J_NUM+rensetu_tbl[precon].j_pos]) return true;
        return false;
}

bool Connect::check_matrix_right(const int& postcon)
{
    int j;
    if(postcon==-1) return false;
    for(j= 0;j<J_NUM;j++)
        if((int)rensetu_mtr[rensetu_tbl[postcon].i_pos*J_NUM+j]) return true;
    return false;
}
