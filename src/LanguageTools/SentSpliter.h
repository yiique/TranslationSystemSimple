#ifndef SENT_SPLITER_H
#define SENT_SPLITER_H

//
#include <assert.h>

//STL
#include <string>
#include <fstream>
#include <utility>
#include <list>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <deque>
#include <stack>
using namespace std;

#include "PuncResource.h"

namespace ssplite
{

#define CH_MAX_WORD_NUM 100
#define CH_MIN_WORD_NUM 3
#define EN_MAX_WORD_NUM 100
#define EN_MIN_WORD_NUM 3

	typedef enum {
		ATTR_SENT_NORMAL,
		ATTR_SENT_TERMINAL
	} SentAttribute;

	class SentStream
	{
	public:

		SentStream(const size_t max_word_num,
				   const size_t min_word_num,
				   vector<pair<size_t, size_t> > & output_vec) :
				m_max_word_num(max_word_num),
				m_min_word_num(min_word_num),
				m_output_vec(output_vec)
		{
		}
		~SentStream(void) {}

		// 向buffer添加内容,遇到terminal或buffer满执行flush
		void Append(const size_t beg, const size_t end, const SentAttribute attr);
		void Flush();

	private:
		const size_t m_max_word_num;
		const size_t m_min_word_num;

		deque<pair<size_t, size_t> > m_sent_buf;        // buf队列
		vector<pair<size_t, size_t> > & m_output_vec;   // 输出向量

		size_t get_buffer_length();
	};


	class SentSpliter
	{
	public:
		SentSpliter(const string & token_src,
					const bool is_add_miss_end_punc,
					const size_t max_word_num,
					const size_t min_word_num,
					const PuncResource & punc_resource);

		~SentSpliter(void) {}

		virtual const vector<string> & GetDebugResult(void);
		virtual const vector<string> & GetSplitResult(void);
	private:

		typedef struct block
		{
			string content;
			string name;
			int    type;
		} Block;

		const size_t m_max_word_num;
		const size_t m_min_word_num;

		const PuncResource & m_punc_resource;   //标点符号资源
		map<size_t, size_t> m_left_right_map;   //左右标点符号配对表
		const bool m_is_add_miss_end_punc;      //如果输入不以标点符号结束，是否在后面补上标点
		bool m_is_finish;

		vector<string> m_src_word_vec;          //原始输入的词序列
		vector<Block> m_block_vec;              //标点识别后的词序列
		vector<string> m_debug_splite_result;   //调试断句结果
		vector<string> m_splite_result;         //断句结果


		string & filter_head_tail(string & str);    //删除首尾换行符和空格
		void split_string_by_blank(const string & src, vector<string> & tgt_vec);
		void split_string_by_tag(const string & src, vector<string> & tgt_vec, const char tag);

		void init_block_seq(const vector<string> & src_word_vec);   // 识别序列中的标点
		void splite_sub_sent(const size_t beg, const size_t end, SentStream & sent_stream); // 划分子句

		void identify(Block & block);
		const string & get_block_content(const size_t idx) const ;
		const string & get_block_name(const size_t idx) const ;
		size_t get_block_type(const size_t idx) const ;

		bool find_last_punc(const size_t beg, const size_t end, size_t & last_punc);
		bool find_right_punc(const size_t beg, const size_t end, size_t & right_punc);
		SentAttribute get_subsent_attribute(const size_t beg, const size_t end);

	};


	class ChSentSpliter
			: public SentSpliter
	{
	public:
		ChSentSpliter(const string & token_src,
					  const bool is_add_miss_end_punc = true,
					  const size_t max_word_num = CH_MAX_WORD_NUM,
					  const size_t min_word_num = CH_MIN_WORD_NUM):
				SentSpliter(token_src,
							is_add_miss_end_punc,
							max_word_num,
							min_word_num,
							ChPuncResource::GetInstrance())
		{
		}
	};

	class EnSentSpliter
			: public SentSpliter
	{
	public:
		EnSentSpliter(const string & token_src,
					  const bool is_add_miss_end_punc = true,
					  const size_t max_word_num = EN_MAX_WORD_NUM,
					  const size_t min_word_num = EN_MIN_WORD_NUM):
				SentSpliter(token_src,
							is_add_miss_end_punc,
							max_word_num,
							min_word_num,
							EnPuncResource::GetInstrance())
		{
		}
	};
}


#endif //SENT_SPLITER_H
