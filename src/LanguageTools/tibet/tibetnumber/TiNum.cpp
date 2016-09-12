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

	rex.initialize(path);//�����������Rex.txt

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

//src ԭ�䣻tgt �滻֮��ľ��ӣ�tps �������ʣ�ctps �������ʣ��������ֻ������ʣ� pseudoSen token֮��ľ��ӣ�������΢С�ı�;medStr��Դ�����г��ֵĻ������ʶ��滻�ɰ���������
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

	//����ĸ���������ʽ
	//extraRex(pseudoSen,tps);//��Ӧreadme����ĵ�һ������

	composedNum(pseudoSen,medSen,tgt,tps,ctps);

	replace(tgt,"#left#","<");
	replace(tgt,"#right#",">");

	//tgt = pseudoSen;
	//numRec(sen);
	//ordinalTimeRec(tgt);

}
void TiNum::extraRex(string &tgt,vector<transPair> &ctps)//���ܻ����ֱ�ӵ�����
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
	int p=0;//��pseudoSenָ��
	int t=0;//tgt��ָ��
	//string tgt;
	//���ھ������滻�������ʣ���ʽ���£�
	//XXXXXXXXX&3&2000XXXXX
	//3��ʾ��tps���ŵ���������Ӧ�Ĳ����ַ��滻Ϊ2000
	//����Ϊ����ԭ�����г��ֵİ�������������
	for (int i=0;i<tps.size();++i)//��ʱ��tps��Ԫ�ص�ָ��Ӧ��ָ��tgt��λ��//���������
	{
		int beg = tps[i].beg;
		int end = tps[i].end;
		
		medStr +=pseudoSen.substr(p,beg-p);
		//t +=beg -p;
		//tps[i].beg = t;
		stringstream ss;
		ss<<i;
		medStr +="&"+ss.str()+"&"+tps[i].ch;//Ϊ�������滻
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
	set<int > cover;//cover��¼�������ڸ��ӹ����еĻ������ʵ�����
	tgt = rex.findRplc(medStr,tps,ctps,cover);
	//������㸴�ӹ��������<5,200��>
	//5��ʾ��ctps���ŵ�5��
	//�����㸴�ӹ���ģ����<7,2000>,Ҳͬ������ctps�У����ǲ����¼��cover
	//

	vector<transPair> tmpp;

	for (int i=0;i<tps.size();++i)//����δ�ڸ������ִ��еĻ�������
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
			string mid = tgt.substr(end,beg-end);//midָ���Լ�����֮���<,>xxxxx<,>,��xxxxx
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
					++last;//����һ��tps
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
					++last;//����һ��tps
				}

			}
		}
	}
	*/

	
	//������Ҫ��ԭ���ѱ����������ʶ���ʶ������Ľ��л�ԭ����������Ĵʶ�
	//post(tgt);

	//���Ŀ���
	//pseudoSen = tgt;

	
	
	/*for (int i=0;i<tps.size();++i)
	{
		if(cover.find(i)==cover.end())
			ctps.push_back(tps[i]);
	}
	
	sort(ctps.begin(),ctps.end(),scorecmp());*/



	//mergeTps(tps,ctps,medStr);//���������ֵĻ�������Ҳ���뵽ctps��

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
		while (u8.ReadOneCharacter(part))//ֻ�ᴦ�����һ����0���ĺ���
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
		if(midstr.size() == ch.size())//���(ȫ�Ǻ���)
		{
			rst += tgt.substr(beg,end +1 -beg);
		}
		else
		{
			if (forward.size() == ch.size())//(ȫ�ǲ��ģ�ȫ����ԭ)
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
void TiNum::mergeTps(vector<transPair> &tps,vector<transPair> &ctps, string &pseudoSen)//�����Ż�����Ӱ�첻��
{
	if(ctps.empty())
		ctps = tps;
	else
	{
		vector<transPair>::iterator it= ctps.begin();
		vector<transPair>::iterator last = it;//��¼��һ��it
		for (int i = 0;i<tps.size();++i)
		{
			
			for (;it!=ctps.end();++it)
			{
				if(it == ctps.begin())//��һ��
				{
					if (tps[i].end < it->beg)
					{
						it = ctps.insert(it,tps[i]);
						break;
					}
					else if (tps[i].beg <= it->end)//���ص�����
					{
						if(tps[i].beg >=it->beg && tps[i].end <=it->end)//����������ʿ��԰����������ʣ����������ʵ�Դ���滻���������ж�Ӧ�İ���������
						{
							it->ti =pseudoSen.substr(it->beg,tps[i].beg - it->beg) + tps[i].ti ;
							if(tps[i].end <it->end)
								it->ti +=pseudoSen.substr(tps[i].end+1,it->end-tps[i].end);
							
						}
						
						break;
					}
				}
				else if((it+1) ==ctps.end())//���һ��
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
				else //�м�Ľ��
				{
					/*if(tps[i].end >=it->beg)
						break;
					else
					{
						ctps.insert(it,tps[i]);
						break;
					}*/
					if ((tps[i].beg<it->beg && tps[i].end >=it->beg) || (tps[i].beg<=it->end && tps[i].end >it->end))//ֻҪ�͸��������в����غϣ��Ͽ�
					{
						break;
					}
					else if (tps[i].beg>=it->beg && tps[i].end <=it->end)//��ȫ�غ�
					{
						it->ti =pseudoSen.substr(it->beg,tps[i].beg - it->beg) + tps[i].ti ;
						if(tps[i].end <it->end)
							it->ti +=pseudoSen.substr(tps[i].end+1,it->end-tps[i].end);
						break;
					}
					else if (tps[i].end<it->beg)//����
					{
						it = ctps.insert(it,tps[i]);
						break;
					}
				}
				

			}
		}

	}
	
	
}
void TiNum::tokenSen(const string &src,vector<string> &sen,vector<transPair> &tps,string &pseudoSen)//���ƥ�䣬�Ȱ������ڵ��0-9//��ǰ���������ִ�ת��Ϊ����������
{
	vector<string> slm;
	//////////////////////////////////////////////////////////////////////////�ʼ���㷨
	/*Utf8Stream u8(src);
	string part;
	string rest;//��¼����һ����Ż�������֮ǰ��
	//bool lastNum=false;
	string numstr;//������ֻ����������֣���¼�䰢������ʽ
	string numore;//������ֻ�����������,��¼��Դ��ʽ
	while (u8.ReadOneCharacter(part))//�������ڣ����ܣ����������ַ��з�//����̫��Ҫ����(),���÷ִʵ�Ԥ����
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
//////////////////////////////////////////////////////////////////////////�ʼ���㷨

	//�ִ��㷨
	vector<pair<string,bool> > block;
	spf.preSplit(src,block);//���ܻ�ı�ԭ����,����˵㣬����Ҫ���ڲ�ά��һ��α��Դ����
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
	int last = 0;//��¼��һ��token�ǲ��ǲ�������������ʿ�
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

	//�ִ��㷨
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
	vector<int> preNode;//��¼��i�ʵ�ǰ����span=[j..i],��¼j
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
	//0. �Ȱ���򵥵�0-9ֱ���滻�ɰ�����
	//1. �������ʵ���С�����з֣��ڣ�ʮ�򣬰���ǧ����ǧ���٣�0-99��Ϊһ�����壩//30��31-39���Ӿ䣬Ҫ�������ƥ���д�
	//�����ʱ�����map,sl; �����ʱ�����,js;ǰ׺pre;��׺��suffix;����link��С����dot�����ֱ�ӳ��ְ���������
	//2.������Ҫ��¼����ֵ<���������ʣ����������ʣ�������>
	//3.������Ҫ��¼����ֵ<���Ļ����ʣ�������>
	//4. Ҫ���ǵ����Ļ����ʵ������������򵥵����
	//5. ��case6�в�������
	
	double sum =0.0;//
	double fraction=0.0;
	int denominator;
	int last=6;//��һ��case,ǰһ��״̬
	int beforlast = 6;//ǰ����״̬
	int lastnum=0;//���һ�ζ����Ļ����ʻ���ǰ׺��
	int isDot=0;//С���㣬1�Ǵ�����������ʮ,������ģʽ��2���Ǵ���������
	float ten = 10;//��С��������ģʽ2��ʱ��Ҫ�ۻ�����10
	
	string lastsl;//�ϴζ�����������
	string total;//�����ͷ�������������������ʣ���¼��һ��
	string org;

	vector<int> ctnNum;//����������������֣�����ӣ���������
	bool isCtnNum = true;//�Ƿ��������������ֵ�����,��������������

	bool isUsed = false;//������������������sl js sl ,��if(i+1<vec.size() && js[vec[i]]>=4 && js[vec[i]]<=9 && sl.find(vec[i+1])!=sl.end())�ͻὫjs�ж�Ϊpre��pre�ͺ����sl��ϣ����ִ�����ʱ��Ҫ�ж���ǰ���sl�Ƿ񱻼���������û�б����㣬��js��ǰ��Ľ��

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
		else if(js.find(vec[i])!=js.end())//ǰ׺�ʺͻ��������ظ�������������������Ǿ���ǰ׺�ʣ�����ʱ������//
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
		case 0://������
			{
				isCtnNum = false;
				if(beforlast == 6 && last ==0)//sl sl ��Ʃ��215��
				{
					total = lastsl;
					//lastsl = vec[i];
					//last = pattern;
				}
				else if(last == 2 &&beforlast ==0)//sl pre sl;��ǧ��;ֻʶ��3000
				{
					sum +=lastnum * sl[vec[i]].arb;
					isUsed = true;
					org= org.substr(lastsl.size());
				}
				else if(last == 2)//pre sl;ǰ׺�ʺͺ���������ʽ��
				{
					sum +=lastnum * sl[vec[i]].arb;
					isUsed = true;
					//lastsl = vec[i];
					//last = pattern;
				}
				
				

				lastsl = vec[i];//
				beforlast = last;//�����ǰ����״̬
				last = pattern;//��Ҫ��¼����״̬λ�������ǰһ��״̬

			}
			break;
		case 1://������
			{
				
				int num = js[vec[i]];
				if (isCtnNum)
				{
					ctnNum.push_back(num);
				}
				if(isDot==1)//С������Ϊģʽ1,���Ȱ������������ɣ�����ٳ���һ����ĸ
				{
					
					if(last ==0)
						fraction +=num * sl[lastsl].arb;

				}
				else if (isDot == 2)//С������Ϊģʽ2����ĸ�ۻ����10
				{
					fraction +=num/ten;
					ten =ten*10;
				}
				else
				{
					if(last == 1)//������
						sum +=num;
					else if(last ==0)
					{
						if(beforlast !=2)//ǰ����״̬����ǰ׺//��ֹpre sl js,�����Ļ���sl������2��
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
		case 2://ǰ׺
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
				
				else//���linkerǰ�治�ǻ����ʵĻ�������
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
		case 4://��
			{
				isCtnNum = false;
				if (i+1<vec.size())
				{
					if(sl.find(vec[i+1])!=sl.end())//��ź���һλ��������
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
				if(sum !=0 )//����ɷֱ���ǰ��һ����λ�����ڣ�����û���򣬾�Ҫ��һ��
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
				else //��ͨ��׺���������ʺ�׺
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
		case 6://�ǵð�total���
			{
				
				if (last!=6)//��������
				{

					transPair tp;
					tp.ti = org;
					tp.ti = tp.ti.substr(0,org.size()-vec[i].size());

					if(last ==3 && tp.ti.size()>=9)// ��������ʽ�β����Ҫ������ɾ��
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
					else//ʶ����������
					{
						if (isCtnNum)
						{
							tp.ch = ctnstr;
						}
						else
						{
							if(last == 0 && sum==0.0 &&sl[lastsl].arb==100)//���������һ��������һ��
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

	if (last!=6)//��������
	{

		transPair tp;
		tp.ti = org;
		//tp.ti = tp.ti.substr(0,org.size()-vec[i].size());

		if(last ==3 && tp.ti.size()>=9)// ��������ʽ�β����Ҫ������ɾ��
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
			if(last == 0 && sum==0.0 &&sl[lastsl].arb==100)//���������һ��������һ��
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
				if(last == 0 && sum==0.0 &&sl[lastsl].arb==100)//���������һ��������һ��
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
	if(!DebugMode)//degugmode���ǲ����Ƿ���ģ�ͣ�ʶ��ʲô�������־���ʲô��������
		EraseTps(tps);//ɾ��0-10���ڷ���ģ���оͿ���ѧϰ��

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