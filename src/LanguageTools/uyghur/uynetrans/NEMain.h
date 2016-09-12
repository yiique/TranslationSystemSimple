#ifndef NEMAIN_H
#define NEMAIN_H
#include "NENum2Chn.h"
#include "NELang2Num.h"
#include "NEReplace.h"
#include "NEInterface.h"
#include "../../deelx.h"
#include "../../Tools2.h"
//#include <sstream>
#include <list>
#include <map>
#undef max
#undef min
#include<sstream>

namespace UYNE
{

class NEMain
{
public:
	// 顺序v[i][1]~v[i][n]对句子整一遍再v[1]~v[n]
	// 每一行对按顺序对句子处理一遍，完成后将句子替换一下
	// 接下来依次类推，后面一行的都在前面一行的基础上做工作
	vector<vector<NEInterface*> > m_vvComponent;
	vector<NEInterface*> m_vPre, m_vC, m_vPost, m_vRex, m_vDict;
	// 上面数据的Index查找副本
	map<string, NEInterface*> m_mapComponent;
	NELang2Num* m_Trans2Num;	
	NENum2Chn* m_Num2Chn;
	NEReplace* m_PlaceDict, *m_NameDict, *m_ComRex, *m_RexAllSub;
	NEReplace* m_pre, *m_post;

	NELang2Num t2n;
	NENum2Chn n2c;	
	NEReplace pd, nd, comrex, rexallsub;
	NEReplace pre,post;

	map<string,string> vMaplog;

