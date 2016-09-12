#ifndef UyToken_H
#define UyToken_H

#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <iterator>
#include <iostream>
#include <map>
#include "Common/Utf8Stream.h"

using namespace std;

class UigurTokenizer
{
public:
	UigurTokenizer(void)
	{
		// 单字节
		m_token.insert(",");
		m_token.insert("!");
		m_token.insert("?");
		m_token.insert(";");
		m_token.insert(":");
		m_token.insert("\"");
		m_token.insert(")");
		m_token.insert("(");
		m_token.insert("/");
		m_token.insert("&");
		m_token.insert("^");
		m_token.insert("[");
		m_token.insert("]");
		m_token.insert("{");
		m_token.insert("}");
		m_token.insert("-");
		m_token.insert(".");
		m_token.insert("%");
		m_token.insert("_");
		m_token.insert("~");
		// 多字节
		//此处添加多字节符号，形式如下：
		const char t1[4]={0xE2,0x80,0xA6,0};//...
		const char t2[4]={0xE3,0x80,0x8A,0};//《
		const char t3[4]={0xE3,0x80,0x8B,0};//》
		const char t4[3]={0xC2,0xAB,0};//《小的
		const char t5[3]={0xC2,0xBB,0};//》小的
		const char t6[4]={0xE2,0x80,0x98,0};//'左
		const char t7[4]={0xE2,0x80,0x99,0};//'右
		const char t8[] ={0xE2,0x80,0x9C,0};//“
		const char t9[] ={0xE2,0x80,0x9D,0};//”
		m_token.insert(t1);
		m_token.insert(t2);
		m_token.insert(t3);
		m_token.insert(t4);
		m_token.insert(t5);
		m_token.insert(t6);
 		m_token.insert(t7);
		m_token.insert(t8);
		m_token.insert(t9);

		//Added by wzy
		//继续多字节
		char w0[]={0xE2,0x91,0xA0,0};//①
		char w1[]={0xE2,0x91,0xA1,0};
		char w2[]={0xE2,0x91,0xA2,0};
		char w3[]={0xE2,0x91,0xA3,0};
		char w4[]={0xE2,0x91,0xA4,0};
		char w5[]={0xE2,0x91,0xA5,0};
		char w6[]={0xE2,0x91,0xA6,0};
		char w7[]={0xE2,0x91,0xA7,0};
		char w8[]={0xE2,0x91,0xA8,0};
		char w9[]={0xE2,0x91,0xA9,0};
		char w10[]={0xE2,0x91,0xAA,0};
		char w11[]={0xE2,0x91,0xAB,0};
		char w12[]={0xE2,0x91,0xAC,0};
		char w13[]={0xE2,0x91,0xAD,0};
		char w14[]={0xE2,0x91,0xAE,0};
		char w15[]={0xE2,0x91,0xAF,0};

		char w16[]={0xE3,0x80,0x90,0};//【
		char w17[]={0xE3,0x80,0x91,0};//】


		char w18[]={0xE3,0x80,0x94,0};//〔
		char w19[]={0xE3,0x80,0x95,0};//〕

		char w20[]={0xEF,0xBC,0x8C,0};//，
		char w21[]={0xE3,0x80,0x82,0};//。
		char w22[]={0xEF,0xBC,0x9B,0};//；
		char w23[]={0xEF,0xBC,0x81,0};//！
		char w24[]={0xEF,0xBC,0x9F,0};//？
		char w25[]={0xE2,0x80,0xB9,0};//<
		char w26[]={0xE2,0x80,0xBA,0};//<

		m_token.insert(w0);
		m_token.insert(w1);
		m_token.insert(w2);
		m_token.insert(w3);
		m_token.insert(w4);
		m_token.insert(w5);
		m_token.insert(w6);
		m_token.insert(w7);
		m_token.insert(w8);
		m_token.insert(w9);
		m_token.insert(w10);
		m_token.insert(w11);
		m_token.insert(w12);
		m_token.insert(w13);
		m_token.insert(w14);
		m_token.insert(w15);
		m_token.insert(w16);
		m_token.insert(w17);
		m_token.insert(w18);
		m_token.insert(w19);
		m_token.insert(w20);
		m_token.insert(w21);
		m_token.insert(w22);
		m_token.insert(w23);
		m_token.insert(w24);
		m_token.insert(w25);
		m_token.insert(w26);
		//end by wzy


		// 要删除的
		m_del.insert("'");
		// 多字节
		char d1[] = {0xE2, 0x80, 0x8F, 0};//一个不可见;
		char d2[] = {0xE3, 0x80, 0x80, 0};//utf8不可见
		m_del.insert(t7);
		m_del.insert(d1);
		m_del.insert(d2);
	}

