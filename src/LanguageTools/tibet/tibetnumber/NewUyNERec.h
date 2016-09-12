#pragma once
#undef max
#undef min
#include <string>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <list>
#include "../../deelx.h"
#include "../../Tools2.h"
//#include "TiNum.h"
#include "utility.h"

using namespace std;
using namespace UtilityTool;

namespace TiNumber
{



class transPair
{
public:
	string ti;
	string ch;
	int beg;//span [beg,end]
	int end;

};

class NewUyNERec
{
private:
	struct NumRecData
	{
		CRegexpT<char>* regall;
		CRegexpT<char>* regsub;
		unsigned short repnum;
	};
public:
	NewUyNERec(const string& ne_config, const string& num_config)
	{
		ReadNeConfig(ne_config);
		//ReadNumConfig(num_config);
	}
	NewUyNERec()
	{

	}
	//NewUyNERec(const string& num_config)//new
	//{
	//	ReadNeConfig(num_config);
	//	//ReadNumConfig(num_config);
	//}
	~NewUyNERec()
	{
		vector<CRegexpT<char>* >::iterator it;
		for (it=vMapRegexp.begin();it!=vMapRegexp.end();++it)
		{
			delete *it;
		}

		vector<pair<CRegexpT<char>*,string> >::iterator its;
		for (its=extraReg.begin();its!=extraReg.end();++its)
		{
			delete its->first;
		}
		//vMapRegexp
	}
	void initialize(const string& path)
	{
		ReadNeConfig(path+"/data/Rex.txt");
		ReadNumConfig(path+"/data/KaNeNum.cfg");
		char negStr[4] = {0xe8,0xb4,0x9f,0};
		neg = negStr;
		char dotStr[4] = {0xe7,0x82,0xb9,0};
		dot = dotStr;

	}
public:
	//第二步
	string findRplc(string& strIn,vector<transPair> &tps,vector<transPair> &ctps,set<int> &cover)//strIn已经是阿拉伯的数字了，现在是要用正则将其周围的藏文搭配翻译成中文
	{
		int count  = 0;
		string repTemp = strIn;
		for(size_t i=0;i<extraReg.size();++i)
		{
			MatchResult rst = extraReg[i].first->Match(repTemp.c_str());
			while (rst.IsMatched())
			{
				string tgtstr;
				string got = repTemp.substr(rst.GetStart(), rst.GetEnd()-rst.GetStart());
				bool isTps = rst.GetGroupStart(1)<0 ? false:true;
				string num = repTemp.substr(rst.GetGroupStart(2),rst.GetGroupEnd(2)-rst.GetGroupStart(2));


				stringstream ss;
				ss<<num;
				float bscNum;
				ss >>bscNum;
				ss.clear();
				ss.str("");
				if (bscNum!=0)
				{
					bscNum *=10;
					ss<<bscNum;
					string rpm = ss.str() + extraReg[i].second ;

					transPair tp;
					if(isTps)
					{
						string index = repTemp.substr(rst.GetGroupStart(1)+1,rst.GetGroupEnd(1) -2 - rst.GetGroupStart(1) );
						string tpsStr = repTemp.substr(rst.GetGroupStart(1),rst.GetGroupEnd(2) - rst.GetGroupStart(1) );

						ss.clear();
						ss.str("");
						ss<<index;
						int i;
						ss>>i;
						cover.insert(i);
						//tp.ti =
						replace(got,tpsStr,tps[i].ti,tp.ti);
					}
					else
						tp.ti = got;
					tp.ch = rpm;
					tp.beg = strIn.find(got);
					tp.end = tp.beg + got.size() -1;
					ctps.push_back(tp);


					tgtstr = "<"+Int2Str(count)+","+tp.ch+">";
					++count;
					repTemp =repTemp.substr(0,rst.GetStart())+tgtstr + repTemp.substr(rst.GetEnd());

					//replace(repTemp,got,rpm);

				}

				rst = extraReg[i].first->Match(repTemp.c_str(),tgtstr.size()+rst.GetStart());


			}
		}
		
		for (unsigned int i = 0; i < vMapRegexp.size(); i++)
		{
			//cout<<i<<endl;
			MatchResult result = vMapRegexp[i]->Match(repTemp.c_str());//如果是多个呢？
			
			while(result.IsMatched() )
			{
				//找到源端可以匹配到的片段
				transPair tp;
				string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
				tp.ti = got;
				int maxGp = result.MaxGroupNumber();//获得正则表达式参数个数
				//cout<<i<<":"<<maxGp<<endl;
				bool isTiNum = false;
				for(int j=1;j<=maxGp;++j)
				{
					if (result.GetGroupStart(j)>=0)
					{
						/*string index = repTemp.substr(result.GetGroupStart(j)+1,result.GetGroupEnd(j) -2 - result.GetGroupStart(j) );
						string tpsStr = repTemp.substr(result.GetGroupStart(j),result.GetGroupEnd(j+1) - result.GetGroupStart(j) );
						int indTps = Scan<int>(index);
						replace(tp.ti,tpsStr,tps[indTps].ti);
						cover.insert(indTps);*/
						string hynum = repTemp.substr(result.GetGroupStart(j),result.GetGroupEnd(j) - result.GetGroupStart(j));
						if (hynum[0] == '&')
						{
							isTiNum = true;
							string numstr = hynum.substr(1,hynum.size()-2);//获得在基本数词列表中的索引
							string tpsStr = repTemp.substr(result.GetGroupStart(j),result.GetGroupEnd(j+1) - result.GetGroupStart(j) );//tpsStr是&3&2000这样的字符串
							int indTps = Scan<int>(numstr);
							replace(tp.ti,tpsStr,tps[indTps].ti);//使得<src,tgt>，src是原生态的藏文字符
							cover.insert(indTps);
							++j;
						}


					}
				}
				
				char *tgtTmp = vMapRegexp[i]->Replace(got.c_str(), vRexStr[i].second.c_str(),0,1);
				
				
				tp.ch = tgtTmp;
				if(!DebugMode)
				{
					if (isTiNum)
					{
						CRegexpT<char> rexNum("-?\\d+\\.?\\d*");
						MatchResult mr = rexNum.Match(tp.ch.c_str());
						string tmpstr;
						int last = 0;
						while (mr.IsMatched())
						{
							string numStr = tp.ch.substr(mr.GetStart(),mr.GetEnd() - mr.GetStart());
							float x = Scan<float>(numStr);
							if(isInt(x) && x >0 && x<=10 && cvtCh[i]==0)
							{
								tmpstr +=tp.ch.substr(last,mr.GetStart() - last);
								tmpstr +=mBasicNum[numStr];
								last = mr.GetEnd();

							}
							else if (isInt(x) && x >0 &&(cvtCh[i] ==1|| x>10000))
							{
								tmpstr +=tp.ch.substr(last,mr.GetStart() - last);
								if(x>=10 && x<20)
									tmpstr +=GetChStrNum20((unsigned int)x);
								else
									tmpstr +=GetChStrNum((unsigned int)x,false);
								last = mr.GetEnd();
							}
							
							

							mr = rexNum.Match(tp.ch.c_str(),mr.GetEnd());
						}

						if (last !=0 && last !=tp.ch.size())
						{
							tmpstr +=tp.ch.substr(last);
						}
						

						if(!tmpstr.empty())
							tp.ch = tmpstr;
						
					}

				}
				
				
				vMapRegexp[i]->ReleaseString(tgtTmp);

				tp.beg = strIn.find(got);
				tp.end = tp.beg + got.size() -1;
				ctps.push_back(tp);

				string tgtstr = "<"+Int2Str(count)+","+tp.ch+">";
				++count;
				repTemp =repTemp.substr(0,result.GetStart())+tgtstr + repTemp.substr(result.GetEnd());
				
				/*char *tmp = vMapRegexp[i]->Replace(repTemp.c_str(), vRexStr[i].second.c_str());
				repTemp = tmp;
				vMapRegexp[i]->ReleaseString(tmp);*/
				result = vMapRegexp[i]->Match(repTemp.c_str(),tgtstr.size()+result.GetStart());
			}
		}
		return repTemp;
	}