	bool flag;
public:
	NEMain(){}
	NEMain(const string& NumCfg, const string& NECfg)
	{

		m_Trans2Num = &t2n;

		m_Num2Chn = &n2c;

    	m_ComRex = &comrex;
		m_RexAllSub = &rexallsub;

		m_pre = &pre;
		m_post = &post;

		m_pre->m_Name = "PreProc";
		m_post->m_Name = "PostProc";

		m_Trans2Num->m_Name = "Trans2Num";
		m_Trans2Num->mMinNum.clear();

		m_Num2Chn->m_Name = "Num2Chn";
		m_Num2Chn->mChNum.clear();

		m_RexAllSub->m_Name = "RexAllSub";
		m_RexAllSub->vNumRex.clear();
		m_RexAllSub->vMapRegexp.clear();
		m_RexAllSub->vRexStr.clear();

		if (ReadNumConfig(NumCfg) == false)
		{
			flag = false;
			return;
		}
		else 
			flag = true;
		if(ReadNEConfig(NECfg) == false)
		{
			flag = false;
			return;
		}
		else
			flag = true;

		m_vPre.push_back(m_pre);
		m_vPost.push_back(m_post);
		m_vvComponent.push_back(m_vPre);		
		m_vvComponent.push_back(m_vC);		
		m_vvComponent.push_back(m_vDict);
		m_vvComponent.push_back(m_vRex);
		m_vvComponent.push_back(m_vPost);

	}
public:
	string NETrans(const string& sent/*,int lineid*/)
	{
		vMaplog.clear();
		string strTemp = sent;
		string strOut = strTemp;
		/*int id = lineid;*/
		bool pre_do = false;
		for (vector<vector<NEInterface*> >::iterator m_vvItr = m_vvComponent.begin();m_vvItr != m_vvComponent.end();m_vvItr++/*i in m_mapComponent*/)
		{
			//list<pair<int, int> > lTransed; // 记录在本层中已经被翻译的部分
			NEResult NETemp;
			//while(true)
			//{// 循环到没有东西被翻译
				bool bBreak = true;
				vector<NEInterface* > m_vComponent = (*m_vvItr);
				vector<NEInterface* >::iterator m_vItr;
				for (m_vItr = m_vComponent.begin(); m_vItr != m_vComponent.end(); m_vItr++ /*j in m_mapComponent[i]*/)
				{
					string temp_name = (*m_vItr)->m_Name;
					if( temp_name == "RexAllSub")
					{
						NEResult recres;
						NEResult n2c_rec;
						string str_Num;
						string str_ch;
						string str_RexRec = strOut;
						bool if_trans = false;
						for (unsigned int i = 0; i < (*m_vItr)->vNumRex.size(); i++)
						{
							recres = m_Trans2Num->GetRecResult(str_RexRec, (*m_vItr)->vNumRex[i]);
							if(recres.regxid != -1)       //嵌套数字的正则表达式可匹配成功
							{
								str_Num = m_Trans2Num->GetTransResult(recres,(*m_vItr)->vNumRex[i]);
								if_trans = (*m_vItr)->vNumRex[i].trans_ch;
								int range = (*m_vItr)->vNumRex[i].repnum;
								if( if_trans )
								{
									string temp_num2ch = strOut;
									n2c_rec.span = recres.sub_span;
									n2c_rec.str = str_Num;
									str_ch = m_Num2Chn->GetTransResult(n2c_rec, NETemp);
									if(str_ch != "")
									{
										if( range == 0)
										{
											strOut = Tools::StrTool::replace_all(temp_num2ch, recres.str, str_ch, n2c_rec.span.first, n2c_rec.span.second);
											vMaplog.insert(make_pair(recres.str, str_ch));
										}
										if( range == 1)
										{
											strOut = Tools::StrTool::replace_all(temp_num2ch, n2c_rec.str, str_ch, n2c_rec.span.first, n2c_rec.span.second);
											string pair_uy = recres.str;
											string pair_ch = Tools::StrTool::replace_all(pair_uy, n2c_rec.str, str_ch);
											map<string,string>::iterator iter;
											bool log_rep = false;
											for(iter = vMaplog.begin(); iter != vMaplog.end(); iter++)
											{
												string a = iter->first;
												string b = iter->second;
												if (b == recres.str)
												{
													vMaplog.erase(iter);
													string new_str = pair_ch;
													vMaplog.insert(make_pair(a, new_str));
													log_rep = true;
													break;
												}
											}
											/*if(log_rep == false)
												vMaplog.insert(make_pair(recres.str, pair_ch));*/
										}								
									}
								}
								else
								{
									vMaplog.insert(make_pair(recres.str, str_Num)); 
									strOut = Tools::StrTool::replace_all(strTemp, recres.str,str_Num, n2c_rec.span.first, n2c_rec.span.second);
								}
							}
							
						}
						
						//bBreak = false;// 先不跳出
						//break;
					}
					else if ( temp_name == "ComRegex")
					{
						NEResult res = (*m_vItr)->GetRecResult(strTemp,NETemp);
						if (res.regxid != -1) // 有被匹配上
						{
							strOut = res.SentOut;
							vMaplog.insert(make_pair(res.str,res.WordOut));
						}		
						continue;
					}
					else if ( temp_name == "PreProc")
					{	
						pre_do = m_ComRex->PreProc(strOut);
						bBreak = true;
					}
					else if ( temp_name == "PostProc")
					{
						m_ComRex->PostProc(strOut,pre_do);
						bBreak = true;
					}
					else 
						break;

				}
				//if (bBreak == true) // 如果没有东西被翻译跳出
				//	break;
			}
		return strOut;
	}
	private:
		bool  ReadNumConfig(const string& file)
		{
			ifstream fin (file.c_str());
			if(!fin)
			{
				cerr << "can not open file: " << file << endl;
				return false;
			}
			string line;
			unsigned int num = 0;
			while (getline(fin, line))
			{
				if (line == "[MinNum]")
					num = 1;
				else if (line == "[ChNum]")
					num = 2;
				else if (line == "[RecRexAllSub]")
					num = 3;
				else
				{
					switch (num)
					{
					case 1:
						{
							stringstream iss(line);
							string str;
							unsigned long long num = 0;
							iss >> str >> num;
							iss.clear();
							/*int test;
							cin >> test;*/
							m_Trans2Num->mMinNum.insert(make_pair(str, num));

						}
						break;
					case 2:
						{
							stringstream iss(line);
							string str;
							unsigned long long num = 0;
							iss >> str >> num;
							m_Num2Chn->mChNum.push_back(make_pair(num, str));

						}
						break;
					case 3:
						{
							vector<string> vRexData;
							Tools::StrTool::parseWord(line, "\t", vRexData);
							NEResult data;
							data.regall = new CRegexpT<char>(vRexData[0].c_str());
							data.regsub = new CRegexpT<char>(vRexData[1].c_str());
							data.repnum = Tools::StrTool::string2X(vRexData[2], data.repnum);
							if( vRexData.size() == 4)
								data.trans_ch = Tools::StrTool::string2X(vRexData[3], data.trans_ch);
							m_RexAllSub->vNumRex.push_back(data);

						}
						break;
					}
				}
			}
			m_mapComponent.insert(make_pair("Trans2Num", m_Trans2Num));

			m_Num2Chn->mChNum.sort();	
			m_mapComponent.insert(make_pair("Num2Chn",m_Num2Chn));

			m_mapComponent.insert(make_pair("RexAllSub",m_RexAllSub));
			m_vRex.push_back(m_RexAllSub);
			fin.close();
			return true;
		}

		bool  ReadNEConfig(const string& file)
		{
			string line;
			ifstream fin (file.c_str());
			if(!fin)
			{
				cerr << "can not open file: " << file << endl;
				return false;
			}
			while(getline(fin,line))
			{
				vector<string> vRex;
				Tools::StrTool::parseWord(line, "\t", vRex);
				m_ComRex->vRexStr.push_back(make_pair(vRex[0], vRex[1]));
				CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
				m_ComRex->vMapRegexp.push_back(pRex);
				m_ComRex->m_Name = "ComRegex";
			}
			m_mapComponent.insert(make_pair("ComRegex", m_ComRex));
			m_vC.push_back(m_ComRex);
			fin.close();
			return true;
		}
};
}
#endif

