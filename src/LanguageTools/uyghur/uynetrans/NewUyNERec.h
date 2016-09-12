#pragma once

#include <string>
#include <vector>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <list>
#include "deelx.h"
#include "NewTools.h"
#include <fstream>

using namespace std;
using namespace LKTools;

struct NumRecData 
{	
	CRegexpT<char>* regall;
	CRegexpT<char>* regsub;
	unsigned short repnum;	
};

class NewUyNERec
{
public:

private:

	class Tripple
	{
	public:
		Tripple(void)
		{
		}
		string RecRegex;
		string TransRegex;
		int d;
	};
	class Number
	{
	public:
		Number(void)
		{
		}
		string str;
		string stepstr;
		string trans;
		pair<int,int> span;
	};
	
public:
	NewUyNERec(const string& number_config, const string& time_config, const string& ne_config)
	{
		ReadNumberConfig(number_config);
		ReadTimeConfig(time_config);
		ReadNeConfig(ne_config);
	}
public:
	string NeRec(const string& strIn, const bool& rec_log = true)
	{
		string strTemp = strIn;
		bool replace_flag = PreProc(strTemp);
		//lout<<"PreProc done"<<endl;
		RecNumber(strTemp, rec_log);
		//lout<<"RecNumber done"<<endl;
		RecTime(strTemp, rec_log);	
		//lout<<"RecTime done"<<endl;
		Num2Chn(strTemp, rec_log);
		//lout<<"Num2Chn done"<<endl;
		PostProc(strTemp, replace_flag);
		//lout<<"PostProc done"<<endl;
		return strTemp;
	}
	//1：4 ||| 3 月 4 日 ||| May，3 ||| 111
	vector<string>ne_trans_table(const string &input,const string& prule)
	{
		vector<string> vtable;
		for( unsigned int i = 0; i < vNumbers.size(); i++)
		{
			if( vNumbers[i].trans != "")
			{
				string temp = vNumbers[i].str;
				/*Replace_all(temp, " ", "");*/
				stringstream sstream;
				sstream << vNumbers[i].span.first+1 << " : " << vNumbers[i].span.second+1;
				string p = prule;
				string tab = sstream.str()  + " ||| " + temp + " ||| " + vNumbers[i].trans + " ||| " + p + " " + p + " " + p + " " + p ;
				vtable.push_back(tab);
			}	
		}
		return vtable;
	}
	//string ne_trans_tag(const string &input)
	//{
	//	string out = input;
	//	for( unsigned int i = 0; i < vNumbers.size(); i++)
	//	{
	//		if( vNumbers[i].trans != "")
	//		{
	//			string temp = vNumbers[i].str;
	//			Replace_all(temp, " ", "");
	//			//我于<ne translation="in 1999">1999年</ne>进入<ne translation="Central Commission for Discipline Inspection of the CPC">中纪委</ne>工作
	//			string label;
	//			label = "<UYNT translation=\"" + vNumbers[i].trans + "\">" + temp + "</UYNT>";
	//			Replace_all(out, vNumbers[i].str, label, vNumbers[i].span.first+i);
	//		}	
	//	}
	//	return out;
	//}
	//string ne_rec_tag(const string &input)
	//{
	//	string out = input;
	//	for( unsigned int i = 0; i < vNumbers.size(); i++)
	//	{
	//		if( vNumbers[i].trans != "")
	//		{
	//			string temp = vNumbers[i].str;
	//			Replace_all(temp, " ", "");
	//			//我于<zone>1999年</zone>进入<zone>中纪委</zone>工作
	//			string label;
	//			label = "<ZONE>" + temp + "</ZONE>";
	//			Replace_all(out, vNumbers[i].str, label, vNumbers[i].span.first+i);
	//		}	
	//	}
	//	return out;
	//}

