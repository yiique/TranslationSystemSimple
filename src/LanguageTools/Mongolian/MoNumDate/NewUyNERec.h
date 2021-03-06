#ifndef _NEWMONE_H_
#define _NEWMONE_H_

#include <string>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <list>
#include "deelx.h"
#include "../../Tools2.h"

using namespace std;

namespace MoNumDate
{

class NewMoNERec
{
private:
	struct NumRecData
	{
		CRegexpT<char>* regall;
		CRegexpT<char>* regsub;
		unsigned short repnum;
		unsigned short rulenum;
	};
public:
	NewMoNERec(void) {}
	NewMoNERec(const string& ne_config, const string& num_config)
	{
		ReadNeConfig(ne_config);
		ReadNumConfig(num_config);
	}
	bool Init(const string& ne_config, const string& num_config)
	{
		return (ReadNeConfig(ne_config) & ReadNumConfig(num_config));
	}
public:
	string NeRec(const string& strIn, const string& arab, const bool& rec_log = true)
	{
		string strTemp = strIn;
		bool replace_flag = PreProc(strTemp);
		RecRex(strTemp, rec_log);
		RecNum(strTemp, rec_log, arab);
		PostProc(strTemp, replace_flag);
		PostRecRex(strTemp, rec_log);
		return strTemp;
	}
	void output_log(const string& file, const string& arab)
	{
		ofstream fout(file.c_str());
		for (map<string, string>::iterator itr = mMapLog.begin(); itr != mMapLog.end(); itr++)
		{
			if (itr->second == "")
				fout << itr->first << "\t" <<  NeRec(itr->first,arab, false) << endl;
			else
				fout << itr->first << "\t" <<  itr->second << endl;
		}
		fout.close();
	}

	void OutputNERule(const string& strIn, vector<string>& dict, const string& arab)
	{
		map<string, string> strDict;
		for (map<string, string>::iterator itr = mMapLog.begin(); itr != mMapLog.end(); itr++)
		{
			strDict.insert(make_pair(itr->first, NeRec(itr->first,arab, false)));
		}
		vector<string> vWords;
		Tools::StrTool::parseWord(strIn, " \t", vWords);
		for (map<string, string>::iterator itr = strDict.begin(); itr != strDict.end(); itr++)
		{
			vector<string> vDictWords;
			Tools::StrTool::parseWord(itr->first, " \t", vDictWords);
			if (vDictWords.size() == 0)
				continue;
			vector<pair<int, int> > vSP = _GetVecPos(vWords, vDictWords);
			for (unsigned int i = 0; i < vSP.size(); i++)
			{
				stringstream sstream;
				sstream << vSP[i].first+1 << ":" << vSP[i].second+1 << " ||| " << Tools::StrTool::vec2str(vDictWords) << " ||| " << itr->second << " ||| 1 1 1 1";
				dict.push_back(sstream.str());
			}
		}
		mMapLog.clear();
	}
private:
	bool PreProc(string& strTemp)
	{
		static const CRegexpT<char> RexSpaceAdd("[\\s]+\\+");
		if (RexSpaceAdd.Match(strTemp.c_str()).IsMatched() == true)
		{
			strTemp = RexSpaceAdd.Replace(strTemp.c_str(), "+");
			return true;
		}
		return false;
	}
	void PostProc(string& strTemp, const bool& replace_flag)
	{
		if (replace_flag == true)
			strTemp = Tools::StrTool::replace_all(strTemp, "+", " +");
		const char di[] = {0xE7, 0xAC, 0xAC, 0};
		const char yi[] = {0xE4, 0xB8, 0x80, 0};
		const char shi[] = {0xE5, 0x8D, 0x81, 0};
		string org = string(di)+string(yi)+string(shi);
		string tgt = string(di)+string(shi);
		strTemp = Tools::StrTool::replace_all(strTemp, org, tgt);
		string org2 = " "+string(yi)+string(shi);
		string tgt2 = " "+string(shi);
		strTemp = Tools::StrTool::replace_all(strTemp, org2, tgt2);

		int pos = strTemp.find("@");
		while ( pos != string::npos )
		{
			strTemp.replace(pos,1,"");
			pos = strTemp.find("@");
		}

	}
	void RecRex(string& strIn, const bool& rec_log)
	{
		//cout << "Start RecRex..." << endl;
		string repTemp = strIn;
		for (unsigned int i = 0; i < vMapRegexp.size(); i++)
		{
			MatchResult result = vMapRegexp[i]->Match(repTemp.c_str());
			//int ct = 0;
			if (result.IsMatched() == true)//while (result.IsMatched() == true)
			{
				//cout << ++ct << endl;
				string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
				if (rec_log == true)
					mMapLog.insert(make_pair(got, ""));
				repTemp = vMapRegexp[i]->Replace(repTemp.c_str(), vRexStr[i].second.c_str());

// 				cout << repTemp << endl;
			}
		}
		strIn = repTemp;
	}

