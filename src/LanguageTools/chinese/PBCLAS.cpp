#include <strstream>
#include <fstream>
#include <iostream>
#include <strstream>
#include <cmath>
#include <vector>
#include <queue>
#include "PBCLAS.h"

CPBCLAS::CPBCLAS()
{
}

CPBCLAS::~CPBCLAS()
{
}

//��ʼ��
bool CPBCLAS::Initialize(const string &path, const string &tag, const double ddict)
{
    //fcl add argument : path
	InitBounds();

	if (!m_cChType.Initialize(path+"/punc", path+"/alph", path+"/date", path+"/num"))
	{
		return false;
	}

	if(tag == "n" || tag == "c")
	{
		if (!m_cPerceptron.Initialize(path+"/model"))
		{
			return false;
		}
	}

	if(tag=="y" || tag == "c")
	{
		if(!posTager.Initialize(path+"/pos", path+"/word_pos_ch", path+"/model_pos", 5))
			return false;
	}

	if(!m_Dict.Initialize(path+"/iniDict"))
	{
		return false;
	}
    
   /* if(!m_userDict.Initialize(path+"/userDict"))
    {
        return false;
    }*/

	char c;
	string s="";
	for(c='0';c<='9';c++)
	{
		s=c;
		nAlph.insert(s);
	}
	for(c='A';c<='Z';c++)
	{
		s=c;
		nAlph.insert(s);
	}
	for(c='a';c<='z';c++)
	{
		s=c;
		nAlph.insert(s);
	}

	nAlph.insert(".");
	nAlph.insert("%");

	//�����ܴʳ�
	m_nScanLen = 15;
	dictWeight = ddict;
	return true;
}

//���ñ߽��Ǽ���
void CPBCLAS::InitBounds()
{
	m_vBound.push_back("b");  //0
	m_vBound.push_back("m");  //1
	m_vBound.push_back("e");  //2
	m_vBound.push_back("s");  //3
	m_nBndCnt = (int)m_vBound.size();
}

void CPBCLAS::SplitByUserDict(const vector<string> &vsrc, vector<pair<string,int> > &vsrcproc, const set<string> &useDict)
{
	int maxlen =10;
	int len = (int)vsrc.size();
	int i;

	//userDict has nothing
	if(useDict.size() == 0)
	{
		for(i=0;i<len;i++)
			vsrcproc.push_back(make_pair(vsrc[i],0));
		return;
	}

	for(i=0;i<len;i++)
	{
		vector<string> elems;
		Split(vsrc[i],elems);

		int elemLen = (int)elems.size();
		int j,dis;
		queue<pair<int,int> > qtag;

		for(j=0;j<elemLen;j++)
		{
			for(dis=maxlen;dis>=0;dis--)
			{
				if(j+dis>elemLen-1)
					continue;

				string dict="";
				int k;
				for(k=j;k<=j+dis;k++)
					dict += elems[k];

				if(useDict.find(dict) != useDict.end())
				{
					qtag.push(make_pair(j,j+dis));
					j+=dis;
					break;
				}
			}//end for
		}//end for

		if(qtag.empty())
		{
			vsrcproc.push_back(make_pair(vsrc[i],0));
			continue;
		}
		 
		string src="";
		int k=0;

		while(!qtag.empty())
		{
			const pair<int,int> &span = qtag.front();
			for(;k<span.first;k++)
				src += elems[k];
			if(src != "")
				vsrcproc.push_back(make_pair(src,0));

			src = "";
			for(;k<=span.second;k++)
				src += elems[k];
			vsrcproc.push_back(make_pair(src,1));

			src = "";
			qtag.pop();
		}//end while
		src = "";
		if(k<(int)elems.size())
		{
			for(;k<(int)elems.size();k++)
				src += elems[k];
			vsrcproc.push_back(make_pair(src,0));
		}
	}
}

void CPBCLAS::SplitByRule(vector<pair<string,int> > &vsrcproc)
{
	vector<pair<string,int> > vproc;
	int i, len =(int)vsrcproc.size();
	for(i=0;i<len;i++)
	{
		if(vsrcproc[i].second == 1)
		{
			vproc.push_back(vsrcproc[i]);
			continue;
		}

		vector<pair<string,int> > temp;
		procSNP(vsrcproc[i].first,temp);
		
		int ltemp=(int)temp.size();
		int j;
		for(j=0;j<ltemp;j++)
			vproc.push_back(temp[j]);
	}

	vsrcproc.clear();
	vsrcproc=vproc;
}