	//void output_log(const int& id, ofstream& fout)
	//{
	//	/*ofstream fout(file.c_str());*/
	//	for( unsigned int i = 0; i < vNumbers.size(); i++)
	//	{
	//		if( vNumbers[i].trans != "")
	//		{
	//			string temp = vNumbers[i].str;
	//			Replace_all(temp, " ", "");
	//			fout << id << '\t' << temp << '\t' << vNumbers[i].span.first << " " << vNumbers[i].span.second << '\t' << vNumbers[i].trans << endl;
	//			/*fout << id << '\t' << vNumbers[i].str << '\t' << vNumbers[i].span.first << " " << vNumbers[i].span.second << '\t' << vNumbers[i].trans << endl;*/
	//		}	
	//	}
	//}
	//void output_log(ofstream& fout)
	//{
	//	/*ofstream fout(file.c_str());*/
	//	for( unsigned int i = 0; i < vNumbers.size(); i++)
	//	{
	//		if( vNumbers[i].trans != "")
	//		{
	//			string temp = vNumbers[i].str;
	//			Replace_all(temp, " ", "");
	//			fout << temp << '\t' << vNumbers[i].trans << endl;
	//			/*fout << vNumbers[i].str << '\t' << vNumbers[i].trans << endl;*/
	//		}
	//	}		
	//}
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
			strTemp = Replace_all(strTemp, "+", " +");
		const char di[] = {0xE7, 0xAC, 0xAC, 0};
		const char yi[] = {0xE4, 0xB8, 0x80, 0};
		const char shi[] = {0xE5, 0x8D, 0x81, 0};
		string org = string(di)+string(yi)+string(shi);
		string tgt = string(di)+string(shi);
		strTemp = Replace_all(strTemp, org, tgt);
		string org2 = " "+string(yi)+string(shi);
		string tgt2 = " "+string(shi);
		strTemp = Replace_all(strTemp, org2, tgt2);
	}
	void RecTime(string& strIn, const bool& rec_log)
	{
		string repTemp = strIn;
		for (unsigned int i = 0; i < vMapRegexp.size(); i++)
		{
			while(true)
			{
				MatchResult result = vMapRegexp[i]->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					
					string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
					if (result.GetStart()==0|| repTemp[result.GetStart()-1]==' ')
					{
						pair<int, int> span = GetSpanByStr(repTemp, got);
						string temp = got;
						string trans;
						if (vRexStr[i].second == "KEEP")
						{
							trans = Replace_all(temp," ","");
						}
						else
						{
							trans = vMapRegexp[i]->Replace(temp.c_str(), vRexStr[i].second.c_str());
						}
						if (rec_log == true)
						{
							lout<<"RecTime cmblog"<<endl;
							vNumbers = CombLog(span, trans, vNumbers);	
						}
						repTemp = Replace_all(repTemp, got, trans);
					}
					else
						repTemp = Replace_all (repTemp,got, " ");
					/*repTemp = vMapRegexp[i]->Replace(repTemp.c_str(), vRexStr[i].second.c_str());*/
				}
				else
					break;
				}
		}
		strIn = repTemp;
	}
	void RecNumber(string& strIn, const bool& rec_log)
	{
		vector<string> vtokens;
		ParseWord(strIn, " ", vtokens);
		vNumbers = Init(strIn);

		string repTemp = strIn;
		//替换基数词 一词对一数字 不会改变词的位置
		for (unsigned int i = 0; i < vBasicRegexp.size(); i++)
		{
			while(true)
			{
				MatchResult result = vBasicRegexp[i]->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
					
					if (result.GetStart()==0|| repTemp[result.GetStart()-1]==' ')
					{
						if (rec_log == true)
						{	
							int pos = GetPosByWord(repTemp, got);
							vNumbers[pos].trans = vBasicRegexp[i]->Replace(vNumbers[pos].stepstr.c_str(), vBasicTri[i].TransRegex.c_str());/*vBasicTri[i].TransRegex;*/
							vNumbers = FreshLog(vNumbers);
						}
						repTemp = vBasicRegexp[i]->Replace(repTemp.c_str(), vBasicTri[i].TransRegex.c_str());
					}
					else
						repTemp = Replace_all (repTemp,got, " ");
				}
				else
					break;
			}
		}		
		/*cout << strIn << endl;
		cout << repTemp << endl;*/
		//合并数词
		
		for(unsigned int i = 0; i < vNumCombRegexp.size(); i++)
		{
			while(true)
			{
				MatchResult result = vNumCombRegexp[i]->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
				
					//去掉词缀只含数字的部分进行加和
					int subStart = result.GetGroupStart(vNumCombTri[i].d);
					int subEnd = result.GetGroupEnd(vNumCombTri[i].d);
					string substr = repTemp.substr(subStart, subEnd-subStart);
					stringstream iss(substr);
					string ntoken;
					unsigned int sum = 0;
					while(iss >> ntoken)
					{
						unsigned int n = String2X(ntoken,n);
						sum += n;
					}	
					pair<int, int> span = GetSpanByStr(repTemp, got);
					string trans = X2String(sum);
					if (rec_log == true)
						vNumbers = CombLog(span, trans, vNumbers);						
					repTemp = Replace_all(repTemp, got, trans);
				}
				else
					break;
			}
		}
		/*cout << repTemp << endl;*/
		//大位数数词识别 多词合并成一个数字 
		for (unsigned int i = 0; i < vNumRex.size(); i++)
		{
			/*RecNum(strIn, repTemp, vNumRex[i], rec_log);*/
			while(true)
			{
				int intStart, intEnd;
				int sub_start, sub_end;
				string strUyNum, sub_str, trans;
				unsigned long long longNum;
				MatchResult result = vNumRex[i].regall->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					intStart = result.GetStart();
					intEnd = result.GetEnd();
					strUyNum = repTemp.substr(intStart, intEnd-intStart);		//lading numerial + suffix
					/*cout << "strUyNum: " << strUyNum << endl;*/

					sub_start = result.GetGroupStart(vNumRex[i].repnum);
					sub_end = result.GetGroupEnd(vNumRex[i].repnum);
					sub_str = repTemp.substr(sub_start, sub_end-sub_start);	//lading numerial part
					longNum = GetUyLongNum(sub_str, vNumRex[i]);				// lading numerial to arab 
					pair<int, int> span = GetSpanByStr(repTemp, strUyNum);
					string needtran = strUyNum;
					trans = Replace_all(needtran, sub_str, X2String(longNum));	//numerial + suffix				
					if (rec_log == true)
						vNumbers = CombLog(span, trans, vNumbers);	

					repTemp = Replace_all(repTemp, strUyNum, trans);				//replace 5 minginchi --> 5000inchi
				}
				else
					break;		
			}
		}
		/*cout << repTemp << endl;*/
		//数中含词缀 分数 小数
		for (unsigned int i = 0; i < vDecimalRegexp.size(); i++)
		{
			while(true)
			{
				MatchResult result = vDecimalRegexp[i]->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
					pair<int, int> span = GetSpanByStr(repTemp, got);
					string temp = got;
					string trans =  vDecimalRegexp[i]->Replace(got.c_str(), vDecimalTri[i].TransRegex.c_str());
					if (rec_log == true)
						vNumbers = CombLog(span, trans, vNumbers);	
					repTemp = Replace_all(repTemp, got, trans);
				}
				else
					break;
			}
		}
		/*cout << repTemp << endl;*/
		//百分数和倍数
		for (unsigned int i = 0; i < vMultipleRegexp.size(); i++)
		{
			while(true)
			{
				MatchResult result = vMultipleRegexp[i]->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
					pair<int, int> span = GetSpanByStr(repTemp, got);
					string temp = got;
					string trans =  vMultipleRegexp[i]->Replace(temp.c_str(), vMultipleTri[i].TransRegex.c_str());
					if (rec_log == true)
						vNumbers = CombLog(span, trans, vNumbers);		
					repTemp = Replace_all(repTemp, got, trans);
					/*repTemp = vMultipleRegexp[i]->Replace(repTemp.c_str(), trans.c_str());*/
				}
				else
					break;
			}
		}
		/*cout << repTemp << endl;*/
		//序数词(inchi|nchi)
		for (unsigned int i = 0; i < vOrdinalRegexp.size(); i++)
		{
			while(true)
			{
				MatchResult result = vOrdinalRegexp[i]->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
					pair<int, int> span = GetSpanByStr(repTemp, got);
					string temp = got;
					string trans =  vOrdinalRegexp[i]->Replace(temp.c_str(),vOrdinalTri[i].TransRegex.c_str());
					if (rec_log == true)
						vNumbers = CombLog(span, trans, vNumbers);		
					repTemp = Replace_all(repTemp, got, trans);
					/*repTemp = vOrdinalRegexp[i]->Replace(repTemp.c_str(), trans.c_str());*/
				}
				else
					break;
			}
		}
		/*cout << repTemp << endl;*/
		//含有X的模板	
		for (unsigned int i = 0; i < vNumberRegexp.size(); i++)
		{
			while(true)
			{
				MatchResult result = vNumberRegexp[i]->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
					pair<int, int> span = GetSpanByStr(repTemp, got);
					string temp = got;
					string trans;
					if(  vNumberTri[i].TransRegex == "KEEP" )
						trans = Replace_all(temp," ", "");
					else
						trans =  vNumberRegexp[i]->Replace(got.c_str(), vNumberTri[i].TransRegex.c_str());
					/*string tt =  vNumberRegexp[i]->Replace(got.c_str(), vNumberTri[i].TransRegex.c_str());
					string trans = Replace_all(tt, " ", "");*/
					if (rec_log == true)
						vNumbers = CombLog(span, trans, vNumbers);		
					repTemp = Replace_all(repTemp, got, trans);
					/*repTemp = vNumberRegexp[i]->Replace(repTemp.c_str(), vNumberTri[i].TransRegex.c_str());*/
				}
				else
					break;
			}
		}
		/*cout << repTemp << endl;*/
		strIn = repTemp;
	}
	
