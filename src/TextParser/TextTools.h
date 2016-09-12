#ifndef TEXT_TOOLS_H
#define TEXT_TOOLS_H

#include <utility>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

//�ж��Ƿ���Ӣ���ı�
//����ֵ�� TRUE Ӣ�ģ� FALSE ���������ģ�
bool IsEnglishText(const string & src);

//����position���������ԭ�ı�
bool RevertTextWithPos(const string & src,
					   const vector<string> & tgt_vec, 
					   const vector<pair<size_t,size_t> > & pos_vec,
					   string & tgt);


#endif //TEXT_TOOLS_H