void CPBCLAS::procSNP(const string &src, vector<pair<string,int> > &temp)
{
	temp.clear();
	vector<string> spsrc;
	Split(src,spsrc);

	int i,len=(int)spsrc.size();

	string alph=""; //��ĸ���ִ�
	string comn=""; //��ͨ��

	for(i=0;i<len;i++)
	{
		if(nAlph.find(spsrc[i]) != nAlph.end())
		{
			if(comn != "")
			{
				temp.push_back(make_pair(comn,0));
				comn = "";
			}
			alph += spsrc[i];
		}
		else if(m_cChType.GetType(spsrc[i]) == 0) //punc
		{
			if(alph != "")
			{
				temp.push_back(make_pair(alph,1));
				alph = "";
			}
			if(comn != "")
			{
				temp.push_back(make_pair(comn,0));
				comn = "";
			}

			temp.push_back(make_pair(spsrc[i],1));
		}
		else //��ͨ��
		{
			if(alph != "")
			{
				if(spsrc[i] == "��" || spsrc[i] == "��" || spsrc[i] == "��" ||
					spsrc[i] == "ʱ" || spsrc[i] == "��" || spsrc[i] == "��" ||
					spsrc[i] == "��" || spsrc[i] == "ǧ" || spsrc[i] == "��" ||
					spsrc[i] == "��" )
				{
					int tempSzie = (int)temp.size();
					if(tempSzie>=2 
						&& m_cChType.GetType(temp[tempSzie-1].first) == 0 
						&& temp[tempSzie-2].first.find(spsrc[i]) == string::npos) //ǰһ���Ƿָ�����ǰ������û��spsrc[i]
					{
						temp.push_back(make_pair(alph,1));
						temp.push_back(make_pair(spsrc[i],1));
					}
					else
					{
						alph += spsrc[i];
						temp.push_back(make_pair(alph,1));
					}//end if

					alph = "";
					continue;
				}
				else
				{
					temp.push_back(make_pair(alph,1));
					alph = "";
				}//end if 
			}//end if 
			
			comn += spsrc[i];
		}//end if
	}//end for
	
	if(alph != "")
	{
		temp.push_back(make_pair(alph,1));
		alph = "";
	}
	if(comn != "")
	{
		temp.push_back(make_pair(comn,0));
		comn = "";
	}
}

void CPBCLAS::Generate(const string &src, string &best,const string &tag, const set<string> &useDict)
{
	best =  "";

	if(tag == "y") //only posTager
	{
		posTager.Decode(src,best);
		return ;
	}
	
	// n || c 
	vector<string> vsrc;	
	vector<pair<string,int> > vsrcproc;	
	string part;
	istrstream is1(src.c_str());
	while(is1 >> part)
		vsrc.push_back(part);

	SplitByUserDict(vsrc,vsrcproc, useDict);

	//Add a rule module  ��ĸ���ִ��ϲ� + - / ����Ҫ�ֿ�
	SplitByRule(vsrcproc); //ֻ���� it->second==0�Ĳ���  

	int len = (int)vsrcproc.size();
	int i;
	for(i=0;i<len;i++)
	{
		string subBest;
		if(vsrcproc[i].second == 1)
			subBest = vsrcproc[i].first;
		else
			GenerateSplit(vsrcproc[i].first,subBest,tag);
		best += subBest + "  ";
	}
	
	best = best.substr(0,best.length()-2);

	
	if(tag == "c") //segmentor and postager
	{
		string bestpos;
		posTager.Decode(best,bestpos);
		best = bestpos;
	}
}

//�з�
void CPBCLAS::GenerateSplit(const string &src, 
                           string &best, 
                           const string &tag)
{
	map<string,int> dicSent; //ÿ�������еĴʵ��д�  5
	vector<CTagElem> elemnew;

	istrstream is(src.c_str());
	string word;

	elemnew.push_back(CTagElem("<s>", -1, -1));
	elemnew.push_back(CTagElem("<s>", -1, -1));

	while(is >> word)
	{
		vector<CTagElem> elems;
		Split(word, elems);
		AtomSeg(elems,dicSent,elemnew);
	}

	elemnew.push_back(CTagElem("</s>", -1, -1));
	elemnew.push_back(CTagElem("</s>", -1, -1));

	vector<vector<pair<string,int> > > featSets;
	vector<pair<string,int> > nullFeats;

	featSets.push_back(nullFeats);
	featSets.push_back(nullFeats);

	int len = int(elemnew.size());
	int from = 2, to = len - 3;
    int i;

	for (i = from; i <= to; ++i)
	{
		vector<pair<string,int> > feats;

		GenFeats(elemnew, i, feats);
		featSets.push_back(feats);
	}

	featSets.push_back(nullFeats);
	featSets.push_back(nullFeats);


	vector<map<string, double> > scores;

	GetScores(featSets, scores);
	BestEdges(elemnew, scores, dicSent);

//	best = TraceSeg(elemnew,scores);

	string res;
    res = TraceSeg(elemnew,scores);
	best = SegPostProc(res);
}