private:

	//初始化句子为结构化的向量
	vector<Number> Init(const string& sent)
	{
		vector<Number> vtokens;
		stringstream iss(sent);
		string word;
		int i = 0;
		while(iss >> word)
		{
			Number num;
			num.str = word;
			num.stepstr = word;
			num.trans = "";
			num.span = make_pair(i,i);
			vtokens.push_back(num);
			i++;			
		}
		return vtokens;
	}

	//更新Log信息，翻译结果代替当前单词
	vector<Number> FreshLog(vector<Number> vNumbers)
	{
		vector<Number> res;
		Number num;
		for(unsigned int i = 0; i < vNumbers.size(); i++)
		{
			if( vNumbers[i].trans != "")
				vNumbers[i].stepstr = vNumbers[i].trans;
			num.span = vNumbers[i].span;
			num.stepstr = vNumbers[i].stepstr;
			num.str = vNumbers[i].str;
			num.trans = vNumbers[i].trans;
			res.push_back(num);
		}
		return res;
	}
	//合并span内的单词，用trans代替
	vector<Number> CombLog(pair<int,int> span, string trans,vector<Number> vNumbers)
	{
		vector<Number> res;
		for( int i = 0; i < span.first; i++ )
		{
			Number num;
			num.span = vNumbers[i].span;
			num.stepstr = vNumbers[i].stepstr;
			num.str = vNumbers[i].str;
			num.trans = vNumbers[i].trans;
			res.push_back(num);
		}
		Number newone;
		string str;
		for( int j = span.first ; j <= span.second ; j++)
		{
			str  = str + vNumbers[j].str + " ";
		}
		newone.str = str.substr(0, str.size()-1);
		newone.trans = trans;
		newone.stepstr = newone.trans;
		newone.span = CombSpan(span, vNumbers);
		res.push_back(newone);
		int k = (int)res.size();
		int vk = k + span.second - span.first;
		while( vk < (int)vNumbers.size())
		{
			Number num;
			num.span = vNumbers[vk].span;
			num.stepstr = vNumbers[vk].stepstr;
			num.str = vNumbers[vk].str;
			num.trans = vNumbers[vk].trans;
			res.push_back(num);
			vk++;
		}
		return res;
	}
	//将span信息合并
	pair<int, int> CombSpan(pair<int, int> span,vector<Number> vNumber)
	{
		int start = vNumber[span.first].span.first;
		int end = vNumbers[span.second].span.second;
		return make_pair(start,end);
	}
	//返回字符串在句子中的span信息
	pair<int, int> GetSpanByStr(const string& strIn, const string& subStr)
	{
		string front = strIn.substr(0, strIn.find(subStr));
		vector<string> vFront, vSubStr;
		ParseWord(front, " ", vFront);
		ParseWord(subStr, " ", vSubStr);
		int s = (int)vFront.size();
		int e = (int)vFront.size() + vSubStr.size() - 1;
		return make_pair(s, e);
	}

	//返回字符串起始位置
	 int GetPosByWord(const string& strIn, const string& word)
	{
		vector<string> vFront;
		ParseWord(strIn.substr(0, strIn.find(word)), " ", vFront);
		return (int)vFront.size();
	}

	////通过span返回原句的字符串
	//string GetStrBySpan(const string& strIn, const pair<unsigned int, unsigned int>& span)
	//{
	//	vector<string> vRex;
	//	ParseWord(strIn, " ", vRex);
	//	string newStr;
	//	for(int i = span.first; i <= span.second+ span.first; i++)
	//		newStr = newStr + vRex[i] + " ";
	//	newStr = newStr.substr(0, newStr.size()-1);
	//	return newStr;
	//}

	////通过span返回原句的字符串向量
	//string GetVecStr(const string& strIn, const pair<unsigned int, unsigned int>& span)
	//{
	//	vector<string> vRex;
	//	ParseWord(strIn, " ", vRex);
	//	string newStr;
	//	for(int i = span.first; i <= span.second+ span.first; i++)
	//		newStr = newStr + vRex[i] + " ";
	//	newStr = newStr.substr(0, newStr.size()-1);
	//	return newStr;
	//}

