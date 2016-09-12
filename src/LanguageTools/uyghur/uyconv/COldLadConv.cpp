#include "COldLadConv.h"


COldLadConv::COldLadConv()
{
}

COldLadConv::~COldLadConv()
{
}

bool COldLadConv::LoadMapTab(const string &file)
{
	//FCL ADD LOCK
	//MutexLockGuard lock_gurad(m_mutex_lock);

    lettersDict.clear();
	lettersDictToArabic.clear();
	vowelDict.clear();
	vowelDictToArabic.clear();
	lowercase_map.clear();
	ifstream fin(file.c_str());

    if (!fin)
    {
        cerr << "Can not open file: " << file << endl;
        return false;
    }
	vector<string> table;
		string line;
		while(getline(fin,line))
		{
			table.push_back(line);
		}
		fin.close();

		if(table.size() < 6)
		{
			cerr << "Uy conv resource file error, line = " << table.size() << ", need 6." << endl; 
			return false;
		}

		int len=int(table[0].size())-3;
        if((unsigned char)table[0][0]==0xEF && 
			(unsigned char)table[0][1] == 0xBB && 
			(unsigned char)table[0][2] == 0xBF)
			table[0]=table[0].substr(3,len);
		istrstream fu(table[0].c_str());
		istrstream fl(table[1].c_str());
		istrstream yu(table[2].c_str());
		istrstream yl(table[3].c_str());
		string la,uy;
		
		while(fu>>uy&&fl>>la)
		{
			lettersDict.insert(make_pair(uy,la));
			lettersDictToArabic.insert(make_pair(la,uy));
		}

		while(yu>>uy&&yl>>la)
		{
			vowelDict.insert(make_pair(uy,la));
			vowelDictToArabic.insert(make_pair(la,uy));
		}
		


		//大小写转换表
		string cap, lower;
		istrstream cap_is(table[4].c_str());
		istrstream lower_is(table[5].c_str());
		while(cap_is >> cap && lower_is >> lower)
		{
			lowercase_map.insert(make_pair(cap,lower));
			uppercase_map.insert(make_pair(lower,cap));
		}

		return true;

}
// 给一个词word，对它进行按字母切开
void COldLadConv::token(const string& word, string& output)
{
	output = "";

	int i = 0;
	while(i < (int)word.size())
	{
		if(i == 0 && // 文件头<BOM>的过滤
			(unsigned char)word[i] == 0xEF && 
			(unsigned char)word[i+1] == 0xBB && 
			(unsigned char)word[i+2] == 0xBF    )
		{
			i += 3;
		}
		else if((unsigned char)word[i] == 0xD8 &&
			(unsigned char)word[i+1] == 0xA6     )
		{
			output += word.substr(i,4);
			i += 4;
		}
		else if((unsigned char)word[i] == 0xEF ||
				(unsigned char)word[i] == 0xEE   )
		{
			output += word.substr(i,3);
			i += 3;
		}
		else if((unsigned char)word[i] == 0xD8 ||
			(unsigned char)word[i] == 0xD9 ||
			(unsigned char)word[i] == 0xDA ||
			(unsigned char)word[i] == 0xDB)
		{
            output += word.substr(i,2);
			i += 2;
		}
		//else if(word[i] < 0) //GBK编码或者是符号
		//{
		//	output += word.substr(i,2);
		//	i += 2;
		//}
		else //ASCII
		{
			output += word.substr(i,1);
			i += 1;
		}
		output += " ";
	}
}
//将传统维文转化为拉丁维文,输入是一行，而不是一段文字
string COldLadConv::Old2Lad(const string &seq)
{
	//FCL ADD LOCK
	//MutexLockGuard lock_gurad(m_mutex_lock);

	set<string> unks,unlow;
	string tgt = "";
	// Add By LIUKAI
	string trim_temp = seq;
	if( trim_temp.size() >= 3 &&
		(unsigned char)trim_temp[0] == 0xEF && 
	   (unsigned char)trim_temp[1] == 0xBB && 
	   (unsigned char)trim_temp[2] == 0xBF )
			trim_temp=trim_temp.substr(3,trim_temp.size()-3);

	istringstream iss(trim_temp.c_str());
	string word,token,temp;

	while(iss >> word)
	{
		word=pro(word);
		this->token(word,token);
		int i=0, len = (int)token.length();
		while(i < len)
		{
			if(token[i] == ' ')
			{
				i+=1;
				continue;
			}
			else if(token[i] > 0)
			{
				tgt += token[i];
				i+=1;
				continue;
			}

			else if((unsigned char)token[i] == 0xD8 && 
					(unsigned char)token[i+1] == 0xA6 )
			{
				temp = token.substr(i,4);
				i += 4;
				if(vowelDict.find(temp) != vowelDict.end())
					temp = vowelDict[temp];
				else
				{
					unks.insert(temp);
					//tgt += temp;
					continue;
				}
				while(temp.size() > 0)
				{
					string ch;
					if(temp[0] > 0)
					{
						ch = temp.substr(0,1);
						temp = temp.substr(1,temp.size());
					}
					else
					{
						ch = temp.substr(0,2);
						temp = temp.substr(2,temp.size());
					}

					if(lowercase_map.find(ch)!=lowercase_map.end())
						tgt += lowercase_map[ch];
					else
					{
						tgt += ch;
						//unlow.insert(temp);
					}
					
				}
			}
			else if((unsigned char)token[i] == 0xD8 ||
				(unsigned char)token[i] == 0xD9 ||
				(unsigned char)token[i] == 0xDA ||
				(unsigned char)token[i] == 0xDB)
			{
				temp = token.substr(i,2);
				i += 2;
				if(lettersDict.find(temp)!=lettersDict.end())
                    tgt+= lettersDict[temp];
				else
				{
					unks.insert(temp);
					//tgt += temp;
					continue;
				}
			}
            else if(token[i]<0)
			{
				//cout<<"fdfdfd"<<endl;
				tgt+=token[i];
				i++;
			}
			else
			{
				cerr << "ERROR token[i] is 0" << endl;
				i++;
			}
		}// end of while word
		tgt += " ";
	}// end of while sent

	// log
	/*ofstream unmapping("unmapping.txt");
	for(set<string>::iterator iter = unks.begin();iter != unks.end();iter++)
		unmapping << (*iter) << endl;
	unmapping.close();
	ofstream unlowing("unlow.txt");
	for(set<string>::iterator iter = unlow.begin();iter != unlow.end();iter++)
		unlowing << (*iter) << endl;
	unlowing.close();*/

    return tgt;
}
string COldLadConv::Lad2Old(const string &seq)
{

	//FCL ADD LOCK
	//MutexLockGuard lock_gurad(m_mutex_lock);
	
	//ofstream outfile("codrec.txt");

// 	cout << "lad2old beg." << endl;
	string tgt = "";

	istringstream iss(seq.c_str());
	string word,temp="",word1;
	while(iss >> word)
	{
		//outfile << "curr_word = [" << word << "]" << endl;
		word=pro(word);
		int i=0,j=(int)word.size();
		word1=word;
		Utf8Stream utf8(word);
		//ofstream fout("abcfdg.txt");
		while(i<j)
		{
		  utf8.ReadOneCharacter(word);

		  //outfile << i << "," << j << " curr_word_char = [" << word << "]" << endl;
		  
			if(i == 0 && // 文件头<BOM>的过滤
			(unsigned char)word[i] == 0xEF && 
			(unsigned char)word[i+1] == 0xBB && 
			(unsigned char)word[i+2] == 0xBF)
			    i+=3;
			else if(word=="/")
			{
                tgt+=word;
				i++;
				while(i!=j)
				{
					//utf8.ReadOneCharacter(word);
					tgt+=word1[i];
					i++;
				}
			}

		
			else if(word=="g" || word=="z" || word=="s")
			{
				temp=word;
				//i+=(int)word.size();
                utf8.ReadOneCharacter(word);
				if(word=="h")
				{ 
					  temp+=word;
			          if(lettersDictToArabic.find(temp)!=lettersDictToArabic.end() && temp!="")
						  tgt +=lettersDictToArabic[temp];
					  i+=2;
		              
				}
				else if(word=="'")
				{
					utf8.ReadOneCharacter(word);
					if(lettersDictToArabic.find(temp)!=lettersDictToArabic.end() && temp!="")
						  tgt +=lettersDictToArabic[temp];
					if(lettersDictToArabic.find(word)!=lettersDictToArabic.end() && word!="")
						  tgt +=lettersDictToArabic[word];
					i+=3;


				}
				else
				{
                      if(lettersDictToArabic.find(temp)!=lettersDictToArabic.end() && temp!="")
						  tgt +=lettersDictToArabic[temp];
					  if(lettersDictToArabic.find(word)!=lettersDictToArabic.end() && word!="")
						  tgt +=lettersDictToArabic[word];
					  if(vowelDictToArabic.find(word)!=vowelDictToArabic.end() && word!="")
						  tgt +=vowelDictToArabic[word];
					  i+=2;
				}
				temp="";
				

			}
			else if(word=="n")
			{
				temp=word;
				//i+=(int)word.size();
                utf8.ReadOneCharacter(word);
				if(word=="g")
				{ 
					  temp+=word;
			          if(lettersDictToArabic.find(temp)!=lettersDictToArabic.end() && temp!="")
						  tgt +=lettersDictToArabic[temp];
					  i+=2;
				}
				else if(word=="'")
				{
					utf8.ReadOneCharacter(word);
					if(lettersDictToArabic.find(temp)!=lettersDictToArabic.end() && temp!="")
						  tgt +=lettersDictToArabic[temp];
					if(lettersDictToArabic.find(word)!=lettersDictToArabic.end() && word!="")
						  tgt +=lettersDictToArabic[word];
					i+=3;


				}
				else
				{
                      if(lettersDictToArabic.find(temp)!=lettersDictToArabic.end() && temp!="")
						  tgt +=lettersDictToArabic[temp];
					  if(lettersDictToArabic.find(word)!=lettersDictToArabic.end() && word!="")
						  tgt +=lettersDictToArabic[word];
					  if(vowelDictToArabic.find(word)!=vowelDictToArabic.end() && word!="")
						  tgt +=vowelDictToArabic[word];
					  i+=2;
				}
				temp="";
				
			}
			else if(word=="c")
			{
                temp=word;
				//i+=(int)word.size();
                utf8.ReadOneCharacter(word);
				if(word=="h")
				{ 
					  temp+=word;
			          if(lettersDictToArabic.find(temp)!=lettersDictToArabic.end() && temp!="")
						  tgt +=lettersDictToArabic[temp];
				}
				temp="";
				i+=2;
			}
			else
			{
                if(lettersDictToArabic.find(word)!=lettersDictToArabic.end() && word!="")
						  tgt +=lettersDictToArabic[word];
			    if(vowelDictToArabic.find(word)!=vowelDictToArabic.end() && word!="")
						  tgt +=vowelDictToArabic[word];
				i+=1;
			}
			
		}
		tgt+=" ";
		
	}

// 	cout << "lad2old end." << endl;

    return tgt;

}
//从文件读取非标准拉丁维文与标准拉丁维文转换的映射表
bool COldLadConv::LoadToStdMap(const string &file)
	{
		//FCL ADD LOCK
	//MutexLockGuard lock_gurad(m_mutex_lock);

		stdcase_map1.clear();
	    stdcase_map2.clear();
	    lowercase_map.clear();
	    ifstream fin(file.c_str());
		vector<string> table;
		string line;
		if(!fin)
		{
			cerr << "Can not open file: " << file << endl;
			return false;
		}
		while(getline(fin,line))
		{
			table.push_back(line);
		}
		fin.close();
		//验证格式
		if(table.size() < 6 )
			return false;

		string nonstd1, std1,nonstd2,std2;
		int len=int(table[0].size())-3;
		

		//TODO HJY
        if(table[0].size()>=3)
		{
			if((unsigned char)table[0][0]==0xEF && 
			(unsigned char)table[0][1] == 0xBB && 
			(unsigned char)table[0][2] == 0xBF)
			  table[0]=table[0].substr(3,len);
		}
		istrstream nonstd_is1(table[0].c_str());
		istrstream std_is1(table[1].c_str());
		while(nonstd_is1 >> nonstd1 && std_is1 >> std1)
		{
			stdcase_map1.insert(make_pair(nonstd1,std1));
		}
		istrstream nonstd_is2(table[2].c_str());
		istrstream std_is2(table[3].c_str());
		while(nonstd_is2 >> nonstd2 && std_is2 >> std2)
		{
			stdcase_map2.insert(make_pair(nonstd2,std2));
		}

		string cap, lower;
		istrstream cap_is(table[4].c_str());
		istrstream lower_is(table[5].c_str());
		while(cap_is >> cap && lower_is >> lower)
		{
			lowercase_map.insert(make_pair(cap,lower));
		}

		return true;

	}