	void PostRecRex(string& strIn, const bool& rec_log)
	{
		//cout << "Start RecRex..." << endl;
		string repTemp = strIn;
		for (unsigned int i = 0; i < vMapRegexp2.size(); i++)
		{
			MatchResult result = vMapRegexp2[i]->Match(repTemp.c_str());
			//int ct = 0; 
			if (result.IsMatched() == true) //while (result.IsMatched() == true)
			{
				//cout << ++ct << endl;
				string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
				if (rec_log == true)
					mMapLog.insert(make_pair(got, ""));
				repTemp = vMapRegexp2[i]->Replace(repTemp.c_str(), vRexStr2[i].second.c_str());
				//result = vMapRegexp2[i]->Match(repTemp.c_str());
			}
		}
		strIn = repTemp;
	}

private:

	void RecNum(string& strIn, const bool& rec_log, const string& arab)
	{
		for (unsigned int cnt = 0; cnt < vNumRex.size(); ++cnt)
		{		
			//下面是用阿拉伯（原文用蒙文表示的数词强行用阿拉伯数字）
			if (arab == "yes" && ( cnt == 5 || cnt == 6 ) )
				continue;
			
			RecRexAll(strIn, vNumRex[cnt], rec_log, arab);
		}
		return;
	}
	void RecRexAll(string& strIn, const NumRecData& regdata, const bool& rec_log, const string& arab)//一句话针对一个模板不断匹配
	{
		int cnt = regdata.rulenum;
		long long longNum = 0;//hy
		MatchResult result;//hy
		string strIn2;
		unsigned int rep_start2;
		unsigned int rep_end2;
		string rep_str2, strChNum = "";
		int ct1 = 0;
		string tmpStr = "";

		while(true&&ct1<20)
		{
			++ct1;
			result = regdata.regall->Match(strIn.c_str());//针对[RecRexAllSub]部分
			if (result.IsMatched() == true)
			{
				int intStart = result.GetStart();
				int intEnd = result.GetEnd();
				string strUyNum = strIn.substr(intStart, intEnd-intStart);
				unsigned int rep_start = result.GetGroupStart(regdata.repnum);
				unsigned int rep_end = result.GetGroupEnd(regdata.repnum);
				string rep_str = strIn.substr(rep_start, rep_end-rep_start);

				if ( cnt == 0 && arab == "yes" )//小数模板
				{
					int pos = strUyNum.find(" ");
					while (pos!= string::npos)
					{
						strUyNum.replace(pos,1,"");
						pos = strUyNum.find(" ");
					}
					const char dian[] = {0xE7, 0x82, 0xB9, 0};
					pos = strUyNum.find(string(dian));
					if ( pos != string::npos )
						strUyNum.replace(pos,3,".");
					strUyNum += " ";
					strUyNum = tmpStr + " " +strUyNum;
					strIn = Tools::StrTool::replace_all(strIn, rep_str, strUyNum, rep_start, rep_end);
					if (rec_log == true)
						mMapLog.insert(make_pair(" "+strUyNum, ""));
					continue;
				}

				if ( cnt == 2 || cnt == 3 )//用于识别复杂数字时先去掉各个@
				{
					int pos = strUyNum.find("@");
					while ( pos != string::npos )
					{
						strUyNum.replace(pos,1,"");
						pos = strUyNum.find("@");
					}
				}

				//专门处理序数词第x,只有x;
				if  ( cnt == 1 || cnt == 7 ) 
				{
					const char di[] = {0xE7, 0xAC, 0xAC, 0};
					int pos = strUyNum.find(string(di));
					if ( pos != string::npos )
					{
						strUyNum.replace(pos,3," ");
						strUyNum = string(di) + " " + strUyNum;
						strIn = Tools::StrTool::replace_all(strIn, rep_str, strUyNum, rep_start, rep_end);
						if (rec_log == true)
							mMapLog.insert(make_pair(" "+strUyNum, ""));						
						continue;
					}
					const char zhiyou[] = {0xE5,0x8F,0xAA,0xE6,0x9C,0x89,0};
					pos = strUyNum.find(string(zhiyou));
					if ( pos != string::npos )
					{
						strUyNum.replace(pos,6," ");
						strUyNum = string(zhiyou) + " " + strUyNum;
						strIn = Tools::StrTool::replace_all(strIn, rep_str, strUyNum, rep_start, rep_end);
						if (rec_log == true)
							mMapLog.insert(make_pair(" "+strUyNum, ""));
						continue;
					}
					else
					{
						cout << "error rule " << cnt << endl;
						break;
					}
				}

				//补上“日”
				if ( cnt == 9 )
				{
					const char ri[] = {0xE6, 0x97, 0xA5, 0};
					string strPost = strIn.substr(intEnd);
					stringstream istr(strPost.c_str());
					string tmp;
					istr >> tmp;
					int pos = tmp.find(string(ri));
					if ( pos == string::npos )
					{
						strUyNum += string(ri);
					}
					int pos1 = strUyNum.find(" ");
					while (pos1!= string::npos)
					{
						strUyNum.replace(pos1,1,"");
						pos1 = strUyNum.find(" ");
					}
					strIn = Tools::StrTool::replace_all(strIn, rep_str, strUyNum, rep_start, rep_end);
					if (rec_log == true)
						mMapLog.insert(make_pair(" "+strUyNum, ""));
					continue;
				}

			    //针对“百分之\d+"转化成“\d+%"
				if ( cnt == 10 && arab == "yes" )
				{
					const char baifenzhi[] = {0xE7, 0x99, 0xBE,0xE5,0x88,0x86,0xE4,0xB9,0x8B,0};
					int pos = strUyNum.find(string(baifenzhi));
					if ( pos != string::npos )
						strUyNum.replace(pos,9,"");
					strUyNum += "% ";
					strIn = Tools::StrTool::replace_all(strIn, rep_str, strUyNum, rep_start, rep_end);
					if (rec_log == true)
						mMapLog.insert(make_pair(" "+strUyNum, ""));
					continue;
				}

				if (rec_log == true)
					mMapLog.insert(make_pair(" "+strUyNum, ""));

				longNum = GetRexSub(strUyNum, regdata,tmpStr);//longNum用于后续转化成中文时候用；         

				//下面是翻译成汉字(原文是蒙文则用汉字表达)
				if ( arab == "no" )//|| cnt == 11 )//cnt == 11时针对“二|三...分之\d+"一定转化成全汉字形式
				{
					if (strUyNum.empty())
						strChNum = "";
					else
					{
						if ( longNum >=0 )
							strChNum = GetChStrNum(longNum,cnt,tmpStr);//hy 
						else continue;

						int pos = strChNum.find(" ");
						while (pos!= string::npos)
						{
							strChNum.replace(pos,1,"");
							pos = strChNum.find(" ");
						}
						strChNum += " ";
						strChNum = tmpStr + " " +strChNum;
					}
					strIn = Tools::StrTool::replace_all(strIn, rep_str, strChNum, rep_start, rep_end);
					if (rec_log == true)
					    mMapLog.insert(make_pair(" "+strChNum, ""));
				}
				else//下面是翻译成阿拉伯文（原文用蒙文表示的数词强行用阿拉伯数字）
				{
					strUyNum += " ";
					strUyNum = tmpStr + " " +strUyNum;
					strIn = Tools::StrTool::replace_all(strIn, rep_str, strUyNum, rep_start, rep_end);
					if (rec_log == true)
					    mMapLog.insert(make_pair(" "+strUyNum, ""));
				}
			}
			else
				break;
		}
	}
	long long GetRexSub(string& str, const NumRecData& regdata, string& tmpStr)//unsigned long long GetUyLongNum(const string& str, const NumRecData& regdata)
	{
		//return -1:出错时；
		//return 其他值：str本身的value就是这个值，包括0；或者str = 0 JAGV 也会返回0
		int cnt = regdata.rulenum;
		long long num = 0,tmpNum = 0;
		char t[256];//hy

		int pos = str.find("0 JAGV");
		while ( pos != string::npos )
		{
			str.replace(pos,6,"");
			pos = str.find("0 JAGV");
		}
		stringstream istr(str);
		int ct = 0;
		string tmp = "";
		while ( istr >> tmp )
		{
			++ct;
		}
		if ( ct == 0)
		{
			str = "";
			return 0;
		}

		int findStart = -1;
		MatchResult result = regdata.regsub->Match(str.c_str(), findStart);
		ct = 0;
		while (result.IsMatched() == true && ct < 20)
		{
			++ct;
			int intStart = result.GetStart();
			int intEnd = result.GetEnd();
			string strUyNumSub = str.substr(intStart, intEnd-intStart);
			string tmp = strUyNumSub;
			tmpNum = GetRexSubNum(strUyNumSub);
			if ( tmp != strUyNumSub )//传进去的是str =二 dUNG  这种，返回“二”
				tmpStr = strUyNumSub;
			if (tmpNum >= 0)//没出错,持续加，其实只有cnt == 1或者2用得上这种持续加
				num += tmpNum;
			else
				return tmpNum;
			result = regdata.regsub->Match(str.c_str(), intEnd);
		}
		sprintf(t,"%lld",num);
		str = t;//当强制 只要阿拉伯，不必转成中文时，则需要返回str给strUyNum来替换原始的匹配处；
		return num;
	}
	long long GetRexSubNum(string& str)// str= 1 MINGG; str= 0 MINGG; str= 1.1 MINGG(没测）;//str = 0 or 1 or 1.1; str = TUME; //防止两个都不是数字，防止出现两个以上单元。
	{
		//返回-1：当出现两个以上单元的错;
		//返回阿拉伯数字：str = 0 or 1 or 1.1 or 1 MINGG or JAGV;
		//尤其是返回0时：str = 0 or 0 MINGG;
		//另外，如果str = 二 MINGG,那么会 返回的str会是二；
		float num = 0;
		string str2 = "",tmpStr = "";
		long long temp;
		int ct = 0;
		vector<string> vStr;
		stringstream sstream(str);
		while ( sstream >> tmpStr )
		{
			++ct;
			vStr.push_back(tmpStr);
		}
		tmpStr = "";
		if (ct >2 || ct == 0)
			return -1;
		if ( ct == 1 )
		{
			if (vStr[0]=="0")
				return 0;
			else
			{
				stringstream sstr(vStr[0]);
				sstr >> num;
				if (num == 0)
					str2 = str;//str = TUME（单独一个单词）
				else return num;
			}
		}
		else//ct == 2
		{
			stringstream sstrr(str);
			sstrr >> num; 
			if (num == 0)
			{
				if (vStr[0] == "0")
					return num;
				else
				{
					str2 = vStr[1];//str = MINGG TUME;
					str = vStr[0];
				}
			}
			else 
				sstrr >> str2;// str= 1 MINGG
		}
		for (int itr=0; itr<vUyNum.size(); itr++ )
		{
			string::size_type pos = str2.find(vUyNum[itr].first);
			if (pos != string::npos && pos == 0)
			{
				if ( num == 0 )
					temp = vUyNum[itr].second;
				else
					temp = num * vUyNum[itr].second;
				return temp;
			}
		}
		return num;
	}