private:
	
	unsigned long long GetUyLongNum(const string& str, const NumRecData& regdata)
	{
		unsigned long long num = 0;
		string strIn = str;
		vector<string> tokens;
		ParseWord(strIn, " ", tokens);
		//MatchResult result = regdata.regsub->Match(tokens[0].c_str());
		int id = 0;
		if((int)tokens.size() >= 2)
		{
			num = GetUySubNum( tokens[0] + " " + tokens[1]/*,id*/);
			id = GetNumId(tokens[1]);
			if((int)tokens.size() > 3)
			{
				for(unsigned int i = 2; i < tokens.size(); i++)
				{
					MatchResult result = regdata.regsub->Match(tokens[i].c_str());
					if( result.IsMatched() == true)
					{
						unsigned long long temp;
					
						if( i != tokens.size()-1)
						{
							if( GetNumId(tokens[i+1]) < id)
							{
								id = GetNumId(tokens[i+1]);
								temp = GetUySubNum( tokens[i] + " " + tokens[i+1]);
								num = num + temp;
								i++;
							}
							else
							{
								temp = String2X(tokens[i],temp);
								num = num + temp;
							}
						}
						else
						{
							temp = String2X(tokens[i],temp);
							num = num + temp;
						}
					}
					else
					{
						num = GetUySubNum( X2String(num) + " " + tokens[i]/*, id*/);
					}
				}
			}
		}
		return num;
	}
	int GetNumId(const string& str)
	{
		int id = 0;
		for (map<string, pair<unsigned long long, int> >::iterator itr = mUyNum.begin(); itr != mUyNum.end(); itr++)
		{
			if (str.find(itr->first) != string::npos)
			{
				id = itr->second.second;
				return id;
			}
		}
		return id;
	}
	unsigned long long GetUySubNum(const string& str/* ,int &id*/)
	{
		unsigned long long num = 0;
		stringstream sstream(str);
		string bigdata;
		sstream >> num >> bigdata;
		for (map<string, pair<unsigned long long, int> >::iterator itr = mUyNum.begin(); itr != mUyNum.end(); itr++)
		{
			if (bigdata.find(itr->first) != string::npos)
			{
				/*id = itr->second.second;*/
				return num * itr->second.first;
			}
		}
		return num;
	}

