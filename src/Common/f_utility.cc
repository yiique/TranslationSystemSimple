#include "f_utility.h"


bool str_tag_find(const string &src, const string &beg_tag, const string &end_tag, string &content)
{

	//找出头标记
	size_t beg_pos = src.find(beg_tag);

	if( beg_pos == string::npos )
    {
		cerr << "ERROR: str_tag_find() find beg_tag failed. Beg Tag = " << beg_tag << endl;
		return false;
    }
	
	beg_pos += beg_tag.size();

	//找出尾标记
	size_t end_pos = src.find(end_tag, beg_pos);

	if( end_pos == string::npos )
    {
		cerr << "ERROR: str_tag_find() find end_tag failed. End Tag = " << end_tag << endl;
		return false;
    }

	//截取
	content.reserve(end_pos-beg_pos);
	content = src.substr(beg_pos, end_pos-beg_pos);
	return true;
}

bool cstr_2_num(const char *cstr, int &num)
{
	if( !cstr )
		return false;

	stringstream ss;
	ss << cstr;
	ss >> num;

	return true;
}

int str_2_num(const string & str)
{
	int num = -1;

	cstr_2_num(str.c_str(), num);
	
	return num;
}


bool split_string_by_blank(const char * src, vector<string> & tgt_vec)
{
	return split_string_by_tag(src, tgt_vec, ' ');
}

bool split_string_by_tag(const char * src, vector<string> & tgt_vec, const char tag)
{
	if(!src)
	{
		return false;
	}

	tgt_vec.clear();

	char c = *src;
	size_t idx = 0;
	string curr_str;

	while(c != '\0')
	{
		if( tag == c )
		{
			if(curr_str.size() != 0)
			{
				tgt_vec.push_back(curr_str);
				curr_str.clear();
			}
		}else
		{
			curr_str += c;
		}

		c = src[++idx];
	}

	if(curr_str.size() != 0)
	{
		tgt_vec.push_back(curr_str);
	}

	return true;

}

bool split_sequence_by_tag(const char * src, vector<string> & tgt_vec, const char tag)
{
	if(!src)
	{
		return false;
	}

	tgt_vec.clear();

	char c = *src;
	size_t idx = 0;
	string curr_str;

	while(c != '\0')
	{
		if( tag == c )
		{	
			tgt_vec.push_back(curr_str);
			curr_str.clear();
		}else
		{
			curr_str += c;
		}

		c = src[++idx];
	}
  
  if(curr_str.size() > 0)
  {
  	tgt_vec.push_back(curr_str);
  }else
  {
  	if(idx > 0 && src[idx-1] == tag)
  		tgt_vec.push_back(curr_str);
  	else if(idx == 0)
  		tgt_vec.push_back(curr_str);
  }
  
		

	return true;
}


bool merge_strvec_to_str(const vector<string> & str_vec, string & tgt)
{
	tgt.clear();

	for(size_t i=0; i<str_vec.size(); ++i)
	{
		tgt += str_vec[i];

		if(i != str_vec.size()-1)
			tgt += " ";
	}

	return true;
}

void del_str_blank(string & str)
{
	string tmp_str;

	for(size_t i=0; i<str.size(); ++i)
	{
		if( ' ' != str[i])
			tmp_str += str[i];
	}

	if(tmp_str.size() < str.size())
		str = tmp_str;
}


string & del_head_tail_blank(string & str)
{
	if(str.size() == 0)
		return str;


	size_t head = 0;
	size_t tail = str.size();  //最后一个字符的后一位

	//找到首部
	for(head=0; head < str.size(); ++head)
	{
		if(str[head] != ' ')
			break;
	}

	//找到尾部
	for(tail=str.size(); tail > 0; --tail)
	{
		if(str[tail-1] != ' ')
			break;
	}

	if(tail <= head) //说明全是空格
		return (str = "");
	
	return (str = str.substr(head, tail-head));
}

string & filter_head_tail(string & str)
{
	if(str.size() == 0)
		return str;

	size_t head = 0;
	size_t tail = str.size(); //最后一个字符的后一位

	//找到首部
	for(head=0; head < str.size(); ++head)
	{
		if(    str[head] != ' '
			&& str[head] != '\r'
			&& str[head] != '\n')
			break;
	}

	//找到尾部
	for(tail=str.size(); tail > 0; --tail)
	{
		if(    str[tail-1] != ' '
			&& str[tail-1] != '\r'
			&& str[tail-1] != '\n')
			break;
	}

	if(tail <= head) //说明全是空格回车和换行
		return (str = "");
	
	return (str = str.substr(head, tail-head));
}


size_t count_words(const string & src)
{
	if(src.size() == 0)
		return 0;

	size_t words_num = 0;

	char c = src[0];
	size_t idx = 0;
	bool is_word = false;

	while(c != '\0')
	{
		if( ' ' == c )
		{
			if(true == is_word)
			{
				++words_num;
				is_word = false;
			}
		}else
		{
			is_word = true;
		}

		c = src[++idx];
	}

	if(true == is_word)
	{
		++words_num;
	}

	return words_num;

}

string GenerateUUID()
{
	boost::uuids::random_generator rgen;  
    boost::uuids::uuid ranUUID = rgen();

	stringstream ss;
	ss << ranUUID;

	return ss.str();
}


size_t find_and_replace(string & src, const string & from_s, const string & to_s)
{
    if(from_s.size() == 0)
        return 0;

	size_t r_cnt = 0;

	size_t beg = 0;
	string tgt;

	while( true )
	{
		size_t pos = src.find(from_s, beg);

		if(string::npos == pos)
		{
			if(r_cnt != 0)
			{
				tgt += src.substr(beg, src.size()-beg);
				src = tgt;
			}
			return r_cnt;
		}else
		{
			++r_cnt;
			tgt += src.substr(beg, pos-beg);
			tgt += to_s;
			beg = pos + from_s.size();

			if(beg >= src.size())
			{
				src = tgt;
				return r_cnt;
			}
		}
	}


}