	string GetChStrNum(const long long& num, int& cnt, const string& tmpStr, const bool& bZero = false)
	{
		string strChNum;
		for (list<pair<unsigned long long, string> >::reverse_iterator ritr = mChNum.rbegin(); ritr != mChNum.rend(); ritr++)
		{
			//ritr->first是除数
			if ( ritr->first == 0 )
				return ritr->second;
			unsigned long long quotient = num / ritr->first; // 商
			unsigned long long remaider = num % ritr->first; // 余数
			if (quotient != 0)
			{
				if (quotient == 1 && ritr->first < 10)//零~九
				{
					if (bZero == false)
						return ritr->second;
					else
						return mChNum.begin()->second + ritr->second;
				}

				//记录下除数，处理商，加在前面
				if (bZero == true) // 如果需要加零
					strChNum += mChNum.begin()->second + GetChStrNum(quotient,cnt,tmpStr) + ritr->second;
				else//新加
				{	
					if ( quotient == 1 && (( cnt == 3 && !tmpStr.empty()) 
						|| cnt == 4 ) ) //去掉亿、万前面的一，其它时候不能不考虑商哦
						strChNum += ritr->second;
					else
						strChNum += GetChStrNum(quotient,cnt,tmpStr) + ritr->second;
				}//新加

				//处理余数，加在后面
				if (remaider == 0)
					return strChNum;
				if (remaider*10 < ritr->first)
					strChNum += GetChStrNum(remaider, cnt,tmpStr, true);
				else
					strChNum += GetChStrNum(remaider, cnt,tmpStr, false);
				return strChNum;
			}
		}
		return strChNum;
	}

