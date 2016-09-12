#include "TiNum.h"
#include "../../../Common/Utf8Stream.h"


namespace UtilityTool
{
	bool DebugMode = false;
	int senti = 0;
}
namespace TiNumber
{

//ofstream flog("log",iostream::app);

TiNum::TiNum(void)
{
}
bool TiNum::initialize(const string &path)
{
	string pathRule = path +"/data/rule.txt";
	ifstream in(pathRule.c_str());
	if (!in)
	{
		cerr<<"can not find file "<<path<<endl;
		return false;
	}

	spf.Initialize(path);//
	char str[4]={0xe0,0xbc,0x8b,0};

	sdot = string(str);
	string line;
	char sepstr[]={0xe0,0xbc,0x8d,0};
	sep = string(sepstr);

	rex.initialize(path);//正则表达的引擎Rex.txt

	maxSlm = 8;

	int cls = 10000;
	int i = 0;
	while (getline(in,line))
	{
		//cout<<++i<<endl;
		if (line.empty())
		{
			continue;
		}
		if(line == "[sl]")
			cls = 0;
		else if(line == "[js]")
			cls = 1;
		else if(line == "[pre]")
			cls = 2;
		else if(line == "[dot]")
			cls = 3;
		else if(line =="[linker]")
			cls = 4;
		else if(line =="[noNum]")
			cls = 5;
		else if(line == "[suffix]")
			cls = 6;
		else if (line == "[dictionary]")
			cls = 7;
		else
		{
			switch (cls)
			{
			case 0:
				{
					vector<string> tokens= splitByStr(line,"\t");
					sl[tokens[0]] = chArb(tokens[1],Scan<int>(tokens[2]));

					dicNum.insert(tokens[0]);
	
					if(tokens[0].substr(tokens[0].size()-3)==sdot)
					{
						sl[tokens[0].substr(0,tokens[0].size()-3)] = sl[tokens[0]];
						dicNum.insert(tokens[0].substr(0,tokens[0].size()-3));
					}


				}
				break;
			case 1:
				{
					vector<string> tokens= splitByStr(line,"\t");
					
					dicNum.insert(tokens[0]);
					if (i<=12)
					{
						bsc[tokens[0]] = Scan<int>(tokens[1]);
						++i;
					}
					else
					{
						js[tokens[0]] = Scan<int>(tokens[1]);
						if(tokens[0].substr(tokens[0].size()-3)==sdot)
						{
							js[tokens[0].substr(0,tokens[0].size()-3)] = js[tokens[0]];
							dicNum.insert(tokens[0].substr(0,tokens[0].size()-3));
						}
					}

					
				}
				break;
			case 2:
				{
					vector<string> tokens= splitByStr(line,"\t");
					pre[tokens[0]] = Scan<int>(tokens[1]);
					dicNum.insert(tokens[0]);

					if(tokens[0].substr(tokens[0].size()-3)==sdot)
					{
						pre[tokens[0].substr(0,tokens[0].size()-3)] = pre[tokens[0]];
						dicNum.insert(tokens[0].substr(0,tokens[0].size()-3));
					}
				}
				break;
			case 3:
				{
					dot.insert(line);
					dicNum.insert(line);

					if(line.size()>=3 && line.substr(line.size()-3)==sdot)
					{
						dot.insert(line.substr(0,line.size()-3));
						dicNum.insert(line.substr(0,line.size()-3));
					}
				}
				break;
			case 4:
				{
					linker.insert(line);
					dicNum.insert(line);

					if(line.size()>=3 && line.substr(line.size()-3)==sdot)
					{
						linker.insert(line.substr(0,line.size()-3));
						dicNum.insert(line.substr(0,line.size()-3));
					}
				}
				break;
			case 5:
				{
					noNum.insert(line);
					dicNum.insert(line);

					if(line.size()>=3 && line.substr(line.size()-3)==sdot)
					{
						noNum.insert(line.substr(0,line.size()-3));
						dicNum.insert(line.substr(0,line.size()-3));
					}
				}
				break;
			case 6:
				{
					suffix.insert(line);
					dicNum.insert(line);

					if(line.size()>=3 && line.substr(line.size()-3)==sdot)
					{
						suffix.insert(line.substr(0,line.size()-3));
						dicNum.insert(line.substr(0,line.size()-3));
					}

				}
				break;
			case 7:
				{
					dicTerm.insert(line);
					if(line.size()>=3 && line.substr(line.size()-3)==sdot)
					{
						dicTerm.insert(line.substr(0,line.size()-3));
					}
				}
				break;
			}
		}

	}

	
	in.close();

	return true;
}

//src 原句；tgt 替换之后的句子；tps 基本数词；ctps 复杂数词，包含部分基本数词； pseudoSen token之后的句子，可能有微小改变;medStr将源句子中出现的基本数词都替换成阿拉伯数字
void TiNum::process(const string &src,string &tgt,vector<transPair> &tps,vector<transPair> &ctps,string &pseudoSen,string &medSen)
{
	//tgt = src;
	//basicRec(tgt);
	string line = MYtrim(src);
	replace(line,"<","#left#");
	replace(line,">","#right#");
	if(line.empty())
		return;
	vector<string> sen;
	tokenSen(line,sen,tps,pseudoSen);

	//vector<transPair> tps;

	numRec(sen,tps);

	sort(tps.begin(),tps.end(),scorecmp());

	//额外的复杂正则表达式
	//extraRex(pseudoSen,tps);//对应readme里面的第一条规则

	composedNum(pseudoSen,medSen,tgt,tps,ctps);

	replace(tgt,"#left#","<");
	replace(tgt,"#right#",">");

	//tgt = pseudoSen;
	//numRec(sen);
	//ordinalTimeRec(tgt);

}
void TiNum::extraRex(string &tgt,vector<transPair> &ctps)//可能会出现直接的数字
{
	/*vector<transPair>::iterator it;
	int beg = 0,end = 0;
	while ((beg = pseudoSen.find(extraStr,end))!=string::npos)
	{
		for (it = tps.begin();it!=tps.end();++it)
		{

		}
	}*/
	//rex.
	
}
void TiNum::composedNum(string &pseudoSen,string &medStr,string &tgt,vector<transPair> &tps,vector<transPair> &ctps)
{
	int p=0;//是pseudoSen指针
	int t=0;//tgt的指针
	//string tgt;
	//先在句子中替换基本数词，形式如下：
	//XXXXXXXXX&3&2000XXXXX
	//3表示在tps中排第三，将对应的藏文字符替换为2000
	//这是为了与原句子中出现的阿拉伯数字区分
	for (int i=0;i<tps.size();++i)//此时，tps中元素的指针应该指向tgt的位置//这个有问题
	{
		int beg = tps[i].beg;
		int end = tps[i].end;
		
		medStr +=pseudoSen.substr(p,beg-p);
		//t +=beg -p;
		//tps[i].beg = t;
		stringstream ss;
		ss<<i;
		medStr +="&"+ss.str()+"&"+tps[i].ch;//为了最后的替换
		//t +=tps[i].ch.size()+ss.str().size()+1;
		//tps[i].end = t;

		p=end +1;
		//++t;
	}
	if(p<pseudoSen.size())
		medStr +=pseudoSen.substr(p);

	/*ofstream log("log");
	log<<medStr<<endl;
	log.close();*/


	//extraRex(tgt,ctps);
	set<int > cover;//cover记录的是用在复杂规则中的基本数词的索引
	tgt = rex.findRplc(medStr,tps,ctps,cover);
	//如果满足复杂规则，则会变成<5,200斤>
	//5表示在ctps中排第5个
	//不满足复杂规则的，变成<7,2000>,也同样放在ctps中，但是不会记录在cover
	//

	vector<transPair> tmpp;

	for (int i=0;i<tps.size();++i)//处理未在复杂数字词中的基本数词
	{
		if(cover.find(i)==cover.end())
		{
			tmpp.push_back(tps[i]);
			ctps.push_back(tps[i]);
		}
	}

	/*
	int beg =0,end = 0;

	vector<transPair>::iterator it= tmpp.begin(), last = it;
	while ((beg = tgt.find("<",end))!=string::npos)
	{
		int addLen=0;
		if(beg == end)//
		{
			end = tgt.find(">",beg+1);
			if(end==string::npos)
				break;
			else
			{
				int cma = tgt.find(",",beg+1);
				if(cma == string::npos)
					break;
				if(cma>end)
				{
					++end;
					continue;
				}

				string numstr = tgt.substr(beg+1,cma-beg-1);
				int index = Scan<int>(numstr);
				tgt = tgt.substr(0,beg+1) + ctps[index].ti+"," +ctps[index].ch+tgt.substr(end);
				end +=ctps[index].ti.size() - numstr.size();
				++end;
			}
		}
		else
		{
			string mid = tgt.substr(end,beg-end);//mid指两对尖括号之间的<,>xxxxx<,>,即xxxxx
			int pos=0,lastpos = 0;
			for (it = last;it!=tmpp.end();++it)
			{
				
				if ((pos = mid.find(it->ti,lastpos))!=string::npos)
				{
					lastpos =it->ti.size()+pos;
					mid = mid.substr(0,pos) + "<"+it->ti+","+it->ch+">"+mid.substr(lastpos);
					addLen +=3+it->ch.size();
					lastpos +=addLen;
					last = it;
					++last;//消耗一个tps
				}

			}
			if(addLen!=0)
			{
				beg +=addLen;
				tgt = tgt.substr(0,end) + mid + tgt.substr(beg);
				
			}
			end = tgt.find(">",beg+1);
			if(end==string::npos)
				break;

			int cma = tgt.find(",",beg+1);
			if(cma == string::npos)
				break;
			if(cma>end)
			{
				++end;
				continue;
			}

			string numstr = tgt.substr(beg+1,cma-beg-1);
			int index = Scan<int>(numstr);
			tgt = tgt.substr(0,beg+1) + ctps[index].ti+"," +ctps[index].ch+tgt.substr(end);
			end +=ctps[index].ti.size() - numstr.size();

			++end;
		}
		
	}


	if (last!=tmpp.end())
	{
		if (end <tgt.size())
		{
			int addLen = 0;
			string mid = tgt.substr(end,tgt.size()-end);
			int pos=0,lastpos = 0;
			for (it = last;it!=tmpp.end();++it)
			{

				if ((pos = mid.find(it->ti,lastpos))!=string::npos)
				{
					lastpos =it->ti.size()+pos;
					mid = mid.substr(0,pos) + "<"+it->ti+","+it->ch+">"+mid.substr(lastpos);
					addLen +=3+it->ch.size();
					lastpos +=addLen;
					last = it;
					++last;//消耗一个tps
				}

			}
		}
	}
	*/

	
	//可能需要还原，把本来不是数词而被识别出来的进行还原，并输出最后的词对
	//post(tgt);

	//最后的考虑
	//pseudoSen = tgt;

	
	
	/*for (int i=0;i<tps.size();++i)
	{
		if(cover.find(i)==cover.end())
			ctps.push_back(tps[i]);
	}
	
	sort(ctps.begin(),ctps.end(),scorecmp());*/



	//mergeTps(tps,ctps,medStr);//将单独出现的基本数字也加入到ctps中

	//
	/*ofstream out("out1",fstream::app);
	for (int i=0;i<tps.size();++i)
	{
		
		out<<tgt.substr(tps[i].beg,tps[i].end-tps[i].beg +1)<<"|||"<<tps[i].ch<<endl;
		
	}
	out.close();*/
	//
	

}


/*void post(string &tgt)
{
	size_t beg =0,end = 0;
	string rst;

	while ((beg = tgt.find("<",end))!=string::npos)
	{
		rst +=tgt.substr(end,beg - end);
		end = tgt.find(">",beg + 1);
		if(end == string::npos)
			break;
		size_t mid = tgt.find(",",beg +1);
		if (mid == string::npos)
		{
			++end;
			continue;;
		}
		string ti = tgt.substr(beg+1,mid-beg -1);
		string ch = tgt.substr(mid+1,end -mid -1);
		Utf8Stream u8(ch);
		string forward,backward,midstr,part;
		
		bool isForward= true;
		while (u8.ReadOneCharacter(part))//只会处理出现一处或0处的汉字
		{
			bool isCh = isChineseChar(part);
			if(isForward && !isCh)
			{
				forward +=part;
			}
			else if (isCh)
			{
				midstr +=part;
				isForward = false;
			}
			else
			{
				backward +=part;
				
			}
		}
		if(midstr.size() == ch.size())//输出(全是汉字)
		{
			rst += tgt.substr(beg,end +1 -beg);
		}
		else
		{
			if (forward.size() == ch.size())//(全是藏文，全部还原)
			{
				rst +=ti;
			}
			else
			{
				
			}
		}

		++end;
	}
}*/
void TiNum::mergeTps(vector<transPair> &tps,vector<transPair> &ctps, string &pseudoSen)//可以优化，但影响不大
{
	if(ctps.empty())
		ctps = tps;
	else
	{
		vector<transPair>::iterator it= ctps.begin();
		vector<transPair>::iterator last = it;//记录上一个it
		for (int i = 0;i<tps.size();++i)
		{
			
			for (;it!=ctps.end();++it)
			{
				if(it == ctps.begin())//第一个
				{
					if (tps[i].end < it->beg)
					{
						it = ctps.insert(it,tps[i]);
						break;
					}
					else if (tps[i].beg <= it->end)//有重叠部分
					{
						if(tps[i].beg >=it->beg && tps[i].end <=it->end)//如果复杂数词可以包括基本数词，将基本数词的源端替换复杂数词中对应的阿拉伯部分
						{
							it->ti =pseudoSen.substr(it->beg,tps[i].beg - it->beg) + tps[i].ti ;
							if(tps[i].end <it->end)
								it->ti +=pseudoSen.substr(tps[i].end+1,it->end-tps[i].end);
							
						}
						
						break;
					}
				}
				else if((it+1) ==ctps.end())//最后一个
				{
					if (tps[i].end <it->beg)
					{
						it = ctps.insert(it,tps[i]);
						break;
					}
					else if (tps[i].beg >it->end)
					{
						//ctps.push_back(tps[i]);
						for (;i<tps.size();++i)
						{
							ctps.push_back(tps[i]);
						}
						break;
					}
					else if (tps[i].beg >=it->beg && tps[i].end <=it->end)
					{
						it->ti =pseudoSen.substr(it->beg,tps[i].beg - it->beg) + tps[i].ti ;
						if(tps[i].end <it->end)
							it->ti +=pseudoSen.substr(tps[i].end+1,it->end-tps[i].end);
					}
					
					
				}
				else //中间的结点
				{
					/*if(tps[i].end >=it->beg)
						break;
					else
					{
						ctps.insert(it,tps[i]);
						break;
					}*/
					if ((tps[i].beg<it->beg && tps[i].end >=it->beg) || (tps[i].beg<=it->end && tps[i].end >it->end))//只要和复杂数词有部分重合，断开
					{
						break;
					}
					else if (tps[i].beg>=it->beg && tps[i].end <=it->end)//完全重合
					{
						it->ti =pseudoSen.substr(it->beg,tps[i].beg - it->beg) + tps[i].ti ;
						if(tps[i].end <it->end)
							it->ti +=pseudoSen.substr(tps[i].end+1,it->end-tps[i].end);
						break;
					}
					else if (tps[i].end<it->beg)//插入
					{
						it = ctps.insert(it,tps[i]);
						break;
					}
				}
				

			}
		}

	}
	
	
}
void TiNum::tokenSen(const string &src,vector<string> &sen,vector<transPair> &tps,string &pseudoSen)//最大匹配，先按照音节点和0-9//提前将基本数字词转换为阿拉伯数字
{
	vector<string> slm;
	//////////////////////////////////////////////////////////////////////////最开始的算法
	/*Utf8Stream u8(src);
	string part;
	string rest;//记录在下一个点号或者竖杠之前的
	//bool lastNum=false;
	string numstr;//如果出现基本藏文数字，记录其阿拉伯形式
	string numore;//如果出现基本藏文数字,记录其源形式
	while (u8.ReadOneCharacter(part))//按照音节，竖杠，基本数字字符切分//粒度太大，要考虑(),还用分词的预处理
	{
		if (part ==sdot)
		{
			if (!numstr.empty())
			{
				slm.push_back(numstr);
				numstr = "";
				numore = "";
				ti2ch[numore] = numstr;
			}
			slm.push_back(rest+sdot);
			rest = "";
			
		}
		else if (bsc.find(part)!=bsc.end())
		{
			//lastNum = true;
			stringstream ss;
			ss<<bsc[part];
			if (!rest.empty())
			{
				slm.push_back(rest);
				rest = "";


			}
			numstr +=ss.str();
			numore +=part;

			//slm.push_back(ss.str());
			
		}
		else if (part ==sep)
		{
			if (!numstr.empty())
			{
				slm.push_back(numstr);
				numstr = "";

				numore = "";
				ti2ch[numore] = numstr;
			}
			else if(!rest.empty())
			{
				slm.push_back(rest);
				rest = "";
			}
			slm.push_back(sep);
		}
		else
		{
			if (!numstr.empty())
			{
				slm.push_back(numstr);
				numstr = "";

				numore = "";
				ti2ch[numore] = numstr;
			}
			rest +=part;
		}
	}//while
	if(!rest.empty())
		slm.push_back(rest);
	if(!numstr.empty())
	{
		slm.push_back(numstr);
		ti2ch[numore] = numstr;
	}*/
//////////////////////////////////////////////////////////////////////////最开始的算法

	//分词算法
	vector<pair<string,bool> > block;
	spf.preSplit(src,block);//可能会改变原句子,多加了点，所以要在内部维护一个伪的源句子
	//int sumlen=0;
	/*int beg = 0;
	for(int i=0;i<block.size();++i)
	{
		int len = block[i].first.size();
		if(block[i].first!=src.substr(beg,len))
		{
			if (block[i].first.substr(len-3,3) ==sdot && block[i].first.substr(0,len-3) ==src.substr(beg,len-3))
			{
				block[i].first = src.substr(beg,len-3);
			}
			else
				cerr<<"preSplit sentence is wrong"<<endl;
		}
		beg +=block[i].first.size();
	}*/
	


	//
	/*ofstream out("out",fstream::app);
	for (int i=0;i<block.size();++i)
	{
		out<<block[i].first<<"|"<<block[i].second<<" ";
	}*/
	//out.close();

	//
	string bscnumstr;
	string arbstr;
	int len=0;
	int last = 0;//记录上一个token是不是藏文最基本的数词块
	for (int i=0;i<block.size();++i)
	{
		pseudoSen +=block[i].first;

		len +=block[i].first.size();
		if (block[i].second)
		{
			int j=0;
			arbstr ="";
			while (j<block[i].first.size() && bsc.find(block[i].first.substr(j,3))!=bsc.end())
			{
				stringstream ss;
				ss<<bsc[block[i].first.substr(j,3)];
				arbstr +=ss.str();
				j +=3;
			}
			if(j ==block[i].first.size())
			{
				if (last ==1)
				{
					vector<transPair>::reference rf = tps.back();
					rf.end +=block[i].first.size();
					rf.ti +=block[i].first;
					rf.ch +=arbstr;

					vector<string>::reference vrf = slm.back();
					vrf +=block[i].first;
				}
				else
				{
					transPair tp;
					tp.ch = arbstr;
					tp.ti = block[i].first;
					tp.beg = len - block[i].first.size();
					tp.end = len - 1;
					tps.push_back(tp);
					slm.push_back(block[i].first);//
					last = 1;
				}
				
			}
			else if(block[i].first == ".")
			{
				if(last ==1)
				{
					vector<transPair>::reference rf = tps.back();
					rf.end +=1;
					rf.ti +=".";
					rf.ch +=".";

					vector<string>::reference vrf = slm.back();
					vrf +=".";
				}
				else
				{
					last = 0;
					slm.push_back(block[i].first);
				}
				
			}
			else
			{
				last = 0;
				slm.push_back(block[i].first);
			}
		}
		else
		{
			last = 0;
			slm.push_back(block[i].first);
		}
		/*if (bsc.find(block[i].first)!=bsc.end())
		{
			bscnumstr +=block[i].first;
			arbstr +=bsc[block[i].first];
		}
		else
		{
			if(!bscnumstr.empty())
			{
				transPair tp;
				tp.ch = arbstr;
				tp.ti = bscnumstr;
				tps.push_back(tp);
				slm.push_back(arbstr);
				bscnumstr = "";
				arbstr = "";
			}
			else
				slm.push_back(block[i].first);
				
		}*/
		
	}

	//分词算法
	//
	/*ofstream out("out",fstream::app);
	out<<"slm:"<<endl;
	for (int i=0;i<slm.size();++i)
	{
		out<<slm[i]<<" ";

	}
	out<<endl;*/
	//

	shortPath(slm,sen);

	//
	//ofstream out("out",fstream::app);
	/*out<<"sen:"<<endl;
	for (int i=0;i<sen.size();++i)
	{
		out<<sen[i]<<" ";
	}
	out.close();*/

	//

	//vector<transPair> tps;

	//numRec(sen,tps);

	/*ofstream log("log",fstream::app);
	for (int i=0;i<sen.size();++i)
	{
		log<<sen[i]<<" ";
	}
	log<<endl;
	log.close();*/

	

	

}
void TiNum::shortPath(vector<string> &slm,vector<string> &sen)
{
	int len = slm.size();
	vector<vector<int> > lattice(len);
	vector<int> path;//
	vector<int> preNode;//记录第i词的前驱；span=[j..i],记录j
	path.insert(path.end(),len,0);
	preNode.insert(preNode.end(),len,0);

	for (int i=0;i<len;++i)
	{
		lattice[i].insert(lattice[i].end(),len,0);
	}
	
	int maxLen = maxSlm >len?len:maxSlm;
	for (int span=1;span<=maxLen;++span)
	{
		for (int i=0;i<len+1-span;++i)
		{
			if(span == 1)
				lattice[i][i] = 2;
			else
			{
				string term = Vector2String(slm,i,i+span-1);
				if(dicTerm.find(term)!=dicTerm.end())
					lattice[i+span-1][i] = 1;
				if(dicNum.find(term)!=dicNum.end())
					lattice[i+span-1][i] = 2;
			}
			
		}
	}

	path[0] = 1;
	for (int i=0;i<len;++i)
	{
		path[i] = INT_MAX;
		for (int j=0;j<=i;++j)
		{
			if (lattice[i][j])
			{
				if (j==0)
				{
					path[i] = 1;
					preNode[i] = 0;
					break;
				}
				else
				{
					if (path[i] > path[j-1] + lattice[i][j])
					{
						path[i] = path[j-1] + lattice[i][j];
						preNode[i] = j;
					}
				}
			}
		}
	}

	int i = len -1;

	sen.resize(path[i]);
	int j = path[i];
	
	int prep;
	while ((prep=preNode[i])!=0)
	{
		sen[--j] = Vector2String(slm,prep,i);
		i = prep-1;
	}

	sen[0] = Vector2String(slm,0,i);


}
string TiNum::Vector2String(std::vector<string> &slm, int i, int j)
{
	string str;
	for (int b=i;b<=j;++b)
	{
		str +=slm[b];
	}
	return str;
}
void TiNum::basicRec(string &tgt)
{
	
}
void TiNum::numRec(vector<string> &vec,vector<transPair> &tps)
{
	//0. 先把最简单的0-9直接替换成阿拉伯
	//1. 按照数词的最小粒度切分（亿，十万，百万，千万，万，千，百，0-99作为一个整体）//30是31-39的子句，要按照最大匹配切词
	//数量词保存在map,sl; 基数词保存在,js;前缀pre;后缀是suffix;连词link，小数点dot，如果直接出现阿拉伯数字
	//2.数量词要记录三个值<藏文数量词，中文数量词，阿拉伯>
	//3.计数词要记录两个值<藏文基数词，阿拉伯>
	//4. 要考虑单独的基数词的情况，就是最简单的情况
	//5. 在case6中操作即可
	
	double sum =0.0;//
	double fraction=0.0;
	int denominator;
	int last=6;//上一个case,前一个状态
	int beforlast = 6;//前两个状态
	int lastnum=0;//最近一次读到的基数词或者前缀词
	int isDot=0;//小数点，1是代表零点二百三十,这样的模式；2，是代表零点二三
	float ten = 10;//当小数部分是模式2的时候，要累积除以10
	
	string lastsl;//上次读到的数量词
	string total;//如果开头出现两个连续的数量词，记录第一个
	string org;

	vector<int> ctnNum;//如果出现连续的数字，不相加，而是罗列
	bool isCtnNum = true;//是否是连续基本数字的罗列,仅监视整数部分

	bool isUsed = false;//如果出现这样的情况：sl js sl ,在if(i+1<vec.size() && js[vec[i]]>=4 && js[vec[i]]<=9 && sl.find(vec[i+1])!=sl.end())就会将js判断为pre，pre和后面的sl结合，出现错误；这时候要判断下前面的sl是否被计算过，如果没有被计算，则js和前面的结合

	//ofstream out("log",fstream::app);
	int i;

	int len=0;

	for(i=0;i<vec.size();++i)//
	{
		len +=vec[i].size();
		org +=vec[i];
		int pattern;
		if(sl.find(vec[i])!=sl.end())
			pattern = 0;
		else if(js.find(vec[i])!=js.end())//前缀词和基数词有重复，如果后面是数量词那就是前缀词，否则时基数词//
		{
			if( (isUsed || (i>=1 && sl.find(vec[i-1])==sl.end()) || i==0) && i+1<vec.size() && js[vec[i]]>=4 && js[vec[i]]<=9 && sl.find(vec[i+1])!=sl.end())//?????
			{
				pattern = 2;
				isUsed = false;
			}
			else
				pattern = 1;
		}
		else if(pre.find(vec[i])!=pre.end())
			pattern = 2;
		else if(linker.find(vec[i])!=linker.end())
			pattern = 3;
		else if(dot.find(vec[i])!=dot.end())
			pattern =4;
		else if(noNum.find(vec[i])!=noNum.end())
			pattern = 5;
		else if(suffix.find(vec[i])!=suffix.end())
			pattern = 7;
		else
			pattern = 6;
		switch(pattern)
		{
		case 0://数量词
			{
				isCtnNum = false;
				if(beforlast == 6 && last ==0)//sl sl ；譬如215万
				{
					total = lastsl;
					//lastsl = vec[i];
					//last = pattern;
				}
				else if(last == 2 &&beforlast ==0)//sl pre sl;三千万;只识别3000
				{
					sum +=lastnum * sl[vec[i]].arb;
					isUsed = true;
					org= org.substr(lastsl.size());
				}
				else if(last == 2)//pre sl;前缀词和后面的数量词结合
				{
					sum +=lastnum * sl[vec[i]].arb;
					isUsed = true;
					//lastsl = vec[i];
					//last = pattern;
				}
				
				

				lastsl = vec[i];//
				beforlast = last;//这个是前两个状态
				last = pattern;//需要记录两个状态位，这个是前一个状态

			}
			break;
		case 1://基数词
			{
				
				int num = js[vec[i]];
				if (isCtnNum)
				{
					ctnNum.push_back(num);
				}
				if(isDot==1)//小数部分为模式1,；先按照整数的生成，最后再除以一个分母
				{
					
					if(last ==0)
						fraction +=num * sl[lastsl].arb;

				}
				else if (isDot == 2)//小数部分为模式2；分母累积相乘10
				{
					fraction +=num/ten;
					ten =ten*10;
				}
				else
				{
					if(last == 1)//有问题
						sum +=num;
					else if(last ==0)
					{
						if(beforlast !=2)//前两个状态不是前缀//防止pre sl js,这样的话，sl被用了2次
							sum +=num * sl[lastsl].arb;
						else
							sum +=num;
					}
					else 
					{
						sum +=num;
					}
				}
				beforlast = last;
				last = pattern;
			}
			break;
		case 2://前缀
			{
				/*if (last == 1 && beforlast ==6)
				{
					transPair tp;
					tp.ti = org;
					tp.ch = Int2Str((int)sum);
					tp.beg = len - org.size();
					tp.end = tp.beg + tp.ti.size() -1;
					
					tps.push_back(tp);

				}*/
				
				isCtnNum = false;
				lastnum= pre[vec[i]];
				beforlast = last;
				last = pattern;
				

			}
			break;
		case 3://linker
			{
				isCtnNum = false;
				if(last==1 || last == 0)
				{

					if(last==0 && beforlast ==6)
					{
						sum = sl[lastsl].arb;
					}
					else if(last ==0 && (beforlast ==1||beforlast ==0))
						sum +=sl[lastsl].arb;
					beforlast = last;
					last = pattern;
				}
				
				else//如果linker前面不是基数词的话，舍弃
				{
					org = "";
					total  = "";
					isDot = 0;
					beforlast = last;
					last = 6;
					lastnum = 0;
					fraction = 0.0;
					sum = 0.0;
					ten = 10;
				}

			}
			break;
		case 4://点
			{
				isCtnNum = false;
				if (i+1<vec.size())
				{
					if(sl.find(vec[i+1])!=sl.end())//点号后面一位是数量词
					{
						isDot = 1;
						denominator = sl[vec[i+1]].arb *10;
					}
					else if(i+2<vec.size())
					{
						if (sl.find(vec[i+2])!=sl.end())//
						{
							isDot = 1;
							denominator = sl[vec[i+2]].arb *10;
						}
						else
							isDot = 2;
					}
					else
						isDot = 2;
				}
				
				beforlast = last;
				last = pattern;
				//isDot = true;
			}
			break;
		case 5://noNum
			{
				isCtnNum = false;
				if(sum !=0 )//这个成分表明前面一个单位不存在：比如没有万，就要减一万
				{
					sum -=lastnum ;
				}

				beforlast = last;
				last = pattern;

				lastsl = "";
			}
			break;
		case 7://suffix
			{
				isCtnNum = false;
				if(last ==1)
				{
					beforlast = last;
					last = pattern;
				}
				else //普通后缀，不是数词后缀
				{
					org = "";
					total  = "";
					isDot = 0;
					beforlast = last;
					last = 6;
					lastnum = 0;
					fraction = 0.0;
					sum = 0.0;
					ten = 10;
				}
			}
			break;
		case 6://记得把total清空
			{
				
				if (last!=6)//输出翻译对
				{

					transPair tp;
					tp.ti = org;
					tp.ti = tp.ti.substr(0,org.size()-vec[i].size());

					if(last ==3 && tp.ti.size()>=9)// 如果以连词结尾，就要把连词删除
						tp.ti = tp.ti.substr(0,tp.ti.size()-9);

					string ctnstr;
					if(isCtnNum && ctnNum.size()>=2)
					{
						vector<int>::iterator it;
						for (it=ctnNum.begin();it!=ctnNum.end();++it)
						{
							ctnstr +=Int2Str(*it);
						}
					}
					else
						isCtnNum = false;
					
					if(isDot ==1)
					{
						//isDot = false;
						if(isCtnNum)
						{
							float frac = fraction/denominator;
							stringstream ss;
							ss<<frac;
							tp.ch = ctnstr + ss.str();
						}
						else
						{
							sum +=fraction/denominator;
							//out<<sum;
							stringstream ss;
							ss<<sum;
							ss>>tp.ch;
						}
						

					}
					if (isDot ==2)
					{
						if(isCtnNum)
						{
							float frac = fraction/denominator;
							stringstream ss;
							ss<<frac;
							tp.ch = ctnstr + ss.str();
						}
						else
						{
							sum +=fraction;
							stringstream ss;
							ss<<sum;
							ss>>tp.ch;

						}
						
					}
					else//识别整数数词
					{
						if (isCtnNum)
						{
							tp.ch = ctnstr;
						}
						else
						{
							if(last == 0 && sum==0.0 &&sl[lastsl].arb==100)//如果仅出现一个数量词一百
							{
								sum +=sl[lastsl].arb;
							}
							if (sum !=0)
							{
								stringstream ss;
								ss<<(int)sum;
								//out<<(int)sum;
								ss>>tp.ch;
							}
						}
						
						
					}

					if(!total.empty() && !isCtnNum)
					{
						tp.ch +=sl[total].ch;
					}
						//out<<sl[total].ch<<endl;
					//else
						//out<<endl;
					tp.beg = len - org.size();
					tp.end = tp.beg + tp.ti.size() -1;
					if(sum!=0 || isCtnNum)
						tps.push_back(tp);
				}



				org = "";
				total  = "";
				isDot = 0;
				beforlast = last;
				last = 6;
				lastnum = 0;
				fraction = 0.0;
				sum = 0.0;
				ten = 10;
				isCtnNum = true;
				ctnNum.clear();
			}
			break;
		}


	}

	if (last!=6)//输出翻译对
	{

		transPair tp;
		tp.ti = org;
		//tp.ti = tp.ti.substr(0,org.size()-vec[i].size());

		if(last ==3 && tp.ti.size()>=9)// 如果以连词结尾，就要把连词删除
			tp.ti = tp.ti.substr(0,tp.ti.size()-9);

		string ctnstr;
		if(isCtnNum && ctnNum.size()>=2)
		{
			vector<int>::iterator it;
			for (it=ctnNum.begin();it!=ctnNum.end();++it)
			{
				ctnstr +=Int2Str(*it);
			}
		}
		else
			isCtnNum = false;

		/*if(isDot ==1)
		{
			//isDot = false;
			sum +=fraction/denominator;
			//out<<sum;
			stringstream ss;
			ss<<sum;
			ss>>tp.ch;

		}
		if (isDot ==2)
		{
			sum +=fraction;
			stringstream ss;
			ss<<sum;
			ss>>tp.ch;

		}
		else
		{
			if(last == 0 && sum==0.0 &&sl[lastsl].arb==100)//如果仅出现一个数量词一百
			{
				sum +=sl[lastsl].arb;
			}
			stringstream ss;
			ss<<(int)sum;
			//out<<(int)sum;
			ss>>tp.ch;
		}

		if(!total.empty())
		{
			tp.ch +=sl[total].ch;
		}*/
		//out<<sl[total].ch<<endl;
		//else
		//out<<endl;
		if(isDot ==1)
		{
			//isDot = false;
			if(isCtnNum)
			{
				float frac = fraction/denominator;
				stringstream ss;
				ss<<frac;
				tp.ch = ctnstr + ss.str();
			}
			else
			{
				sum +=fraction/denominator;
				//out<<sum;
				stringstream ss;
				ss<<sum;
				ss>>tp.ch;
			}


		}
		if (isDot ==2)
		{
			if(isCtnNum)
			{
				float frac = fraction/denominator;
				stringstream ss;
				ss<<frac;
				tp.ch = ctnstr + ss.str();
			}
			else
			{
				sum +=fraction;
				stringstream ss;
				ss<<sum;
				ss>>tp.ch;

			}

		}
		else
		{
			if (isCtnNum)
			{
				tp.ch = ctnstr;
			}
			else
			{
				if(last == 0 && sum==0.0 &&sl[lastsl].arb==100)//如果仅出现一个数量词一百
				{
					sum +=sl[lastsl].arb;
				}
				if (sum !=0)
				{
					stringstream ss;
					ss<<(int)sum;
					//out<<(int)sum;
					ss>>tp.ch;
				}
			}


		}

		if(!total.empty() && !isCtnNum)
		{
			tp.ch +=sl[total].ch;
		}
		
		tp.beg = len - org.size();
		tp.end = tp.beg + tp.ti.size() -1;
		tps.push_back(tp);
	}
	//IsTenTps(tps);
	//out.close();
	if(!DebugMode)//degugmode就是不考虑翻译模型，识别什么样的数字就是什么样的数字
		EraseTps(tps);//删除0-10，在翻译模型中就可以学习到

}
void TiNum::EraseTps(vector<transPair> &tps)
{
	vector<transPair>::iterator it;
	for (it = tps.begin();it!=tps.end();)
	{
		float num = Scan<float>(it->ch);
		if(num >=0 && num <=4 && isInt(num))
			it = tps.erase(it);
		else
			++it;
	}
}
void TiNum::IsTenTps(std::vector<transPair> &tps)
{
	vector<transPair>::iterator it;
	for (it = tps.begin();it!=tps.end();++it)
	{
		float num = Scan<float>(it->ch);
		if(num >=0 && num <=10 && isInt(num))
		{
			cerr<<senti<<endl;
			//flog<<senti<<endl;
			break;
		}
	}
}
void TiNum::ordinalTimeRec(std::string &tgt)
{

}
TiNum::~TiNum(void)
{
}

}