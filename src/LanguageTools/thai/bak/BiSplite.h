#pragma once
#include "splitutf.h"
#include "utility.h"

using namespace std;
namespace thaiSeg {

class BiSplite
{
public:
	void Initialize(const string& path)
	{
		//m_tiSplit.Initialize("/home3/jwb/sunmeng/cplus/bisplit");
		m_tiSplit.Initialize(path);
	}
	string TiSplit(string &src)
	{
		vector<pair<string,bool> > block;
		m_tiSplit.preSplit(src,block);
		string rst;
		for (int i=0;i<(int)block.size();++i)
		{
			rst +=block[i].first + " ";
		}
		rst = rst.substr(0,rst.size()-1);
		return rst;
	}
	string MoSplit(string &src)
	{
		stringstream ss(src);
		string part;
		string rst;
		while(ss>>part)
		{
			Utf8Stream u8(part);
			string mo;
			while(u8.ReadOneCharacter(mo))
			{
				//cout<<mo<<endl;
				rst.append(" "+mo);
			}
		}
		return trim(rst);
	}
	string ChSplit(string &src)
	{
		stringstream ss(src);
		string part;
		string rst;
		while (ss >>part)
		{
			Utf8Stream u8(part);
			string ch;
			string inRst;
			int lastState = -1;//0指藏文,1指汉文，2英文不予以切分
			while (u8.ReadOneCharacter(ch))
			{
				if(ch.size()==1)//半角字符
				{
					if(lastState !=2 )
						rst +=" ";
					rst +=ch;
					lastState = 2;
				}
				else if(isChineseUtf8(ch))//中文
				{

					rst += " "+ch;
					lastState = 1;
				}
				else
				{
					if(lastState !=0 )
						rst +=" ";
					rst +=ch;
					lastState = 0;
				}
			}
		}
		rst = trim(rst);

		return rst;
	}

	BiSplite(void);
	~BiSplite(void);

private:
	splitutf m_tiSplit;
};

}