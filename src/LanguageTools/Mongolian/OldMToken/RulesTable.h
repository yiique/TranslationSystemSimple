//#ifndef NAMEDENTITYTRANSLATION_H_INCLUDED
//#define NAMEDENTITYTRANSLATION_H_INCLUDED

//RulesTable.h
//author:chuanjie
//class:RulesTable

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "deelx.h"

using namespace std;

class RulesTable
{
public:
    RulesTable(char* CfgFileName)
    {
        ifstream inCfgFileStream(CfgFileName);//"New.Mo.Cfg"
        string ruleLine;
        unsigned int pos;

        while (getline(inCfgFileStream, ruleLine))
        {
            if((unsigned char)ruleLine[0] != '#')
            {
                pos = ruleLine.find("\t", 0);
                string pattern = ruleLine.substr(0, pos);
                patterns.push_back(pattern);
                string substitute = ruleLine.substr(pos+1);
                substitutes.push_back(substitute);
            }

        }
        inCfgFileStream.close();
    }
public:
    string Translate(string line)
    {
        static CRegexpT <char> regexp;
        unsigned int i;
        for(i = 0; i < patterns.size(); i++)
        {
            //导入一个模式
            regexp.Compile(patterns.at(i).c_str());

            //用上面导入的模式对当前字符串进行匹配
            if(regexp.Match(line.c_str()).IsMatched() == true) //如果成功
                line = regexp.Replace(line.c_str(), substitutes.at(i).c_str()); //用替代品进行替换

        }

        return line;
    }

private:
    std::vector<string> patterns;
    std::vector<string> substitutes;
};



//#endif // NAMEDENTITYTRANSLATION_H_INCLUDED