	string NeRec(const string& strIn, const bool& rec_log = true)
	{
		string strTemp = strIn;
		//bool replace_flag = PreProc(strTemp);
		RecRex(strTemp, rec_log);
		//RecNum(strTemp, rec_log);
		//PostProc(strTemp, replace_flag);
		return strTemp;
	}
	void output_log(const string& file)
	{
		ofstream fout(file.c_str());
		for (map<string, string>::iterator itr = mMapLog.begin(); itr != mMapLog.end(); itr++)
		{
			fout << itr->first << "\t" <<  NeRec(itr->first, false) << endl;//这个只是对识别出来的片段替换
		}
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
	}
	void RecRex(string& strIn, const bool& rec_log)
	{
		string repTemp = strIn;
		for (unsigned int i = 0; i < vMapRegexp.size(); i++)
		{
			MatchResult result = vMapRegexp[i]->Match(repTemp.c_str());
			
			if (result.IsMatched() == true)
			{
				//找到源端可以匹配到的片段
				string got = repTemp.substr(result.GetStart(), result.GetEnd()-result.GetStart());
				if (rec_log == true)
					mMapLog.insert(make_pair(got, ""));
				repTemp = vMapRegexp[i]->Replace(repTemp.c_str(), vRexStr[i].second.c_str());
			}
		}
		strIn = repTemp;
	}
private:
	void RecNum(string& strIn, const bool& rec_log)
	{
		for (unsigned int i = 0; i < vNumRex.size(); i++)
		{
			RecNum(strIn, vNumRex[i], rec_log);
		}
	}
	void RecNum(string& strIn, const NumRecData& regdata, const bool& rec_log)
	{
		while(true)
		{
			MatchResult result = regdata.regall->Match(strIn.c_str());
			if (result.IsMatched() == true)
			{
				int intStart = result.GetStart();
				int intEnd = result.GetEnd();
				string strUyNum = strIn.substr(intStart, intEnd-intStart);
				if (rec_log == true)
					mMapLog.insert(make_pair(" "+strUyNum, ""));
				unsigned long long longNum = GetUyLongNum(strUyNum, regdata);
				string strChNum = GetChStrNum(longNum);
				stringstream sstream;
				unsigned int rep_start = result.GetGroupStart(regdata.repnum);
				unsigned int rep_end = result.GetGroupEnd(regdata.repnum);
				string rep_str = strIn.substr(rep_start, rep_end-rep_start);
				strIn = Tools::StrTool::replace_all(strIn, rep_str, strChNum, rep_start, rep_end);
			}
			else
				break;
		}
	}
	unsigned long long GetUyLongNum(const string& str, const NumRecData& regdata)
	{
		unsigned long long num = 0;
		int findStart = -1;
		MatchResult result = regdata.regsub->Match(str.c_str(), findStart);
		while (result.IsMatched() == true)
		{
			int intStart = result.GetStart();
			int intEnd = result.GetEnd();
			string strUyNumSub = str.substr(intStart, intEnd-intStart);
			num += GetUySubNum(strUyNumSub);
			result = regdata.regsub->Match(str.c_str(), intEnd);
		}
		return num;
	}
	unsigned long long GetUySubNum(const string& str)
	{
		float num = 0;
		stringstream sstream(str);
		sstream >> num;
		for (map<string, unsigned long long>::iterator itr = mUyNum.begin(); itr != mUyNum.end(); itr++)
		{
			string::size_type pos = str.find(itr->first);
			if (pos != string::npos)
				return num * itr->second;
		}
		return num;
	}
	