void CPBCLAS::Split(const string &src, vector<string> &elems)
{    
    elems.clear(); 
    int len = (int)src.size();
    int i = 0;

    while (i < len)
    {
        string ch;   
        if (src[i] < 0)
        {
            ch = src[i];
            ch += src[i + 1];
            i += 2;   
        }
        else
        {
           ch = src[i];      
           ++i;
        }        
        elems.push_back(ch);
    }
}

//�з�Ϊ��������
void CPBCLAS::Split(const string &src, vector<CTagElem> &elems)
{
	elems.clear();

	int i, curType;
	for (i = 0; i < 2; ++i)
	{
		elems.push_back(CTagElem("<s>", -1, -1));
	}

	istrstream is(src.c_str());
	string word;

	while (is >> word)
	{
		vector<string> chs;
		int len = int(word.size());
		int i = 0;

		while (i < len)
		{
			string ch;

			if (word[i] < 0)
			{
				ch = word[i];
				ch += word[i + 1];
				i += 2;
			}
			else
			{
				ch = word[i];
				++i;
			}
			
			curType = 4;

		/*	curType = charType(ch);

			string cc,cc1="";
			
			if(i<len)
				cc =  word[i];
			if(i+1<len && word[i]<0)
				cc += word[i+1];
			if(!elems.empty())
				cc1 = elems.back().ch;

			if((ch[0] == '.' || ch[0] == '/') && (charType(cc)==5 || word[i] >= '0' && word[i] <= '9'))
					curType = 5;//CT_NUM
			else if((ch == "��" || ch == "��" || ch == "��") && charType(cc)==5)
					curType = 5;//CT_NUM
            else if((ch=="��" || ch[0]=='%') && !elems.empty() && elems.back().elemsClass == 5)
					curType = 5;//CT_NUM
			else if(charType(cc1)==5 && (ch == "��" || ch == "��"))
					curType = 5;//CT_NUM
			else if((ch == "��" || ch == "��") && (cc1 == "��" || cc1 == "��"))
				    curType = 5;//CT_NUM
			//else if((ch == "��" || ch == "��" || ch[0] == '/' || ch[0] == '-' || ch[0] == '��') && !elems.empty() && elems.back().elemsClass != 5)
            else if((ch == "��" || ch == "��" ) && !elems.empty() && elems.back().elemsClass == 6)
                    curType = 6;//CT_LETTER
			else if(ch == "��" && ( charType(cc) == 7 || cc == "��" ))
				    curType = 7;//CT_CHINESENUM
			else if(ch == "��" && !elems.empty() && elems.back().elemsClass == 7)
					curType = 7;//CT_CHINESENUM
			else if(ch == "��" && ( cc == "֮" || charType(cc)==7))
					curType = 7;//CT_CHINESENUM
			else if(ch == "֮" && charType(cc)==7)
					curType = 7;//CT_CHINESENUM
			else if(ch == "��" && charType(cc)==7 && !elems.empty() && elems.back().elemsClass == 7)
					curType = 7;//CT_CHINESENUM
			*/

			elems.push_back(CTagElem(ch, m_cChType.GetType(ch),curType));
		}
	}

	for (i = 0; i < 2; ++i)
	{
		elems.push_back(CTagElem("</s>", -1, -1));
	}
}


bool CPBCLAS::isCHNum(const string &s)
{
	string CHNum = "���һ�������������߰˾�ʮإ��ǧ����ئ";
	int len = int(CHNum.size()),i;
	for(i=0;i<=len-2;i=i+2)
	{
		if(s == CHNum.substr(i,2))
			break;
	}
	if(i>len-2)
		return false;
	return true;
}

int CPBCLAS::charType(const string &s)
{
	if(s.length() == 1)
	{

		if(s[0] >= '0' && s[0] <= '9')
			return 5;//CT_NUM;
		
		else if(s[0] >= 'a' && s[0] <= 'z' || s[0] >= 'A' && s[0] <= 'Z')
			return 6;//CT_LETTER;
		
		else 
			return 4; //CT_SINGLE
	}
	
	else if(s >= "��" && s<= "��")
	    return 5;//CT_NUM

	else if(s>="��"&&s<="��" || s>="��" && s<="��")
	    return 6;//CT_LETTER

    
    else if(s>="��"&&s<="��" || s>="��" && s<="��")
    {
        return 6;
	}

	else if(isCHNum(s))
        return 7;//CHNum

	else
		return 4;//OTHERS
}