//将非标准拉丁维文转换为标准拉丁维文
string COldLadConv::NoStdLad2StdLad(const string &seq)
{
	//FCL ADD LOCK
	//MutexLockGuard lock_gurad(m_mutex_lock);

	string tgt = "";

	istringstream iss(seq.c_str());
	string word,temp="",word1;
	string lowercase;
	for(map<string,string>::iterator iter=lowercase_map.begin();
		iter!=lowercase_map.end();iter++)
    lowercase+=iter->second;
	while(iss >> word)
	{
		word=pro(word);
		int i=0,j=(int)word.size();
		word1=word;
		Utf8Stream utf8(word);
		//ofstream fout("abcfdg.txt");
		while(i!=j)
		{
		  utf8.ReadOneCharacter(word);
		  //i+=(int)word.size();
		  //cout<<word<<" "<<word.size()<<endl;
		//}
			//utf8.ReadOneCharacter(word);
			//fout<<word<<endl;
			//i+=word.size();
		//}
			if(i == 0 && // 文件头<BOM>的过滤
			(unsigned char)word[i] == 0xEF && 
			(unsigned char)word[i+1] == 0xBB && 
			(unsigned char)word[i+2] == 0xBF)
			    i+=3;
			else if(word=="/")
			{
                tgt+=word;
				i++;
				while(i!=j)
				{
					//utf8.ReadOneCharacter(word);
					tgt+=word1[i];
					i++;
				}
			}

			//else if(word=="c" || word=="C")
			//{
			//	string temp1="";
			//	temp1+=word;
			//	//cout<<word<<" "<<word.size()<<endl;
			//	utf8.ReadOneCharacter(word);
			//	//cout<<word<<" "<<word.size()<<endl;
			//	
			//	if((int)word.size()==1)
			//	{
			//		temp1+=word;
			//		//cout<<temp1<<" "<<temp1.size()<<endl;
			//		if(stdcase_map2.find(temp1) != stdcase_map2.end())
			//		  tgt+= stdcase_map2[temp1];
			//		else
			//		  tgt+=temp1;
   //                 i+=2;
			//	}
			//	else
			//	{
			//		tgt+=temp1+word;
			//		i+=1+(int)word.size();
			//	}
			//	
			//}
			else
			{
				temp=word;
				if(lowercase.find(temp)!=string::npos)
			    {
				
					if(stdcase_map1.find(temp)!=stdcase_map1.end())
					{
						tgt+= stdcase_map1[temp];
						i+=(int)word.size();
					}
					else
					{   
						i+=(int)word.size();
						utf8.ReadOneCharacter(word);
						   temp+=word;
						if(stdcase_map2.find(temp)!=stdcase_map2.end())
						{
							tgt+=stdcase_map2[temp];
						}
						else
							tgt+=temp;
						i+=(int)word.size();
					}
				}
				else
				{
					tgt+=word;
					i+=(int)word.size();
				}
			}
			
			
		}
		tgt+=" ";
		////this->tokenNotStdLad(word,token);
		//int i = 0;
		//Utf8Stream utf8(word);
		//int len = (int)word.length();
		//while(i < len)
		//{
		//	//if(token[i] == ' ')
		//	{
		//		utf8.ReadOneCharacter(token);
		//		i++;
		//	}
		//	else if(token[i]=='/')
		//	{
		//		while(i!=len)
		//		{
		//			tgt+=token[i];
		//			i++;
		//		}
		//	}

		//	else if((unsigned char)token[i] == 0x43 || 
		//		(unsigned char)token[i] == 0x63)
		//	{
		//		utf8.ReadOneCharacter(token);
		//		utf8.ReadOneCharacter(token);
		//		temp = token.substr(i,2);;
		//		if(stdcase_map2.find(temp) != stdcase_map2.end())
		//				temp= stdcase_map2[temp];
		//			
		//		i += 2;
		//		while(temp.size() > 0)
		//		{
		//			string ch;
		//			if(temp[0] > 0)
		//			{
		//				ch = temp.substr(0,1);
		//				temp = temp.substr(1,temp.size());
		//			}
		//			else
		//			{
		//				ch = temp.substr(0,2);
		//				temp = temp.substr(2,temp.size());
		//			}
		//			if(lowercase_map.find(ch)!=lowercase_map.end())
		//			{
		//				
		//				tgt += lowercase_map[ch];
		//			}
		//			else
		//				tgt +=ch;
		//		}
		//	}
		//	else if(((unsigned char)token[i]>='a' && (unsigned char)token[i]<='z')
		//		||((unsigned char)token[i]>='A' && (unsigned char)token[i]<='Z'))
		//	{
		//		utf8.ReadOneCharacter(token);
		//		temp = token.substr(i,1);
		//		if(stdcase_map1.find(temp)!=stdcase_map1.end())
  //                  temp = stdcase_map1[temp];
		//		
		//		i += 1;
		//		while(temp.size() > 0)
		//		{
		//			string ch;
		//			if(temp[0] > 0)
		//			{
		//				ch = temp.substr(0,1);
		//				temp = temp.substr(1,temp.size());
		//			}
		//			else
		//			{
		//				ch = temp.substr(0,2);
		//				temp = temp.substr(2,temp.size());
		//			}
		//			if(lowercase_map.find(ch)!=lowercase_map.end())
		//			{
		//				
		//				tgt += lowercase_map[ch];
		//			}
		//			else
		//				tgt +=ch;
		//		}
		//	}
		//	else if(token[i]>0)
		//	{
		//		utf8.ReadOneCharacter(token);
		//		tgt+=token[i];
		//		i++;
		//	}
		//	else 
		//	{
		//		utf8.ReadOneCharacter(token);
		//		tgt += token;
		//		i+=(int)(token.size());
		//	}
		//	//else if(token[i]<0)
		//	//{
		//		//tgt +=token.substr(i,2);
		//		//i+=2;
		//	//}
		//	//else	//一般不会发生
		//	//{
		//	//	cerr << "ERROR tokenNotStdLad[i] is 0" << endl;
		//	//	i++;
		//	//}
		//	
		//	
		//}
		//tgt += " ";
		
	}
    return tgt;
	/*map<string,string>::iterator itv;
    string strTemp = seq;

	for(itv=stdcase_map1.begin();itv!=stdcase_map1.end();++itv)
		strTemp=Replace(strTemp,itv->first,itv->second);
	for(itv=stdcase_map2.begin();itv!=stdcase_map2.end();++itv)
		strTemp=Replace(strTemp,itv->first,itv->second);
	return strTemp;*/
	
}


