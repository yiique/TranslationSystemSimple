#include "stdafx.h"
#include "iotool.h"


iotool::iotool(void)
{
	p_data = NULL;
    LineNo = 0;
    fin.clear();
}


iotool::~iotool(void)
{
	fin.close();
    free(p_data);
}

bool iotool::file_open(string filename)
{
    fin.open(filename.c_str());
    if(!fin.is_open()) return false;
	return true;
}

bool iotool::file_close()
{
    fin.close();
	return true;
}

void iotool::clean_comment(string& line)
{
    size_t f = line.find(";");
    if(f==string::npos) return ;
    if(f==0) line.clear();
    else line = line.substr(0,f);
}

void iotool::clean_empty(string& line)
{
    if(line.length()==0) return;
    int i = 0;
    while(line[i]==' '||line[i]=='\t') i++;
    line = line.substr(i,line.length()-i);
}

bool iotool::read_tree()
{
    p_data = new HEAD_NODE();
    int level = 0,count = 0;
    string line;
    vector <string> data;
    int cnt;
    while(getline(fin,line))
    {
        LineNo++;
        //不存在有效信息
        clean_comment(line);
        clean_empty(line);
        if(line.length()==0) continue;
        //该行还没有开始读取树结构
        cnt = 0;
        while(line[cnt]=='(') cnt++;
        if(cnt==0&&level==0) continue;
        count += cnt;
        if(cnt!=0) line = line.substr(cnt,line.length()-cnt);
        //开始获取树内信息
        if(level==0)
        {
            getLineInfo(line,p_data->data);
            level = 1;
        }
        else
        {
            getLineInfo(line,data);
            if(data.size()!=0) p_data->son.push_back(data);
        }
        size_t f = line.find(")");
        for(int i=0;i<line.length();i++)
            if(line[i]==')') count--;
        if(count==0) return true;
    }
    return false;
}

void iotool::getVector(const string& line,vector <string> &data)
{
    string tmp;
    data.clear();
    istringstream stream(line);
    while(stream>>tmp)  data.push_back(tmp);
}

void iotool::getLineInfo(string &line, vector<string>& data)
{
    size_t  f = line.find(")");
    string line1;
    if(f!=string::npos)
    {
        line1 = line.substr(0,f);
        line = line.substr(f,line.length()-f);
    }
    else
    {
        line1 = line;
        line[0] = '\0';
    }
    getVector(line1,data);
}

void iotool::Debug(HEAD_NODE* now)
{
    cout<<"root: ";
    for(int i=0;i<now->data.size();i++) cout<<"<"<<now->data[i]<<">";
    cout<<endl;
    for(int i=0;i<now->son.size();i++)
    {
        cout<<"son "<<i<<"th:";
        for(int j=0;j<now->son[i].data.size();j++)
        {
            cout<<"<"<<now->son[i].data[j]<<">";
        }
        cout<<endl;
    }
}

HEAD_NODE* iotool::get_tree_head()
{
    return p_data;
}
