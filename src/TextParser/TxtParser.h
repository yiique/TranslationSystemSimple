#ifndef TXT_PARSER_H
#define TXT_PARSER_H


//STL
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <utility>
using namespace std;



class TxtParser
{
public:
	TxtParser(void);
	~TxtParser(void);

	bool Parse(const string & src,
			   vector<string> & para_vec, 
			   vector<pair<size_t,size_t> > & pos_vec);


private:

	//返回false表示读取到结尾
	bool get_line(const string & src, 
				  const size_t offset, 
				  pair<size_t, size_t> & line_pos);

	//返回true表示此行有实际内容（跳过开头的空格和TAB）
	bool parse_line(const string & src, 
					const pair<size_t, size_t> & line_pos, 
					pair<size_t, size_t> & content_pos);


	size_t ignore_flag(const string & src, const size_t offset);
};

#endif //TXT_PARSER_H

