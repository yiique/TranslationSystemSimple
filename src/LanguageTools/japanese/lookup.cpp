// lookup.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "token.h"
#include <iostream>
using namespace std;
#include <fstream>
using namespace JPTOKEN;
int _tmain(int argc, _TCHAR* argv[])
{
	int tag=1;
    JPTOKEN::token jptoken;
    vector <string> token_ans;
    string line;
	int cnt = 0;
	jptoken.init("D:\\CodeBak\\lookup\\lookup\\setting.ini");
	ifstream fin("D:\\input.txt");
	ofstream out("D:\\output.txt");
    while(getline(fin,line))
    {
		cerr<<++cnt<<endl;
        jptoken.segment(line,tag,token_ans);
        if(tag == 0)
        {
            for(int i=0;i<token_ans.size();i++)
            {
                if(i!=0) cout<<" ";
                out<<token_ans[i];
            }
            out<<endl;
        }
        else
        {
            for(int i=0;i<token_ans.size();i++)
                out<<token_ans[i]<<endl;
            out<<"EOS"<<endl;
        }
    }
	return 0;
}

