#include "stdafx.h"
#include "Katuyou.h"


Katuyou::Katuyou(void)
{
}


Katuyou::~Katuyou(void)
{
}

int Katuyou::get_form_size(const int& hinsi)
{
    return Form[hinsi].size();
}
void Katuyou::read_katuyou_file(const string& filename)
{
	string file = filename+CONJUGATIONFILE;
    ifstream fin(file.c_str());
    if(!fin.is_open()) 
    {
        cerr<<"error in katuyoufile"<<endl;
        exit(-1);
    }
    fin.close();
    init_katuyou_form();
    read_katuyou_form(file);
}

string Katuyou::get_type(const int& hinsi)
{
    return Type[hinsi].name;
}

string Katuyou::get_katuyou_name(const int& hinsi, const int& bunrui)
{
    return Form[hinsi][bunrui].name;
}

string Katuyou::get_katuyou_gobi(const int& hinsi, const int& bunrui)
{
    return Form[hinsi][bunrui].gobi;
}

string Katuyou::get_katuyou_gobi_yomi(const int& hinsi, const int& bunrui)
{
    return Form[hinsi][bunrui].gobi_yomi;
}

int Katuyou::get_type_id(const string& type)
{
    int i;
    if(type.empty()) 
    {
        cerr<<"NULL type  in reading Type"<<endl;
        exit(-1);
    }
    if(type=="*") return 0;
    for(i=1;i<Type.size();i++)
        if(type == Type[i].name) break;
    if(i==Type.size())
    {
        cerr<<"Error Type"<<endl;
        return -1;
    }
    return i;
}

void Katuyou::init_katuyou_form()
{
    Type.clear();
    Form.clear();
}

void Katuyou::read_katuyou_form(const string& filename)
{
    int i,j;
    i = 1;
    HEAD_NODE *head_node;
    iotool katuyouio;
    katuyouio.file_open(filename);
    vector <FORM> tmp;
    tmp.clear();
    Type.push_back(TYPE());
    Form.push_back(tmp);
    while(katuyouio.read_tree())
    {
        head_node = katuyouio.get_tree_head();
        Type.push_back(TYPE(head_node->data[0]));
        tmp.clear();
        tmp.push_back(FORM());
        j = 1;
        for(vector <DATA_NODE>::iterator iter = head_node->son.begin();iter!=head_node->son.end();iter++)
        {
            DATA_NODE data_node = *iter;
            tmp.push_back(FORM(data_node.data[0],data_node.data[1]));
            if(tmp[j].gobi=="*") tmp[j].gobi = "";
            if(data_node.data.size()==3)
            {
                if(data_node.data[2]=="*")      tmp[j].gobi_yomi = "";
                else tmp[j].gobi_yomi = data_node.data[2];
            }
            else
            {
                tmp[j].gobi_yomi = tmp[j].gobi;
            }
            j++;
        }
        Form.push_back(tmp);
        i++;
    }
}

void Katuyou::Debug()
{
    cout<<"katuyou Debug"<<endl;
    for(int i=1;i<Type.size();i++)
    {
        cout<<"活用型: "<<Type[i].name<<endl;
        if(Form[i].size()==1) continue;
        cout<<"活用形: "<<endl;
        for(int j=1;j<Form[i].size();j++)
        {
            cout<<Form[i][j].name<<" "<<Form[i][j].gobi<<" "<<Form[i][j].gobi_yomi<<endl;
        }
        cout<<endl;
    }
}

int Katuyou::get_form_id(const string& x, const int& type)
{
    int i;
    if(x.empty())
    {
        cerr<<"The string is empty"<<endl;
        exit(-1);
    }
    if(x=="*") return 0;
    if(type ==0 )
    {
        cerr<<"Do not have this type"<<endl;
        exit(-1);
    }
    for(i=1;i<Form[type].size();i++)
        if(Form[type][i].name==x) break;
    if(i==Form[type].size())
    {
        cerr<<Type[type].name<<" does not have katuyou"<<endl;
        exit(-1);
    }
    return i;
}
