#include "SentSpliter.h"

using namespace ssplite;




SentSpliter::SentSpliter(const string & token_src, 
						 const bool is_add_miss_end_punc,
						 const size_t max_word_num, 
						 const size_t min_word_num,
						 const PuncResource & punc_resource): 
						m_is_add_miss_end_punc(is_add_miss_end_punc),
						m_max_word_num(max_word_num), 
						m_min_word_num(min_word_num),
						m_punc_resource(punc_resource),
						m_is_finish(false)
{
	string tmp(token_src);
	filter_head_tail(tmp);
	split_string_by_blank(tmp, m_src_word_vec);
}

string & SentSpliter::filter_head_tail(string & str)
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
	for(tail=str.size(); tail >= 0; --tail)
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


void SentSpliter::split_string_by_blank(const string & src, vector<string> & tgt_vec)
{
	split_string_by_tag(src, tgt_vec, ' ');
	return;
}

void SentSpliter::split_string_by_tag(const string & src, vector<string> & tgt_vec, const char tag)
{
	tgt_vec.clear();

	if(src.size() == 0) 
	{
		tgt_vec.push_back("");
		return ;
	}

	
	size_t idx = 0;
	string curr_str;

	while(idx < src.size())
	{
		char c = src[idx];

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

		++idx;
	}

	if(curr_str.size() != 0)
	{
		tgt_vec.push_back(curr_str);
	}

	return ;

}

const vector<string> & SentSpliter::GetDebugResult(void)
{
	if( true == m_is_finish )
		return m_splite_result;

	vector<pair<size_t, size_t> > splite_pos_vec;
	SentStream sent_stream(m_max_word_num, m_min_word_num, splite_pos_vec);

	//生成标点符号序列
	init_block_seq(m_src_word_vec);

	//划分子句
	splite_sub_sent(0, m_block_vec.size(), sent_stream);

	sent_stream.Flush();

	//生成debug结果
	for(size_t i=0; i<splite_pos_vec.size(); ++i)
	{
		m_splite_result.push_back("");

		for(size_t k=splite_pos_vec[i].first; k<splite_pos_vec[i].second; ++k)
		{
			if(k != splite_pos_vec[i].first)
				*(m_splite_result.rbegin()) += "";
		
			string debug_str = get_block_name(k);

			size_t pos = debug_str.find("/");

			if(string::npos != pos)
			{
				debug_str = debug_str.substr(0, pos);
			}

			*(m_splite_result.rbegin()) += debug_str;
		}
	}


	m_is_finish = true;
	return m_splite_result;
}

const vector<string> & SentSpliter::GetSpliteResult(void)
{
	
	if( true == m_is_finish )
		return m_splite_result;

	vector<pair<size_t, size_t> > splite_pos_vec;
	SentStream sent_stream(m_max_word_num, m_min_word_num, splite_pos_vec);

	//生成标点符号序列
	init_block_seq(m_src_word_vec);

	//划分子句
	splite_sub_sent(0, m_block_vec.size(), sent_stream);

	sent_stream.Flush();

	//生成结果
	for(size_t i=0; i<splite_pos_vec.size(); ++i)
	{
		m_splite_result.push_back("");

		for(size_t k=splite_pos_vec[i].first; k<splite_pos_vec[i].second; ++k)
		{
			if(k != splite_pos_vec[i].first)
				*(m_splite_result.rbegin()) += " ";
		
			*(m_splite_result.rbegin()) += get_block_content(k);
		}
	}

	m_is_finish = true;
	return m_splite_result;
}


bool SentSpliter::find_last_punc(const size_t beg, const size_t end, size_t & last_punc)
{
	assert(end <= m_block_vec.size() && beg <= end);

	if(beg >= end)
		return false;
	
	size_t idx = end - 1;
	while(idx >= beg)
	{
		if(get_block_type(idx) != TYPE_WORD)
		{
			last_punc = idx;
			return true;
		}

		if(beg == idx)
			return false;

		--idx;

	}

	return false;

}