	vector<pair<int, int> > _GetVecPos(const vector<string>& vWords, const vector<string>& vPos)
	{
		vector<pair<int, int> > vSpan;
		if (vPos.size() == 0)
			return vSpan;

		for (unsigned int i = 0; i < vWords.size(); i++)
		{
			if (_GetVecPos(i, vWords, vPos) == true)
				vSpan.push_back(make_pair(i, i+vPos.size()-1));
		}
		return vSpan;
	}

	bool _GetVecPos(const unsigned int& _pos, const vector<string>& vWords, const vector<string>& vPos)
	{
		unsigned int uPos = 0;
		for (unsigned int i = _pos; i < vWords.size() && uPos < vPos.size(); i++, uPos++)
		{
			if (vWords[i] != vPos[uPos])
				return false;
		}
		if (uPos != vPos.size())
			return false;
		return true;
	}
private:
	vector<CRegexpT<char>* > vMapRegexp;
	vector<pair<string, string> > vRexStr;
	vector<pair<string, string> > vRexStr2;
	vector<CRegexpT<char>* > vMapRegexp2;
	vector<NumRecData> vNumRex;
	vector<pair<string, unsigned long long>> vUyNum;//map<string, unsigned long long> mUyNum;
	list<pair<unsigned long long, string> > mChNum;
	map<string, string> mMapLog;
private:
	bool ReadNeConfig(const string& file)
	{
		ifstream fin(file.c_str());
		if (fin.good() == false)
			return false;
		string line;
		int ct = 0;
		//cout << "ReadNeConfig" << endl;
		while (getline(fin, line))
		{
			++ct;
			//cout << ct << endl;
			if ( ct == 1  && 
				(unsigned char) line[0] == 0xEF && 
				(unsigned char) line[1] == 0xBB	&& 
				(unsigned char) line[2] == 0xBF)
				line = line.substr(3);
			vector<string> vRex;
			Tools::StrTool::parseWord(line, "\t", vRex);
			if (vRex.size()<1)
				continue;
			if (line.substr(0,2) == "//")
				continue;
			vRexStr.push_back(make_pair(vRex[0], vRex[1]));
			CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
			vMapRegexp.push_back(pRex);
		}
		fin.close();
		return true;
	}
	bool ReadNumConfig(const string& file)
	{
		ifstream fin(file.c_str());
		if (fin.good() == false)
			return false;
		string line;
		unsigned int num = 0;
		int ct = 0;
		//cout << "ReadNumConfig" << endl;
		while (getline(fin, line))
		{
			++ct;
			//cout << ct << endl;
			if( ct == 1 && // 文件头<BOM>的过滤
				(unsigned char)line[0] == 0xEF && 
				(unsigned char)line[1] == 0xBB && 
				(unsigned char)line[2] == 0xBF)
				line = line.substr(3);

			if (line == "[MoNum]")
				num = 1;
			else if (line == "[ChNum]")
				num = 2;
			else if (line == "[RecRexAllSub]")
				num = 3;
			else if (line == "[Post]")
				num =4;
			else if (line.substr(0,2) == "//")
				continue;
			else
			{
				switch (num)
				{
				case 1:
					{
						stringstream sstream(line);
						string str;
						unsigned long long num = 0;
						sstream >> str >> num;
						vUyNum.push_back(make_pair(str, num));//mUyNum.insert(make_pair(str, num));
					}
					break;
				case 2:
					{
						stringstream sstream(line);
						string str;
						unsigned long long num = 0;
						sstream >> str >> num;
						mChNum.push_back(make_pair(num, str));
					}
					break;
				case 3:
					{
						vector<string> vRexData;
						Tools::StrTool::parseWord(line, "\t", vRexData);
						if (vRexData.size()<1)
							continue;
						NumRecData data;
						data.regall = new CRegexpT<char>(vRexData[0].c_str());
						data.regsub = new CRegexpT<char>(vRexData[1].c_str());
						data.repnum = Tools::StrTool::string2X(vRexData[2], data.repnum);
						data.rulenum = Tools::StrTool::string2X(vRexData[3], data.rulenum);
						vNumRex.push_back(data);
					}
					break;
				case 4:
					{										
						vector<string> vRex;
						Tools::StrTool::parseWord(line, "\t", vRex);
						if (vRex.size()<1)
							continue;
						if (line.substr(0,2) == "//")
							continue;
						vRexStr2.push_back(make_pair(vRex[0], vRex[1]));
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						vMapRegexp2.push_back(pRex);
					}
					break;
				}
			}
		}
		mChNum.sort();
		fin.close();
		return true;
	}
};

}

#endif