void CPBCLAS::AtomSeg(const vector<CTagElem> &elems, map<string,int> &dictSent,vector<CTagElem> &elemnew)
{
	int lb = 2,rb = int(elems.size())-3,i;
	string s="";

	int unknow = 8,dis = 0;

	for(i = lb; i <= rb; i++)
	{
		elemnew.push_back(CTagElem(elems[i].ch,elems[i].type,4));

		for(dis=unknow; dis >=0; dis--)
		{	
			if(i+dis>rb)
				continue;

			string un="";
			int p;
			for (p = i; p <= i + dis; ++p)
			{
				un += elems[p].ch;
			}

			if(m_Dict.InSet(un))
			{
			//	cout << "dict: " << un << endl;
				dictSent.insert(map<string,int>::value_type(un,5));
				int j;
				for(j=i+1;j<=i+dis;j++)
					elemnew.push_back(CTagElem(elems[j].ch,elems[j].type,4));
				i += dis;

				break;
			}

			un = "";
		}
	}
}

int CPBCLAS::InsClass(const string &s, const map<string,int> &sClass)
{
	map<string,int>::const_iterator it = sClass.find(s);
	if(it != sClass.end())
		return it->second;
	else
		return 0;
}

//���ɸû����µ�����
void CPBCLAS::GenFeats(const vector<CTagElem> &elems, 
                           const int &cur, 
                           vector<pair<string,int> > &feats)
{
	feats.clear();

	char feat[32];
	int n;
	
	map<string,string> m_punc;
	for(n = -2; n <= 2; ++n)
	{
		if(elems[cur + n].ch == ",")
			m_punc[elems[cur + n].ch] = "��";
		else
			m_punc[elems[cur + n].ch] = elems[cur + n].ch;
	}

	//Cn
	for (n = -2; n <= 2; ++n)
	{
		sprintf(feat, "%s",m_punc[elems[cur + n].ch].c_str());
		feats.push_back(make_pair(feat,n+2));
	}

	//CnCn+1
	for (n = -2; n <= 1; ++n)
	{
		sprintf(feat, "%s%s",m_punc[elems[cur + n].ch].c_str(), m_punc[elems[cur + n + 1].ch].c_str());
		feats.push_back(make_pair(feat,n+2));
	}

	//C-1C1
	sprintf(feat, "%s%s", m_punc[elems[cur - 1].ch].c_str(), m_punc[elems[cur + 1].ch].c_str());
	feats.push_back(make_pair(feat,4));

	//Pu(C0)
	sprintf(feat, "PuC0==%d", elems[cur].type == 0 ? 1 : 0);
	feats.push_back(make_pair(feat,0));

	//T(C-2)T(C-1)T(C0)T(C1)T(C2)
	sprintf(feat, "TC-22==%d%d%d%d%d", elems[cur - 2].type, elems[cur - 1].type, elems[cur].type, elems[cur + 1].type, elems[cur + 2].type);
	feats.push_back(make_pair(feat,0));
}

//���������ӳ�䵽tag�ĵ÷�
double CPBCLAS::FeatsPred(const vector<pair<string,int> > &feats, const int &tag)
{
	double score = 0.0;

	m_cPerceptron.PredWeight(feats, tag, score);
	
	return score;
}

//�����Span�ķ���
void CPBCLAS::GetScores(const vector<vector<pair<string,int> > > &featSets, 
                            vector<map<string, double> > &scores)
{
	scores.clear();

	int len = int(featSets.size());

    scores.resize(len);

	int lb = 2, rb = len - 3;
	int i;

	for (i = lb; i <= rb; ++i)
	{
		int m;

		for (m = 0; m < m_nBndCnt; ++m)
		{
            scores[i][m_vBound[m]] = FeatsPred(featSets[i], m);
		}
	}
}

//��ע��to��β����len�Ĵ�
void CPBCLAS::MarkBnd(vector<CTagElem> &elems, const int &to, const int &len)
{
	int from = to - len + 1;

	//��ע�ôʶ�
	if(len==1)
	{
		elems[to].bnd = "s";
	}
	else if(len == 2)
	{
		elems[from].bnd = "b";
		elems[to].bnd = "e";
	}
	else
	{
		elems[from].bnd = "b";
		elems[from+1].bnd = "m";
	}
}