bool SentSpliter::find_right_punc(const size_t beg, const size_t end, size_t & right_punc)
{
	assert(end <= m_block_vec.size() && beg <= end);

	if(beg >= end || (beg + 1) == end)
		return false;

	const size_t left_punc = beg;

	map<size_t, size_t>::const_iterator iter = m_left_right_map.find(left_punc);

	if(iter == m_left_right_map.end())
		return false;

	if(iter->second <= left_punc || iter->second >= end)
		return false;

	right_punc = iter->second;

	return true;

}



void SentSpliter::splite_sub_sent(const size_t beg, const size_t end, SentStream & sent_stream)
{
	assert(end <= m_block_vec.size() && beg <= end);

	if(beg >= end) return;

	size_t sub_sent_beg = beg;

	for(size_t i=beg; i<end; ++i)
	{
		if( get_block_type(i) == TYPE_WORD )
		{
			if(i - sub_sent_beg + 1 >= m_max_word_num)
			{
				//前面可能有引号
				size_t last_punc = 0;
				if( false == find_last_punc(sub_sent_beg, i+1, last_punc))
				{
					sent_stream.Append(sub_sent_beg, i+1, ATTR_SENT_NORMAL);

					sub_sent_beg = i+1;
				}else
				{
					sent_stream.Append(sub_sent_beg, last_punc+1, ATTR_SENT_NORMAL);
					sub_sent_beg = last_punc+1;
				}
			}

		}else
		{
			
			size_t right = 0;
			if( find_right_punc(i, end, right) ) //是配对的左标点
			{

				if(right - i + 1 > m_max_word_num)
				{
					//先输出前面的
					sent_stream.Flush(); //先清空buf
					sent_stream.Append(sub_sent_beg, i, get_subsent_attribute(sub_sent_beg, i));

					//左右标点单独成句，同时不能与前面的合并，因此要清空sentbuf

					//左标点
					sent_stream.Flush();
					sent_stream.Append(i, i+1, ATTR_SENT_TERMINAL);
					sent_stream.Flush();

					//中间部分递归切分
					splite_sub_sent(i+1, right, sent_stream);

					//右标点
					sent_stream.Flush();
					sent_stream.Append(right, right+1, ATTR_SENT_TERMINAL);
					sent_stream.Flush();

					sub_sent_beg = right + 1;
					

				}else
				{
					if(right - sub_sent_beg + 1 > m_max_word_num)
					{
						sent_stream.Flush();
						sent_stream.Append(sub_sent_beg, i, get_subsent_attribute(sub_sent_beg, i));
						sent_stream.Append(i, right+1, get_subsent_attribute(i, right+1));
						sub_sent_beg = right + 1;
					}else
					{
						if( ATTR_SENT_TERMINAL == get_subsent_attribute(i, right+1))
						{
							sent_stream.Append(sub_sent_beg, right+1, ATTR_SENT_TERMINAL);
							sub_sent_beg = right + 1;
						}else
						{
							//donothing
						}
					}
					
				}

				i = right;
				
			}else
			{
				//其他标点
				sent_stream.Append(sub_sent_beg, i+1, get_subsent_attribute(sub_sent_beg, i+1));
				sub_sent_beg = i+1;

			}
		}
	}

	if(sub_sent_beg < end)
		sent_stream.Append(sub_sent_beg, end, get_subsent_attribute(sub_sent_beg, end));

}

void SentStream::Append(const size_t beg, const size_t end, const SentAttribute attr)
{
	assert(end >= beg);

	if( beg >= end)
		return;

	if(get_buffer_length() + end - beg > m_max_word_num)
		Flush();

	m_sent_buf.push_back(make_pair(beg, end));

	if(attr == ATTR_SENT_TERMINAL && get_buffer_length() > m_min_word_num)
		Flush();
}