string COldLadConv::pro(const string &word)
{

    string lowercase;
	for(map<string,string>::iterator iter=lowercase_map.begin();
		iter!=lowercase_map.end();iter++)
    lowercase+=iter->second;
	string tmp1=word,tmp2=word;
	Utf8Stream utf8(tmp1);
	for(int j=0;j<(int)word.size();)
	{
		utf8.ReadOneCharacter(tmp1);
    	if(lowercase.find(tmp1)!=string::npos)
		{
			tmp1="";
			break;

		}
		j+= (int)tmp1.size();
	}
	if(tmp1=="")
	{
		Utf8Stream Utf8(tmp2);
		for(int k=0;k<(int)word.size();)
		{
			
			Utf8.ReadOneCharacter(tmp2);
			if(tmp2=="/")
			{
				tmp1+=word.substr(k,word.size());
				k=(int)word.size();
				break;
			}
			else if(lowercase_map.find(tmp2)!=lowercase_map.end())
			{
				tmp1 += lowercase_map[tmp2];
				k+=(int)tmp2.size();
			}
			else
			{
				tmp1+=tmp2;
				k+=(int)tmp2.size();
			}
		}
	}
	else
		tmp1=word;
	return tmp1;
}

bool COldLadConv::isLattinString(string& letter)
{

	//FCL ADD LOCK
	//MutexLockGuard lock_gurad(m_mutex_lock);

	bool allUpper = true;
	int i=0;
	while(i<letter.size())
	{
		bool find = false;
		int j = letter.size();
		for(;j>i;j--)
		{
			string sub = letter.substr(i,j-i);

			//cout << i << "," << j << " curr_sub = [" << sub << "]" << endl;

			if( lettersDictToArabic.find(sub) != lettersDictToArabic.end() ||
				vowelDictToArabic.find(sub) != vowelDictToArabic.end() ||
				lettersDict.find(sub) != lettersDict.end() )
			{
				if(lettersDictToArabic.find(sub) != lettersDictToArabic.end())
					allUpper = false;
				find = true;
				break;
			}else
			{
				//cout << i << "," << j << " curr_sub not find = [" << sub << "]" << endl;
			}
		}
		if(find) i = j;
		else return false;
	}
	return allUpper == false;
}
set<string> COldLadConv::Lad2Old_new(const string &word)
{
	//lout << "first for beg." << endl;
	vector<vector<string> > vec;
	string strTemp;
	vec.push_back(vector<string>());
	vec.push_back(vector<string>());
	for(int i=0;i<(int)word.size();)
	{
        if(word[i]=='s' && word[i+1]=='h')
		{
            vec[0].push_back(strTemp);
			vec[1].push_back(strTemp);
			strTemp="";
			vec[0].push_back("sh");
			vec[1].push_back("s'h");
			i=i+2;
			
		}
		else if(word[i]=='z' && word[i+1]=='h')
		{

			vec[0].push_back(strTemp);
			vec[1].push_back(strTemp);
			strTemp="";
			vec[0].push_back("zh");
			vec[1].push_back("z'h");
			i=i+2;
			
		}
		else if(word[i]=='g' && word[i+1]=='h')
		{
            vec[0].push_back(strTemp);
			vec[1].push_back(strTemp);
			strTemp="";
			vec[0].push_back("gh");
			vec[1].push_back("g'h");
			i=i+2;
			
		}
		else if(word[i]=='n' && word[i+1]=='g')
		{
            vec[0].push_back("ng");
			vec[1].push_back("n'g");
			i=i+2;
		}
		else
		{
			strTemp+=word[i];
			i++;
		}
        
	}
	if(strTemp!="")
	{
        vec[0].push_back(strTemp);
		vec[1].push_back(strTemp);
		strTemp="";
	}

	//lout << "first for end." << endl;
	int count=1;
	for(int k=0;k<(int)vec[0].size();k++)
	{
			count=count*2;
	}

	//lout << "count = " << count << "  vec[0].size = " << vec[0].size() << endl;

	//vec_str.resize(count);
    int size=(int)vec[0].size();
	int indicator;
	set<string> strset;

	//lout << "sec for beg." << endl;
	for(int indicator=0; indicator < count ; indicator ++)
    {
		indicator = indicator & (count-1);
		string s="";
		for(int i=0;i<(int)vec[0].size();i++)
		{
		  if(fun(i) == (fun(i) & indicator)) 
			 s+=vec[1][i];
		  else
			 s+=vec[0][i];
		}
		string upstr="";
		if(s[0]>='a' && s[0]<='z')
		{
			upstr+=uppercase_map[s.substr(0,1)];
			upstr+=s.substr(1,s.size());
		}
		else
		{
            upstr+=uppercase_map[s.substr(0,2)];
			upstr+=s.substr(2,s.size());
		}
		strset.insert(Lad2Old(s));
		strset.insert(Lad2Old(upstr));
    }

	//lout << "sec for end." << endl;
	return strset;
}
int COldLadConv::fun(int m)
{
	long count=1;
	if(m==0)count=1;
	else
	{
	for(int i=0;i<m;i++)
		count=2*count;
	}
	return count;
}
//void COldLadConv::tokenNotStdLad(const string& word, string& output)
//{
//	output = "";
//   
//	int i = 0;
//	while(i < (int)(word1.size())
//	{
//		if(i == 0 && // 文件头<BOM>的过滤
//			(unsigned char)word1[i] == 0xEF && 
//			(unsigned char)word1[i+1] == 0xBB && 
//			(unsigned char)word1[i+2] == 0xBF)
//		{
//			i += 3;
//		}
//		else if(word[i]=='/')
//		{
//			output+=word.substr(i,word.size());
//			//i=j;
//			i+=(int)word.size()-i;
//		}
//		else if((unsigned char)word[i]== 0x43 ||
//			(unsigned char)word[i] == 0x63 )
//		{
//            /*Utf8.ReadOneCharacter(word1);
//			output+=word1;
//            Utf8.ReadOneCharacter(word1);
//			output+=word1;*/
//			output += word.substr(i,2);
//			i += 2;
//		}
//		else if(word[i] < 0) //GBK编码或者是符号
//		{
//			/*Utf8.ReadOneCharacter(word1);
//			output+=word1;
//			i+=word1.size();*/
//			output += word.substr(i,2);
//			i += 2;
//		}
//		else	// 其他ASCII码
//		{
//			/*Utf8.ReadOneCharacter(word1);
//			output+=word1;*/
//			output += word.substr(i,1);
//			i += 1;
//		}
//		output += " ";
//	}
//}
//string COldLadConv::Replace(string&   str,const   string&   old_value,const   string&   new_value)
//	{
//		for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   
//	    {      
//		   if(   (pos=str.find(old_value,pos))!=string::npos   )      
//			  str.replace(pos,old_value.length(),new_value);      
//		   else   break;      
//	    }      
//	    return   str;
//	}



