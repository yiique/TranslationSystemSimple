#pragma once
#include "configure.h"
#include "iotool.h"
class Katuyou
{
public:
	Katuyou(void);
	~Katuyou(void);
	//记录活用形
    void read_katuyou_file(const string& filename);
    void Debug();
    int get_form_id(const string& x,const int& type);
    string get_type(const int& hinsi);
    string get_katuyou_name(const int& hinsi,const int& bunrui);
    string get_katuyou_gobi(const int& hinsi,const int& bunrui);
    string get_katuyou_gobi_yomi(const int& hinsi,const int& bunrui);
    int get_form_size(const int& hinsi);
    int get_type_id(const string& type);
private:
	typedef struct FORM{
        string name;      //活用形名
        string gobi;    //词尾
        string gobi_yomi;       //片假名
        FORM()  {}
        FORM(string _name,string _gobi)
        {
            name = _name;
            gobi = _gobi;
        }
    };
    //记录活用型
    typedef struct TYPE{
        string name;
        TYPE()
        {
            name.clear();
        }
        TYPE(string _name)
        {
            name = _name;
        }
    };
    vector <TYPE> Type;
    vector <vector<FORM> > Form;
    void init_katuyou_form();
    void read_katuyou_form(const string& filename);
};

