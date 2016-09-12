#include "splitutf.h"


using namespace TiNumber;

void splitutf::Initialize(const string &path)
{
	string p = path;
	readFile(p+"/utf/tisegutf",this->tiseg);
	readFile(p+"/utf/chsegutf",this->chseg);
	readFile(p+"/utf/numutf",this->numbzd);
	readFile(p+"/utf/semisegutf",this->semiseg);
	readFile(p+"/utf/semisegsgutf",this->semisegsg);
	readFile(p+"/utf/numinerutf",this->numiner);
	readFile(p+"/utf/numfirst",this->numfirst);
}

splitutf::splitutf(void)
{
	
	//readFile("bzd/dict",this->dict);
	char dotchar[4]={0xe0,0xbc,0x8b,0};
	dot = string(dotchar);
	char spchar1[4]={0xe0,0xbc,0x8d,0};
	separator1 = string(spchar1);
	char spchar2[4]={0xe0,0xbc,0x8e,0};
	separator2 = string(spchar2);
	char sfchar[4]={0xe0,0xbd,0xa0,0};
	suffix = string(sfchar);
	//en_set.
	en_set.insert("{");
	en_set.insert("}");
	en_set.insert("[");
	en_set.insert("]");
	en_set.insert("(");
	en_set.insert(")");

	
}

splitutf::~splitutf(void)
{
}

void splitutf::readFile(const string & file,set<string> &s)
{
	string line;
	ifstream fin(file.c_str());
    if(!fin)
    {
        cerr<<"can not open file "<<file<<endl;
        exit(1);
    }
	while(getline(fin,line))
		s.insert(line);
	fin.close();
}

void splitutf::processSym(string &s,vector<pair<string,bool> > &block,bool isEnd)//isEnd=false,应该在s添加一个音节点；
{
	//isEnd = true;
	if(this->tiseg.find(s)!=this->tiseg.end())
	{
		if(isEnd)
			block.push_back(make_pair(s,true));
		else
			block.push_back(make_pair(s+dot,true));
	}
	else
	{
		int len = s.size();
		if(len>6)//是不是出现这样现象就一定得切分???????
		{
			string tmp = s.substr(len-6);
			if(this->semiseg.find(tmp)!=this->semiseg.end())
			{
				string rest = s.substr(0,len-6);
				//if(dict.find(rest)!=dict.end())
				block.push_back(make_pair(rest,false));
				if(!isEnd)
					tmp +=dot;
				block.push_back(make_pair(tmp,false));

				return;

			}
			if(this->semisegsg.find(tmp)!=this->semisegsg.end())
			{
				string rest = s.substr(0,len-6);
				block.push_back(make_pair(rest,false));
				
				if(!isEnd)
					tmp +=dot;
				block.push_back(make_pair(tmp,false));

				return;
			}
			

		}
		
		if(len >3)
		{
			string tmp = s.substr(len-3);
			if(this->semisegsg.find(tmp)!=this->semisegsg.end())//???是不是出现这样现象就一定得切分
			{
				string rest = s.substr(0,len-3);
				//bool isseg=false;

				/*if(dict.find(rest+suffix)!=dict.end())
				{
					isseg = true;
					block.push_back(make_pair(rest+suffix,isseg));
				}
				else*/

				block.push_back(make_pair(rest,false));
				
				if(!isEnd)
					tmp +=dot;
				block.push_back(make_pair(tmp,false));
			}
			else
			{

				if(isEnd)
					block.push_back(make_pair(s,false));
				else
					block.push_back(make_pair(s+dot,false));


			}
		}
		else
		{

			if(isEnd)
				block.push_back(make_pair(s,false));
			else
				block.push_back(make_pair(s+dot,false));
		}
	}//else
}