	string GetChStrFromArabicStr(const string &arabicStr)//普通整数
	{
		if(arabicStr.empty())
			return "";
		size_t ind=0;
		string unsgnStr,chstr="";
		if(arabicStr[0]=='-')
			unsgnStr = arabicStr.substr(1);
		else
			unsgnStr = arabicStr;
		
		if((ind = unsgnStr.find("."))!=string::npos)
		{
			chstr +=dot;
			for (size_t i= ind +1;i<unsgnStr.size();++i)
			{
				chstr +=mBasicNum[unsgnStr.substr(i,1)];
			}
			unsgnStr = unsgnStr.substr(0,ind);	
		}

		unsigned long intNum = Scan<unsigned long>(unsgnStr);
		string intPart = GetChStrNum(intNum,true);
		if(arabicStr[0] == '-')
			intPart = neg + intPart;

		
		
		return intPart+chstr;//整数+小数

	}
	string GetChstrNoFromArabic(const string &arabicStr)//不能输入负数；没有量词，譬如，一九八八
	{
		string chStr;
		for (size_t i=0;i<arabicStr.size();++i)
		{
			chStr +=mBasicNum[arabicStr.substr(i,1)];
		}

		return chStr;
		
	}
	string GetChStrNum20(const unsigned int &num)//只能转10-19之间
	{
		int remainder = num %10;
		string ten = Int2Str(10);
		if(remainder >0)
		{
			string single = Int2Str(remainder);
			return mBasicNum[ten]+mBasicNum[single];
		}
		else
			return mBasicNum[ten];
		
	}
	string GetChStrNum(const unsigned long long& num, const bool& bZero = false)//由阿拉伯数字转换为汉字的数字（只能处理整数）true
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

private:
	vector<CRegexpT<char>* > vMapRegexp;
	vector<int > cvtCh;
	vector<pair<CRegexpT<char>*,string > > extraReg;
	vector<pair<string, string> > vRexStr;
	vector<NumRecData> vNumRex;
	map<string, unsigned long long> mUyNum;
	list<pair<unsigned long long, string> > mChNum;
	map<string, string> mMapLog;
	string neg;
	string dot;
	map<string,string> mBasicNum;

private:
	void ReadNeConfig(const string& file)
	{
		LK_IFSTREAM(fin, file);
		string line;
		int num = 0;
		//int ii=0;
		while (getline(fin, line))
		{
			if(line.empty())
				continue;
			//cout<<++ii<<endl;
			if(line =="[TiNum]")
				num = 1;
			else if(line =="[extra]")
				num = 2;
			else
			{
				switch(num)
				{
				case 1:
					{
						vector<string> vRex;
						Tools::StrTool::parseWord(line, "\t", vRex);
						if (line.substr(0,2) == "//")
							continue;
						vRexStr.push_back(make_pair(vRex[0], vRex[1]));
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						vMapRegexp.push_back(pRex);	
						if(vRex.size()==3)
							cvtCh.push_back(Scan<int>(vRex[2]));//1一定要转换为汉语，2一定不能转换为汉语
						else
							cvtCh.push_back(0);//0 ，十以内的转换为汉语
					}
					break;
				case 2:
					{
						vector<string> vRex;
						Tools::StrTool::parseWord(line, "\t", vRex);
						if (line.substr(0,2) == "//")
							continue;
						CRegexpT<char>* pRex = new CRegexpT<char>(vRex[0].c_str());
						extraReg.push_back(make_pair(pRex,vRex[1]));
					}
					break;
				}
			}
			
		}
	}
	void ReadNumConfig(const string& file)
	{
		LK_IFSTREAM(fin, file);
		string line;
		unsigned int num = 0;
		while (getline(fin, line))
		{
			if (line == "[UyNum]")
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
						stringstream sstream(line);
						string str;
						unsigned long long num = 0;
						sstream >> str >> num;
						mUyNum.insert(make_pair(str, num));
					}
					break;
				case 2:
					{
						stringstream sstream(line);
						string str;
						unsigned long long num = 0;
						sstream >> str >> num;
						mChNum.push_back(make_pair(num, str));
						stringstream ss(line);
						string numStr,chStr;
						ss >>chStr>>numStr;
						mBasicNum[numStr] = chStr;
					}
					break;
				case 3:
					{
						vector<string> vRexData;
						Tools::StrTool::parseWord(line, "\t", vRexData);
						NumRecData data;
						data.regall = new CRegexpT<char>(vRexData[0].c_str());
						data.regsub = new CRegexpT<char>(vRexData[1].c_str());
						data.repnum = Tools::StrTool::string2X(vRexData[2], data.repnum);
						vNumRex.push_back(data);
					}
					break;
				}
			}
		}
		mChNum.sort();
	}
};

}