private:

	void Num2Chn(string& strIn, const bool& rec_log)
	{
		string repTemp = strIn;
		for (unsigned int i = 0; i < vNum2ChnRex.size(); i++)
		{
			while(true)
			{
				MatchResult result = vNum2ChnRex[i].regall->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					int intStart = result.GetStart();
					int intEnd = result.GetEnd();
					string got = repTemp.substr(intStart, intEnd-intStart);
					pair<int, int> span = GetSpanByStr(repTemp, got);
								
					unsigned int rep_start = result.GetGroupStart(vNum2ChnRex[i].repnum);  //数字字串
					unsigned int rep_end = result.GetGroupEnd(vNum2ChnRex[i].repnum);
					string rep_str = repTemp.substr(rep_start, rep_end-rep_start);
					//转化成中文汉字
					unsigned long long number = String2X(rep_str, number);
					string strChNum = GetChStrNum(number);
					string temp = got;
					string trans = Replace_all(temp,rep_str,strChNum);
					PostProc(trans,false);
					if (rec_log == true)
						vNumbers = CombLog(span, trans, vNumbers);	
					repTemp = Replace_all(strIn, got, trans);
					
				}
				else
					break;
			}
		}
		for (unsigned int i = 0; i < vBigData2Ch.size(); i++)
		{
			while(true)
			{
				MatchResult result = vBigData2Ch[i].regall->Match(repTemp.c_str());
				if (result.IsMatched() == true)
				{
					int intStart = result.GetStart();
					int intEnd = result.GetEnd();
					string got = repTemp.substr(intStart, intEnd-intStart);
					pair<int, int> span = GetSpanByStr(repTemp, got);
								
					unsigned int rep_start = result.GetGroupStart(vNum2ChnRex[i].repnum);  //数字字串
					unsigned int rep_end = result.GetGroupEnd(vNum2ChnRex[i].repnum);
					string rep_str = repTemp.substr(rep_start, rep_end-rep_start);
					//转化成中文汉字
					unsigned long long number = String2X(rep_str, number);
					string strChNum0 = GetChStrNum0(number);
					string strChNum = GetChStrNum(number);
					
					string temp = got;
					string trans;
					if( strChNum0 != "")
						trans = Replace_all(temp,rep_str,strChNum0);
					else
					{
						if( strChNum < strChNum0 )
							trans = Replace_all(temp,rep_str,strChNum);
						else 
							trans = Replace_all(temp,rep_str,strChNum0);
					}
					if (rec_log == true)
						vNumbers = CombLog(span, trans, vNumbers);					

					repTemp = Replace_all(strIn, got, trans);
					
				}
				else
					break;
			}
		}
		
		/*cout << repTemp << endl;*/
		strIn = repTemp;
	}
	
	//数字转化为汉字形式
	string GetChStrNum(const unsigned long long& num, const bool& bZero = false)
	{
		if (num == 0)
			return "";
		string strChNum;
		for (list<pair<unsigned long long, string> >::reverse_iterator ritr = mChNum.rbegin(); ritr != mChNum.rend(); ritr++)
		{
			unsigned long long quotient = num / ritr->first; // 商
			unsigned long long remaider = num % ritr->first; // 余数
			if (quotient != 0)
			{
				if (quotient == 1 && ritr->first < 10)
				{
					if (bZero == false)
						return ritr->second;
					else
						return mChNum.begin()->second + ritr->second;
				}
				if (bZero == true) // 如果需要加零
					strChNum += mChNum.begin()->second + GetChStrNum(quotient) + ritr->second;
				else
					strChNum += GetChStrNum(quotient) + ritr->second;
				if (remaider == 0)
					return strChNum;
				if (remaider*10 < ritr->first)
					strChNum += GetChStrNum(remaider, true);
				else
					strChNum += GetChStrNum(remaider, false);
				return strChNum;
			}
		}
		return strChNum;
	}
	string GetChStrNum0(const unsigned long long& num, const bool& bZero = false)
	{
		if (num == 0)
			return "";
		string strChNum = "";
		for (list<pair<double, string> >::iterator ritr = Ch.begin(); ritr != Ch.end(); ritr++)
		{
			double quotient = num / ritr->first; // 商
			//double remaider = num % ritr->first; // 余数
			string quo = X2String(quotient);
			string temp = quo + ritr->second;
			if(strChNum == "")
				strChNum = temp;
			else if(temp.size() < strChNum.size())
				strChNum = temp;
			/*if(quo != "0" && quo.find(".") == string::npos )
			{
				strChNum = quo + ritr->second;
			}
			else
			{
				string quoxs = quo.substr(quo.find("."), quo.size()-quo.find("."));
				if(quoxs.size() <= 2)
				{	
					strChNum = quo + ritr->second;
				}
			}*/
			
		}
		return strChNum;		
	}