void splitutf::preSplit(const string &s,vector<pair<string,bool> > &block)
{
	int i;
	block.clear();

	stringstream ss;
	ss<<s;
	string linechar;
	while(ss >> linechar)
	{
		Utf8Stream utfStream(linechar);
		string u8char;
		vector<string> line;//store utf8 characters
		while(utfStream.ReadOneCharacter(u8char))
		{
			line.push_back(u8char);
		}
		int len = line.size();
		string sym;
		bool isLastArabNum=false;
		for(i=0;i<len;)
		{
			//cout<<i<<endl;
			if(line[i].size()==1 && en_set.find(line[i])==en_set.end())//如果为一个字节的
			{
				string bk;

				bool isArabNum = false;
				if(numfirst.find(line[i])!=numfirst.end())
					isArabNum = true;

				if(isArabNum)
				{
					while(i<len &&line[i].size()==1 && en_set.find(line[i])==en_set.end())//english characters and without {}()
					{
						if(numfirst.find(line[i])==numfirst.end())
							isArabNum = false;
						bk += line[i++];
					}//while
				}
				else
				{
					while(i<len &&line[i].size()==1 && en_set.find(line[i])==en_set.end())//english characters and without {}()
					{
						bk += line[i++];
					}//while
				}

				if(sym.size()!=0)
				{
					processSym(sym,block,true);
					sym = "";
				}
				string tmp = bk;

				if(isLastArabNum)
				{
					if(isArabNum)
					{
						//isLastArabNum = true;
						if(block.empty())
							block.push_back(make_pair(tmp,true));
						else
						{
							vector<pair<string,bool> >::reference rf = block.back();
							rf.first +=tmp;
						}
					}
					else
					{
						isLastArabNum = false;
						block.push_back(make_pair(tmp,true));
					}
				}
				else
				{
					if(isArabNum)
					{
						isLastArabNum = true;
						
						block.push_back(make_pair(tmp,true));
						
					}
					else
					{
						
						block.push_back(make_pair(tmp,true));
					}
				}
				
				
			}//if(line[i].size()==1 && en_set.find(line[i])==en_set.end())
			else if(line[i].size() ==1 && en_set.find(line[i])!=en_set.end())//appear {}()
			{
				isLastArabNum = false;
				if(sym.size()!=0)
				{
					processSym(sym,block,true);
					sym = "";
				}
				
				block.push_back(make_pair(line[i],true));
				++i;
			}
			else//如果为多字节的
			{
				//cout<<i<<endl;
				bool lastIsNum=false;
				while(i<len && line[i].size()>1)
				{
					//cout<<i<<endl;
					string tmp = line[i];
					++i;
					sym +=tmp;

					if(this->numbzd.find(tmp)!=this->numbzd.end())//如果出现藏文的数字，当做一个块。藏文数字的后面有没有点号？
					{
						isLastArabNum = false;
						int symLen = sym.size();
						if(symLen>3)
						{
							sym = sym.substr(0,symLen-3);
							processSym(sym,block,true);
						}
						if(lastIsNum)
						{
							if(!block.empty())
							{
								vector<pair<string,bool> >::reference rf = block.back();
								rf.first +=tmp;
							}
							else
							{
								block.push_back(make_pair(tmp,true));
							}
						}
						else
							block.push_back(make_pair(tmp,true));
						lastIsNum = true;
						sym = "";
					}
					else if(this->numiner.find(tmp)!=this->numiner.end())//如果出现出现数字块1.2% １．２％
					{
						if(isLastArabNum && !block.empty())
						{
							vector<pair<string,bool> >::reference rf = block.back();
							rf.first +=tmp;
						}
						else
						{
							isLastArabNum = true;
							if(sym.size()==3)
								block.push_back(make_pair(sym,true));
							else
							{
								sym = sym.substr(0,sym.size()-3);
								processSym(sym,block,true);
								block.push_back(make_pair(tmp,true));
							}
						}

						sym = "";
					}
					else if(this->chseg.find(tmp)!=this->chseg.end())//如果有汉字的标点符号，或者汉字的数字标号
					{
						isLastArabNum = false;
						lastIsNum = false;
						int symLen = sym.size();
						if(symLen==3)
							block.push_back(make_pair(sym,true));
						else
						{
							sym = sym.substr(0,symLen-3);
							processSym(sym,block,true);
							block.push_back(make_pair(tmp,true));
						}
						
						sym = "";
					}
					else if(tmp==dot)//遇到音节点
					{
						//
						isLastArabNum = false;
						if(lastIsNum)
						{
							vector<pair<string,bool> >::reference rf = block.back();
							rf.first +=dot;
						}
						else
						{
							
							int symLen = sym.size();
							if(symLen==3)
							{
								block.push_back(make_pair(sym,true));
							}
							else
							{
								sym = sym.substr(0,symLen-3);
								processSym(sym,block,false);
							}
						}

						lastIsNum = false;
						sym = "";
					}
					else if(tmp == separator1 || tmp ==separator2)
					{
						//
						isLastArabNum = false;
						lastIsNum = false;
						int symLen = sym.size();
						
						if(symLen>3)
						{
							sym = sym.substr(0,symLen-3);
							processSym(sym,block,true);
						}
						if(!block.empty())
						{
							vector<pair<string,bool> >::reference rf = block.back();
							if(rf.first!=separator1 && rf.first!=separator2)
								block.push_back(make_pair(tmp,true));
							else
								rf.first +=tmp;
						}
						else
							block.push_back(make_pair(tmp,true));

						sym = "";
					}
					else if(i>=len)//最后不以音节点或者竖杠结束
					{
						//
						isLastArabNum = false;
						lastIsNum = false;
						processSym(sym,block,true);

					}
					else
					{
						isLastArabNum = false;
						lastIsNum = false;
					}

				}//while
			}//else
			/*if(line[i]<0 && i+1==len)//如果最后一个字符是小于0的，只能这样做
			{
				string tmp2;
				tmp2[0] = line[i];
				block.push_back(make_pair(tmp2,true));
				break;
			}*/

		}//for

	}//while

	
}