//��̬�滮���·��
void CPBCLAS::BestEdges(vector<CTagElem> &elems, 
                            vector<map<string, double> > &scores, 
							const map<string,int> &sClass)
{
	elems[1].best = 0.0;

	int lb = 2, rb = int(elems.size()) - 3;
	int i;

	for (i = lb; i <= rb; ++i)
	{
		bool first = true;
		int dis;

		for (dis = 0; dis < m_nScanLen && i - dis >= lb; ++dis)
		{
			MarkBnd(elems, i, dis + 1);

			string s = "";
			double q = 0.0;

            double score = elems[i - dis - 1].best;
            int p;
            double incre = 0.0;

            for (p = i - dis; p <= i; ++p)
            {
                incre += scores[p][elems[p].bnd];
				s += elems[p].ch;
            }

			int sclass = InsClass(s,sClass);

			if(sclass == 5)
				q = dictWeight * (dis + 1);//�ʵ�

            score += incre + q;

            if (first || score > elems[i].best)
            {
                elems[i].best = score;
                elems[i].length = dis + 1;
            }

            first = false;
		}
	}
}

bool CPBCLAS::isNumCi(const string &s)
{
	int len = (int)s.size();
	if(len < 3)
		return false;
	if(s.substr(len-2,2)!="��" && s.substr(len-2,2)!= "��" && s.substr(len-2,2)!="��" 
		&& s.substr(len-2,2)!="ʱ" && s.substr(len-2,2)!="��" && s.substr(len-2,2)!="��" 
		&& s.substr(0,2)!="��")
		return false;
	else if(s.substr(0,2)=="��")
	{
		for(int i=2;i<len;++i)
			if(s[i]<'0' || s[i] >'9')
				return false;
	}
	else
	{
		for(int i=0;i<len-2;++i)
			if(s[i]<'0' || s[i] >'9')
				return false;
	}

	return true;
}

string CPBCLAS::SegPostProc(const string &str)
{
	string seg = "";
	istrstream is(str.c_str());
	string word;
	vector<string> newline;
	while(is >> word)
	{
		vector<string> elems;
		int len = (int)word.size(),i=0;
		vector<bool> tag;
			
		if(isNumCi(word))
		{
			newline.push_back(word);
			continue;
		}

		while (i < len)
		{
			string ch;

			if (word[i] < 0)
			{
				ch = word[i];
				ch += word[i + 1];
				i += 2;
				if(ch >= "��" && ch <= "��" || ch >= "��" && ch <= "��" || ch == "��")
					tag.push_back(false);
				else
					tag.push_back(true);
			}
			else
			{
				ch = word[i];
				++i;
				tag.push_back(false);
			}

				elems.push_back(ch);
		}

		len = (int)tag.size();
		bool flag = tag[0]; 
		string newword = "";
		for(i=0;i<len;i++)
		{
			if(i == len-1)
			{
				if(flag != tag[i])
				{
					newline.push_back(newword);
					newline.push_back(elems[i]);
				}
				else
				{
					newword += elems[i];
					newline.push_back(newword);
				}		
			}

			else
			{
				if(flag != tag[i])
				{
					flag = tag[i];
					newline.push_back(newword);
					newword = elems[i];
				}
				else
				{
					newword += elems[i];
				}
			}			 
		}
	}

	int lenm = (int)newline.size();
	for(int i=0;i<lenm;i++)
	{
		if(i!=lenm-1)
			seg += newline[i] + "  ";
		else
			seg += newline[i];
	}

	return seg;
}


// �������·������tag����
string CPBCLAS::TraceSeg(vector<CTagElem> &elems, vector<map<string, double> > &scores)
{
    string seg = "";

    int from = 2;
    int p = int(elems.size()) - 3;

    while (p >= from)
    {
		bool first = true;
        int len = elems[p].length;
        int k;

        for (k = 0; k < len; ++k)
        {
            seg = elems[p].ch + seg;
            --p;
        }

        if (p >= from)
        {
            seg = "  " + seg;
        }
    }

    return seg;
}

bool CPBCLAS::InsertEntry(const string &s, set<string> &useDict)
{
   // if(m_userDict.DInsertEntry(s))
    //    return true;
	//return false;
	
	useDict.insert(s);
	return true;
}

bool CPBCLAS::DeleteEntry(const string &s, set<string> &useDict)
{
   // if(m_userDict.DDeleteEntry(s))
     //   return true;

	//return false;
	set<string>::iterator it = useDict.find(s);
	if(it != useDict.end())
	{
		useDict.erase(it);
		return true;
	}
	
	cerr << "Error: dict: " << s << "isn't in useDict! " << endl; 
	return true;
}
