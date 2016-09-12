#pragma once
#undef min
#undef max
#include <vector>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <cmath>
#include <limits>

using namespace std;

inline string trim(const string &s)
{
	string t = "";
	size_t beg = s.find_first_not_of(" \t\n\r");

	if(beg != string::npos)
	{
		size_t end = s.find_last_not_of(" \t\n\r");
		t = s.substr(beg,end+1-beg);
	}
	return t;
}

inline bool isInt(float f)
{
	float t;
	float x = modf(f,&t);
	if(fabs(x)<numeric_limits<float>::epsilon())
		return true;
	else
		return false;

}

inline bool isChineseChar(const string & str)//仅仅判断知不是中文字符，包括半角符号
{
	string w= str;
	w = trim(w);
	if(w.empty())
		return false;
	if(w[0]>0)
		return true;
	if ((unsigned char)w[0]==0xE4 ||
		(unsigned char)w[0]==0xE5 ||
		(unsigned char)w[0]==0xE6 ||
		(unsigned char)w[0]==0xE7 ||
		(unsigned char)w[0]==0xE8 ||
		(unsigned char)w[0]==0xE9 )//汉字
		return true;

	return false;
}
//
//空格或者空串不是中文;也算是中文
inline bool isChineseUtf8(const string & str)
{
	string w= str;
	w = trim(w);
	if(w.empty())
		return false;
	if(w[0]>0)
		return false;
	if ((unsigned char)w[0]==0xE4 ||
			(unsigned char)w[0]==0xE5 ||
			(unsigned char)w[0]==0xE6 ||
			(unsigned char)w[0]==0xE7 ||
			(unsigned char)w[0]==0xE8 ||
			(unsigned char)w[0]==0xE9 )//汉字
			return true;
	if(w.size()>=3)
	{
		if(((unsigned char)w[0]==0xEF &&(unsigned char)w[1]==0xBC && (unsigned char)w[2]==0x9F)|| //？
			((unsigned char)w[0]==0xE3 &&(unsigned char)w[1]==0x80 && (unsigned char)w[2]==0x82) || //。
			((unsigned char)w[0]==0xEF &&(unsigned char)w[1]==0xBC && (unsigned char)w[2]==0x9A) || //：
			((unsigned char)w[0]==0xEF &&(unsigned char)w[1]==0xBC && (unsigned char)w[2]==0x9B) || //；
			((unsigned char)w[0]==0xE2 &&(unsigned char)w[1]==0x80 && (unsigned char)w[2]==0x9C) || //“
			((unsigned char)w[0]==0xE2 &&(unsigned char)w[1]==0x80 && (unsigned char)w[2]==0x9D) || //”
			((unsigned char)w[0]==0xE3 &&(unsigned char)w[1]==0x80 && (unsigned char)w[2]==0x8A) || //《
			((unsigned char)w[0]==0xE3 &&(unsigned char)w[1]==0x80 && (unsigned char)w[2]==0x8B) || //》
			((unsigned char)w[0]==0xEF &&(unsigned char)w[1]==0xBC && (unsigned char)w[2]==0x81) || //！
			((unsigned char)w[0]==0xE3 &&(unsigned char)w[1]==0x80 && (unsigned char)w[2]==0x81) || //、
			((unsigned char)w[0]==0xEF &&(unsigned char)w[1]==0xBC && (unsigned char)w[2]==0x88) || //（
			((unsigned char)w[0]==0xEF &&(unsigned char)w[1]==0xBC && (unsigned char)w[2]==0x89) || //）
			((unsigned char)w[0]==0xE2 &&(unsigned char)w[1]==0x80 && (unsigned char)w[2]==0xA2) ||//?            
			((unsigned char)w[0]==0xE2 &&(unsigned char)w[1]==0x80 && (unsigned char)w[2]==0xB0))//?
			return true;
	}

	return false;
}

template <typename T>
inline T Scan(string &s)
{
	stringstream ss;
	ss<<s;
	T rst;
	ss>>rst;
	return rst;
}

inline string Int2Str(int s)
{
	stringstream ss;
	ss<<s;
	string rst;
	ss>>rst;
	return rst;
}
//
//void String2Vector(const string &src,vector<string>& v)
//{
//	stringstream ss;
//	string part;
//	ss<<src;
//	while(ss >>part)
//	{
//		v.push_back(part);
//	}
//}
//
//void Vector2StringWithSpace(const vector<string> &v,string & src)
//{
//	src = "";
//	vector<string>::const_iterator it=v.begin();
//	for(;it!=v.end();++it)
//	{
//		src +=(*it)+" ";
//	}
//	src.erase(src.size()-1,1);
//}

inline vector<string> splitByStr(const string &src,const string &sep)
{
	vector<string> v;
	size_t cur,last=0;
	while((cur=src.find(sep,last))!=string::npos)
	{
		//string tmp = src.substr(last,cur-last);
		if(cur > last)
			v.push_back(src.substr(last,cur-last));
		last = cur +sep.size();
	}
	if(last < src.size())
		v.push_back(src.substr(last));

	return v;
}

inline void replace(const string &src,const string &alter,const string &part,string &tgt)
{
	size_t beg=0,end=0;
	
	while((beg = src.find(alter,end))!=string::npos)
	{
		tgt +=src.substr(end,beg-end);
		tgt +=part;
		end = beg + alter.size();
	}
	if(end<src.size())
		tgt +=src.substr(end);
}
//
inline void replace(string &tgt,const string &alter,const string &part)//用part替换alter
{
	size_t beg=0,end=0;
	string src = tgt;
	tgt="";
	
	while((beg = src.find(alter,end))!=string::npos)
	{
		tgt +=src.substr(end,beg-end);
		tgt +=part;
		end = beg + alter.size();
	}
	if(end<src.size())
		tgt +=src.substr(end);
}
//
//void readPage(const string& fname, string& pageContent)
//{
//	//ifstream in(fname.c_str(),ios_base::binary);
//	ifstream in(fname.c_str());
//	if ( in==NULL )
//	{
//		return ;
//	}
//	in.seekg( 0, ios::end );   
//	int len = in.tellg();   
//	in.seekg( 0, ios::beg );   
//	char* str = new char[len+1]; 
//	memset( str, 0, len+1 );
//	in.read( str, len );//将流in的前len个字符读到str中；
//	pageContent = str;
//	delete []str;
//	in.close();
//}
//
//string Remove(string&   str,const   string&   old_value)
//{
//	if(old_value.empty())
//		return str;
//
//	string tgt = str;
//
//	int lenRep = old_value.size();
//
//	size_t i=0,j=0;
//	while((i = tgt.find(old_value,j))!=string::npos)
//	{
//		tgt.erase(i,lenRep);
//		j = i;
//	}
//
//	return tgt;
//}
//
//
//string   Replace(string&   str,const   string&   old_value,const   string&   new_value)   
//{ 
//	/*
//	while(true)   
//	{   
//	string::size_type   pos(0);   
//	if((pos=str.find(old_value))!=string::npos)  
//	str.replace(pos,old_value.length(),new_value);   
//	else   
//	break;   
//	}   
//	return   str;   */
//	for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   
//	{      
//		if(   (pos=str.find(old_value,pos))!=string::npos   )      
//			str.replace(pos,old_value.length(),new_value);      
//		else   break;      
//	}      
//	return   str;      
//}  
