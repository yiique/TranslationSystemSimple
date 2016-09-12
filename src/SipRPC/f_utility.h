/**
* Copyright (c) 2008, ������Ϣ�ص�ʵ����,�й���ѧԺ���㼼���о���
* All rights reserved.
*
* �ļ�����f_utility.h
* @brief ���ú���
* @project ToWin���߷���ƽ̨
*
* @comment 
* @version 0.5
* @author ������
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

/* Function  ͨ����β��ǽ�ȡ�ַ�������
*  Since     v0.5
*  Concurrency  true */
bool str_tag_find(const string &src, const string &beg_tag, const string &end_tag, string &content);

/* Function  �ַ���ת��Ϊ����
*  Since     v0.5
*  Concurrency  true */
bool cstr_2_num(const char *cstr, int &num);
int  str_2_num(const string & str);

/* Function  ����ת��Ϊ�ַ���
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


/* Function  ȥ���ַ������пո�
*  Since     v0.5
*  Concurrency  true */
void del_str_blank(string &str);

/* Function  �������еĴʰ��ո��зֶ�ȡ��һ��vector  ������������Ŀո�
*  Since     v0.5
*  Concurrency  true */
bool split_string_by_blank(const char * src, vector<string> & tgt_vec);

/* Function  �������еĴʰ��ַ�tag�зֶ�ȡ��һ��vector  �������������tag
*  Since     v0.5
*  Concurrency  true */
bool split_string_by_tag(const char * src, vector<string> & tgt_vec, const char tag);

/* Function  ���ַ����а��ַ�tag�зֶ�ȡ��һ��vector  
			 �������������tag  �ϸ��з�  
			 tag��Ϊ�ָ���׺ ��tag = ' 'ʱ  src = "a b" ���ָ�Ϊ��"a"��"b"; src = "a  " ���ָ�Ϊ��"a"��"" 
*  Since     v0.5
*  Concurrency  true */

bool split_sequence_by_tag(const char * src, vector<string> & tgt_vec, const char tag);

/* Function  ��vector���浽һ��string�������ո�ָ�
*  Since     v0.5
*  Concurrency  true */
bool merge_strvec_to_str(const vector<string> & str_vec, string & tgt);

//ȥ����β�ո�
string & del_head_tail_blank(string & str);
//ȥ����β�Ļ��з��Ϳո�
string & filter_head_tail(string & str);

//��������еĴ���
size_t count_words(const string & src);

//���Ҳ��滻 ���ر��滻�Ĵ���
size_t find_and_replace(string & src, const string & from_s, const string & to_s);

//����һ��UUID
string GenerateUUID();

#endif
