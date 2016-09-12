#include "stdafx.h"
#include "Class.h"


Class::Class(void)
{
}


Class::~Class(void)
{
}

void Class::read_grammar_file(const string& filename)
{
	string file = filename+GRAMMARFILE;
    ifstream fin(file.c_str());
    if(!fin.is_open())
    {
        cerr<<"Could not open "<<filename<<endl;
    }
    fin.close();
    init_class();
    read_class(file);
}

void Class::init_class()
{
    for(int i=0;i<=CLASSIFY_MAX;i++)
        for(int j=0;j<=CLASSIFY_MAX;j++)
        {
            Classify[i][j].id.clear();
            Classify[i][j].kt = false;
            Classify[i][j].cost = 0;
        }
}

void Class::read_class(const string& filename)
{
    iotool      classio;
    HEAD_NODE *now_class;
    int i=1,j;
    bool katuyou_flag = false;

    classio.file_open(filename);
    while(classio.read_tree())
    {
        j=0;
        now_class = classio.get_tree_head();
        if(!now_class->data.empty())
        {
            Classify[i][j].id = now_class->data[0];
            if(now_class->data.size()==2) 
            {
                Classify[i][j].kt = true;
                katuyou_flag = true;
            }
            else katuyou_flag = false;
            j++;
        }
        else
        {
            cerr<<"error in grammar file read"<<endl;
            exit(-1);
        }
        for(vector <DATA_NODE>::iterator iter = now_class->son.begin();iter!=now_class->son.end();iter++,j++)
        {
            if(j>=CLASSIFY_MAX) 
            {
                cerr<<"error in bunrui"<<endl;
                exit(-1);
            }
            DATA_NODE son_data = *iter;
            Classify[i][j].id = son_data.data[0];
            if(katuyou_flag==true||son_data.data.size()==2)     Classify[i][j].kt = true;
        }
        if(i>=CLASSIFY_MAX)
        {
            cerr<<"error in hinsi"<<endl;
            exit(-1);
        }
        i++;
    }
	//Debug();
}

void Class::read_cost(const vector <DATA_NODE>& pinci)
{
    int hinsi,bunrui,cost;
	int cnt = 0,pinci_size=pinci.size();
    while(cnt!=pinci_size)
    {
        DATA_NODE tmp = pinci[cnt];
        cost = (int)atoi(tmp.data[tmp.data.size()-1].c_str());
        if(tmp.data[0]=="*")
        {
            for(hinsi = 1;!Classify[hinsi][0].id.empty();hinsi++)
                for(bunrui = 0; !Classify[hinsi][bunrui].id.empty();bunrui++)
                    Classify[hinsi][bunrui].cost = cost;
        }
        else
        {
            hinsi = get_hinsi_id(tmp.data[0]);
            if(tmp.data.size()==2||tmp.data[1]=="*")
            {
                for(bunrui = 0;!Classify[hinsi][bunrui].id.empty();bunrui++)
                    Classify[hinsi][bunrui].cost = cost;
            }
            else
            {
                bunrui = get_bunrui_id(tmp.data[1],hinsi);
                Classify[hinsi][bunrui].cost = cost;
            }
        }
		cnt++;
    }
}

int Class::get_hinsi_id(const string& name)
{
    int i;
    if(name.empty()) 
    {
        cerr<<"NULL string for hinsi"<<endl;
        exit(-1);
    }
    if(name == "*") return 0;
    if(name == DEF_RENGO) return (atoi(RENGO_ID));
	for(i=1;name!=Classify[i][0].id;)
	{
		//cerr<<Classify[i][0].id<<endl;
        if(Classify[i++][0].id.empty())
        {
            cerr<<name<<" is undefined"<<endl;
            exit(-1);
        }
	}
	//cerr<<"end find"<<endl;
    return i;
}

int Class::get_bunrui_id(const string& name,const int& hinsi)
{
    int i;
    if(name.empty())
    {
        cerr<<"NULL string for bunrui"<<endl;
        exit(-1);
    }
    if(name=="*") return 0;
    if(Classify[hinsi][1].id.empty())
    {
        cerr<<Classify[hinsi][0].id<<" has no bunrui in"<<endl;
        exit(-1);
    }
    for(i = 1;name!=Classify[hinsi][i].id;)
        if(Classify[hinsi][i++].id.empty())
        {
            cerr<<Classify[hinsi][0].id<<" does not have bunrui "<<name<<endl;
            exit(-1);
        }
    return i;
}

void Class::Debug()
{
	ofstream out("D:\\ff.txt");
    out<<"class debug"<<endl;
    out<<Classify[0][0].id<<endl;
    for(int i=1;!Classify[i][0].id.empty();i++)
    {
        out<<"一级词类: "<<endl;
        out<<Classify[i][0].id<<" ";
        if(Classify[i][0].kt)   out<<"% ";
        else out<<" ";
        out<<Classify[i][0].cost<<endl;
        if(Classify[i][1].id.empty()) continue;
        out<<" 二级词类: "<<endl;
        for(int j=1;!Classify[i][j].id.empty();j++)
        {
            out<<"     ";
            out<<Classify[i][j].id<<" ";
            if(Classify[i][j].kt)   out<<"% ";
            else out<<" ";
            out<<Classify[i][j].cost<<endl;
        }
        out<<endl;
    }
}

bool Class::kt(const int& hinsi, const int& bunrui)
{
    return Classify[hinsi][bunrui].kt;
}

int Class::getcost(const int& hinsi, const int& bunrui)
{
    return Classify[hinsi][bunrui].cost;
}
