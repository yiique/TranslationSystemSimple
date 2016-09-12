/**
* Copyright (c) 2008, 智能信息重点实验室,中国科学院计算技术研究所
* All rights reserved.
*
* 文件名：f_utility.h
* @brief 共用函数
* @project ToWin在线翻译平台
*
* @comment 
* @version 0.5
* @author 傅春霖
* @date 2009-11-30
* @
*/


#ifndef F_UTILITY_H
#define F_UTILITY_H

//BOOST
#include <boost/uuid/uuid.hpp>  
#include <boost/uuid/uuid_generators.hpp>  
#include <boost/uuid/uuid_io.hpp> 

//STL
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

/* Function  通过首尾标记截取字符串内容
*  Since     v0.5
*  Concurrency  true */
bool str_tag_find(const string &src, const string &beg_tag, const string &end_tag, string &content);

/* Function  字符串转换为数字
*  Since     v0.5
*  Concurrency  true */
bool cstr_2_num(const char *cstr, int &num);
int  str_2_num(const string & str);

/* Function  数字转换为字符串
*  Since     v0.5
*  Concurrency  true */
template <typename num_t>
string num_2_str(const num_t num)
{
	string s_num;
	stringstream ss;
	ss << num;
	ss >> s_num;

	return s_num;
}


/* Function  去掉字符串所有空格
*  Since     v0.5
*  Concurrency  true */
void del_str_blank(string &str);

/* Function  将句子中的词按空格切分读取到一个vector  将会忽略连续的空格
*  Since     v0.5
*  Concurrency  true */
bool split_string_by_blank(const char * src, vector<string> & tgt_vec);

/* Function  将句子中的词按字符tag切分读取到一个vector  将会忽略连续的tag
*  Since     v0.5
*  Concurrency  true */
bool split_string_by_tag(const char * src, vector<string> & tgt_vec, const char tag);

/* Function  将字符序列按字符tag切分读取到一个vector  
			 不会忽略连续的tag  严格切分  
			 tag作为分隔后缀 如tag = ' '时  src = "a b" 被分隔为："a"和"b"; src = "a  " 被分割为："a"和"" 
*  Since     v0.5
*  Concurrency  true */

bool split_sequence_by_tag(const char * src, vector<string> & tgt_vec, const char tag);

/* Function  将vector储存到一个string，并按空格分隔
*  Since     v0.5
*  Concurrency  true */
bool merge_strvec_to_str(const vector<string> & str_vec, string & tgt);

//去掉首尾空格
string & del_head_tail_blank(string & str);
//去掉首尾的换行符和空格
string & filter_head_tail(string & str);

//计算句子中的词数
size_t count_words(const string & src);

//查找并替换 返回被替换的次数
size_t find_and_replace(string & src, const string & from_s, const string & to_s);

//产生一个UUID
string GenerateUUID();

#endif