void SentStream::Flush()
{
	if(m_sent_buf.size() == 0) 
		return;

	if(m_sent_buf.begin()->first < m_sent_buf.rbegin()->second)
		m_output_vec.push_back(make_pair(m_sent_buf.begin()->first, m_sent_buf.rbegin()->second));
	
	m_sent_buf.clear();

}



size_t SentStream::get_buffer_length()
{
	if(m_sent_buf.size() == 0)
		return 0;

	assert(m_sent_buf.rbegin()->second >= m_sent_buf.begin()->first);

	return m_sent_buf.rbegin()->second - m_sent_buf.begin()->first;
}


void SentSpliter::init_block_seq(const vector<string> & src_word_vec)
{
	for(size_t i=0; i<src_word_vec.size(); ++i)
	{
		Block block;
		block.content = src_word_vec[i];
		identify(block);

		m_block_vec.push_back(block);
	}

	if(m_is_add_miss_end_punc && m_block_vec.size() > 0 )
	{
		if(TYPE_WORD == m_block_vec.rbegin()->type)
		{
			//在后面加上结束标点
			Block block;
			block.content = m_punc_resource.SentEndPunc();
			block.name = m_punc_resource.SentEndPuncName();
			block.type = TYPE_PUNC_TERMINAL;
			m_block_vec.push_back(block);
		}
	}

	//生成标点配对
	vector<size_t> punc_stack;
	for(size_t i=0; i<m_block_vec.size(); ++i)
	{
		if(m_block_vec[i].type == TYPE_PUNC_QUOT)
		{

			size_t num = punc_stack.size();

			while( num > 0 )
			{
				const size_t idx = num - 1;

				if(true == m_punc_resource.IsLeftRightMatch(m_block_vec[punc_stack[idx]].name, m_block_vec[i].name))
					break;

				num--;
			}
			
			if( 0 == num )
				punc_stack.push_back(i);
			else
			{
				m_left_right_map.insert(make_pair(punc_stack[num-1], i));

				m_block_vec[punc_stack[num-1]].type = TYPE_PUNC_QUOT_LEFT;
				m_block_vec[i].type = TYPE_PUNC_QUOT_RIGHT;

				punc_stack.erase(punc_stack.begin()+(num-1), punc_stack.end());
			}

		}
	}

}

void SentSpliter::identify(Block & block)
{

	//生成name 后面可能要进行标准转换，保留原来的content
	string punc_name = block.content;

	//转换为标准名称
	m_punc_resource.Conv2StdPunc(punc_name);

	//查找标点属性表
	if(false == m_punc_resource.FindPuncAttr(punc_name, block.type))
	{
		//没找到 作为普通词
		block.name.clear();
		block.type = TYPE_WORD;
		return;
	}else
	{
		//找到了 设置属性
		block.name = punc_name;
	}

	return;
}


const string & SentSpliter::get_block_content(const size_t idx) const 
{
	assert(idx < m_block_vec.size());

	return m_block_vec[idx].content;
}
const string & SentSpliter::get_block_name(const size_t idx) const 
{
	assert(idx < m_block_vec.size());

	return m_block_vec[idx].name;
}

size_t SentSpliter::get_block_type(const size_t idx) const 
{
	assert(idx < m_block_vec.size());

	return m_block_vec[idx].type;
}


SentAttribute SentSpliter::get_subsent_attribute(const size_t beg, const size_t end)
{
	assert(end <= m_block_vec.size() && beg <= end);

	if(beg >= end) 
		return ATTR_SENT_NORMAL;

	const size_t last = end - 1;

	const int type = get_block_type(last);

	if( type == TYPE_PUNC_TERMINAL )
	{
		return ATTR_SENT_TERMINAL;
	}else if( type == TYPE_PUNC_QUOT_RIGHT && last - beg > 0)
	{
		if(get_block_type(last-1) == TYPE_PUNC_TERMINAL && m_punc_resource.IsRightSpeakQuot(get_block_name(last)) )
			return ATTR_SENT_TERMINAL;
	}

	return ATTR_SENT_NORMAL;
}