	bool TokenLatin(const string& src, string& tgt)
	{
		vector<string> vChar = ToUtfVecChar(src);
		stringstream sstream;
		for (unsigned int i = 0; i < vChar.size(); i++)
		{
			string left = (i == 0) ? "" : vChar[i-1];
			string right = (i + 1 >= vChar.size()) ? "" : vChar[i+1];

			string rep = inReplace(vChar[i], left, right);
			if (rep != "")
				vChar[i] = rep;

			if (inDel(vChar[i], left, right) == true)
				continue;

			TokenType type = inToken(vChar[i], left, right);

			if (type == BOTH)
				sstream << " " << vChar[i] << " ";
			else if (type == LEFT)
				sstream << " " << vChar[i];
			else if (type == RIGHT)
				sstream << vChar[i] << " ";
			else
				sstream << vChar[i];
		}
		tgt = sstream.str();
		return true;
	}
public:
	enum TokenType
	{
		LEFT,
		RIGHT,
		BOTH,
		NONE
	};
private:
	TokenType inToken(const string& str, const string& left = "", const string& right = "")
	{
		// 处理数字后是其他东西的向右token
		if (_num(str) == true && !(right == "" || right == " " || _num(right) == true || right == "." || right == "-") )
			return RIGHT;

		// 处理小数点
 		if (str == ".")
 			if (_num(left) == true && _num(right) == true)
 				return NONE;

		if (str == "-")
			if (_num(left) == true && _num(right) == true)
				return NONE;

		if (m_token.find(str) != m_token.end())
		{
			if (left == "" && right == "")
				return NONE;
			else if ((left == "" || left == " ") && (right != " " || right != ""))
				return RIGHT;
			else if ((right == "" || right == " ") && (left != " " || left != ""))
				return LEFT;
			else
				return BOTH;
		}
		else
			return NONE;
	}

	bool inDel(const string& str, const string& left = "", const string& right = "")
	{
		char t7[4]={0xE2,0x80,0x99,0};
		if (str == t7 && (left == " " || right == " "))
			return false;
		if (str == "'" && (left == " " || right == " "))
			return false;

		return m_del.find(str) != m_del.end();
	}

	string inReplace(const string& str, const string& left = "", const string& right = "")
	{
		char r1[] = {0xE2,0x80,0x93,0};//-
		char r2[] = {0xD9, 0x80, 0};//_
		const char r_o[] = {0xC3, 0x96, 0};
		const char r_u[] = {0xC3, 0x9C, 0};
		const char r_e[] = {0xC3, 0x8B, 0};
		const char t_o[] = {0xC3, 0XB6, 0};
		const char t_u[] = {0xC3, 0xBC, 0};
		const char t_e[] = {0xC3, 0xAB, 0};

		if (str == r1)
			return "-";
		else if (str == r2)
			return "_";
		else if (str == r_o)
			return t_o;
		else if (str == r_u)
			return t_u;
		else if (str == r_e)
			return t_e;

		return "";
	}
public:
	static vector<string> ToUtfVecChar(const string& str)
	{
		Utf8Stream utfStream(str);
		string temp;
		vector<string> vChar;
		while (utfStream.ReadOneCharacter(temp))
		{
			vChar.push_back(temp);
		}
		return vChar;
	}
private:
	bool _num(const string& word)
	{
		if (word.size() != 1)
			return false;
		if ((unsigned)word[0] < 48 || (unsigned)word[0] > 57)
			return false;
		return true;
	}
private:
	set<string> m_token;
	set<string> m_del;
};

class UyLower
{
public:
	static string to_lower(const string& str)
	{
		static const char __O[] = {0xC3, 0x96, 0};
		static const char __o[] = {0xC3, 0xB6, 0};
		static const char __U[] = {0xC3, 0x9C, 0};
		static const char __u[] = {0xC3, 0xBC, 0};
		static const char __E[] = {0xC3, 0x8B, 0};
		static const char __e[] = {0xC3, 0xAB, 0};
		static const string O(__O);
		static const string o(__o);
		static const string U(__U);
		static const string u(__u);
		static const string E(__E);
		static const string e(__e);
		static const string vStr[6] = {O,o,U,u,E,e};
		vector<string> vChar = UigurTokenizer::ToUtfVecChar(str);
		for (unsigned int i = 0; i < vChar.size(); i++)
		{
			if (vChar[i].size() == 1 && (vChar[i].at(0) >=65 && vChar[i].at(0) <= 90))
				vChar[i].at(0) += 32;
			else if (vChar[i].size() != 1)
			{
				if (vChar[i] == vStr[0])
					vChar[i] = vStr[1];
				else if (vChar[i] == vStr[2])
					vChar[i] = vStr[3];
				else if (vChar[i] == vStr[4])
					vChar[i] = vStr[5];
			}
		}
		stringstream sstream;
		copy(vChar.begin(), vChar.end(), ostream_iterator<string>(sstream, ""));
		return sstream.str();
	}
};

#endif
