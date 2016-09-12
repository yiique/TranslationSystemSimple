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

		// ��buffer�������,����terminal��buffer��ִ��flush
		void Append(const size_t beg, const size_t end, const SentAttribute attr);
		void Flush();

	private:
		const size_t m_max_word_num;
		const size_t m_min_word_num;

		deque<pair<size_t, size_t> > m_sent_buf;        // buf����
		vector<pair<size_t, size_t> > & m_output_vec;   // �������

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

		const PuncResource & m_punc_resource;   //��������Դ
		map<size_t, size_t> m_left_right_map;   //���ұ�������Ա�
		const bool m_is_add_miss_end_punc;      //������벻�Ա����Ž������Ƿ��ں��油�ϱ��
		bool m_is_finish;

		vector<string> m_src_word_vec;          //ԭʼ����Ĵ�����
		vector<Block> m_block_vec;              //���ʶ���Ĵ�����
		vector<string> m_debug_splite_result;   //���ԶϾ���
		vector<string> m_splite_result;         //�Ͼ���


		string & filter_head_tail(string & str);    //ɾ����β���з��Ϳո�
		void split_string_by_blank(const string & src, vector<string> & tgt_vec);
		void split_string_by_tag(const string & src, vector<string> & tgt_vec, const char tag);

		void init_block_seq(const vector<string> & src_word_vec);   // ʶ�������еı��
		void splite_sub_sent(const size_t beg, const size_t end, SentStream & sent_stream); // �����Ӿ�

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