private:
	
	//span信息等
	/*map<string, NumRecData> mMapLog;*/
	vector<Number> vNumbers;
	//时间词
	vector<CRegexpT<char>* > vMapRegexp;
	vector<pair<string, string> > vRexStr;	
	
	//数词	
	vector<CRegexpT<char>* > vBasicRegexp, vMultipleRegexp, vDecimalRegexp, vOrdinalRegexp, vNumberRegexp, vNumCombRegexp;
	vector<Tripple> vBasicTri, vMultipleTri, vDecimalTri, vOrdinalTri, vNumberTri, vNumCombTri;
	vector<NumRecData> vNumRex, vBigDataRex, vNum2ChnRex, vBigData2Ch;  
	
	list<pair<unsigned long long, string> > mChNum;
	list<pair<double,string> > Ch;
	map<string, pair<unsigned long long,int> > mUyNum;
	/*map<string, string> mMapLog;*/

private:
	void ReadTimeConfig(const string& file)
	{
		LK_IFSTREAM(fin, file);
		string line;
		while (getline(fin, line))
		{
			vector<string> vRex;
			ParseWord(line, "\t", vRex);
			if (line.substr(0,2) == "//")
				continue;
			vRexStr.push_back(make_pair(vRex[0], vRex[1]));
			CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
			vMapRegexp.push_back(pRex);
		}
	}

	//读取数据的Basic配置 
	//[Basic]为基本数字及其词缀识别，保留词缀信息
	//[BigData]针对大位数数字进行处理
	//[Decimal]处理词缀在数词中间的情况：分数、小数
	//[Multiple]倍数和百分数
	//[Ordinal]序数词(inchi|nchi)
	//[Number] 含有X的模板
	void ReadNumberConfig(const string& file)
	{
		LK_IFSTREAM(fin, file);
		string line;
		unsigned int num = 0;
		while (getline(fin, line))
		{
			if (line == "[Basic]")
				num = 1;
			else if (line == "[BigData]")
				num = 2;
			else if (line == "[Decimal]")
				num = 3;
			else if (line == "[Multiple]")
				num = 4;
			else if (line == "[Ordinal]")
				num = 5;
			else if (line == "[Number]")
				num = 6;
			else if (line == "[NumberCombine]")
				num = 7;
			else
			{
				switch (num)
				{
				case 1:	//[Basic]
					{
						vector<string> vRex;
						ParseWord(line, "\t", vRex);
						if (line.substr(0,2) == "//")
							continue;
						Tripple num;
						num.RecRegex = vRex[0];
						num.TransRegex = vRex[1];
						vBasicTri.push_back(num);
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						vBasicRegexp.push_back(pRex);
					}
					break;
				case 2:	//[BigData]
					{
						vector<string> vRex;
						ParseWord(line, "\t", vRex);
						if (line.substr(0,2) == "//")
							continue;
						/*Tripple num;
						num.RecRegex = vRex[0];
						num.TransRegex = vRex[1];
						vBigDataTri.push_back(num);
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						vBigDataRegexp.push_back(pRex);*/
						NumRecData data;
						data.regall = new CRegexpT<char>(vRex[0].c_str());
						data.regsub = new CRegexpT<char>(vRex[1].c_str());
						data.repnum = String2X(vRex[2], data.repnum);
						vNumRex.push_back(data);
					}
					break;
				case 3:  //[Decimal]
					{
						vector<string> vRex;
						ParseWord(line, "\t", vRex);
						if (line.substr(0,2) == "//")
							continue;
						Tripple num;
						num.RecRegex = vRex[0];
						num.TransRegex = vRex[1];
						vDecimalTri.push_back(num);
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						vDecimalRegexp.push_back(pRex);
					}
					break;
				case 4:  //[Multiple]
					{
						vector<string> vRex;
						ParseWord(line, "\t", vRex);
						if (line.substr(0,2) == "//")
							continue;
						Tripple num;
						num.RecRegex = vRex[0];
						num.TransRegex = vRex[1];
						vMultipleTri.push_back(num);
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						vMultipleRegexp.push_back(pRex);
					}
					break;
				case 5:  //[Ordinal]
					{
						vector<string> vRex;
						ParseWord(line, "\t", vRex);
						if (line.substr(0,2) == "//")
							continue;
						Tripple num;
						num.RecRegex = vRex[0];
						num.TransRegex = vRex[1];
						vOrdinalTri.push_back(num);
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						vOrdinalRegexp.push_back(pRex);
					}
					break;
				case 6:  //[Number]
					{
						vector<string> vRex;
						ParseWord(line, "\t", vRex);
						if (line.substr(0,2) == "//")
							continue;
						Tripple num;
						num.RecRegex = vRex[0];
						num.TransRegex = vRex[1];
						vNumberTri.push_back(num);
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						vNumberRegexp.push_back(pRex);
					}
					break;
				case 7:	//[NumberCombine]
					{
						vector<string> vRex;
						ParseWord(line, "\t", vRex);
						if (line.substr(0,2) == "//")
							continue;
						Tripple num;
						num.RecRegex = vRex[0];
						num.d = String2X(vRex[1],num.d);
						vNumCombTri.push_back(num);
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						vNumCombRegexp.push_back(pRex);
					}
					break;
				}
			}
		}
	}

	void ReadNeConfig(const string& file)
	{
		LK_IFSTREAM(fin, file);
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
			else if (line == "[Ch]")
				num = 4;
			else if (line == "[BigData2]")
				num = 5;
			else
			{
				switch (num)
				{
				case 1:
					{
						if (line.substr(0,2) == "//")
							continue;
						stringstream sstream(line);
						string str;
						int minId;
						unsigned long long num = 0;
						sstream >> str >> num >> minId;
						mUyNum[str] = make_pair(num, minId);
						/*mUyNum.insert(make_pair(str, num));*/
					}
					break;
				case 2:
					{
						if (line.substr(0,2) == "//")
							continue;
						stringstream sstream(line);
						string str;
						unsigned long long num = 0;
						sstream >> str >> num;
						mChNum.push_back(make_pair(num, str));
					}
					break;
				case 3:
					{
						if (line.substr(0,2) == "//")
							continue;
						vector<string> vRexData;
						ParseWord(line, "\t", vRexData);
						NumRecData data;
						data.regall = new CRegexpT<char>(vRexData[0].c_str());
						data.regsub = new CRegexpT<char>(vRexData[1].c_str());
						data.repnum = String2X(vRexData[2], data.repnum);
						vNum2ChnRex.push_back(data);
					}
					break;
				case 4:
					{
						if (line.substr(0,2) == "//")
							continue;
						stringstream sstream(line);
						string str;
						double num = 0;
						sstream >> num >> str;
						Ch.push_back(make_pair(num, str));
					}
					break;
				case 5:
					{
						if (line.substr(0,2) == "//")
							continue;
						vector<string> vRexData;
						ParseWord(line, "\t", vRexData);
						NumRecData data;
						data.regall = new CRegexpT<char>(vRexData[0].c_str());
						data.regsub = new CRegexpT<char>(vRexData[1].c_str());
						data.repnum = String2X(vRexData[2], data.repnum);
						vBigData2Ch.push_back(data);
					}
					break;
				}				
			}
		}
		mChNum.sort();
		Ch.sort();
	}
};
