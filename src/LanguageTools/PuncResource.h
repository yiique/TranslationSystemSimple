#ifndef PUNC_RESOURCE_H
#define PUNC_RESOURCE_H

#include <iostream>
#include <map>
#include <string>
#include <set>

using namespace std;

namespace ssplite
{

#define TYPE_PUNC_NORMAL        0
#define TYPE_PUNC_TERMINAL      1
#define TYPE_WORD               2
#define TYPE_PUNC_QUOT          3
#define TYPE_PUNC_QUOT_LEFT     4
#define TYPE_PUNC_QUOT_RIGHT    5

	class PuncResource
	{
	public:
		virtual ~PuncResource(void) {}

		// 找出punc的类型,否则返回false和TYPE_WORD
		bool FindPuncAttr(const string & punc_name, int & attr) const;
		// 将punc转为标准形式
		virtual string & Conv2StdPunc(string & punc_name) const;
		// 判断left和right是否匹配
		bool IsLeftRightMatch(const string & left_name, const string & right_name) const;
		// 判断是否right结尾符号
		bool IsRightSpeakQuot(const string & punc_name) const;

		const string & SentEndPunc() const
		{
			return m_sent_end_punc;
		}

		const string & SentEndPuncName() const
		{
			return m_sent_end_punc_name;
		}

	protected:
		PuncResource(void);

		virtual void init_punc_resource() = 0;

		map<string, int> m_punc_map;                    // punc和punc类型对照map
		map<string, string> m_std_conv_map;             // punc和其标准形式的对照map
		set<pair<string, string> > m_left_right_set;    // 左右punc对照
		set<string> m_right_speak_quot_set;             // 右punc结尾集合

		string m_sent_end_punc;                         //用于加在没有结束标点的句子后面
		string m_sent_end_punc_name;
	};


	class ChPuncResource
			: public PuncResource
	{
	public:
		static const ChPuncResource & GetInstrance();
		~ChPuncResource(void) {}

		string & Conv2StdPunc(string & punc_name) const;
	private:
		static ChPuncResource ms_ch_punc_resource;

		ChPuncResource(void)
		{
			init_punc_resource();
		}

		void init_punc_resource();
	};


	class EnPuncResource
			: public PuncResource
	{
	public:
		static const EnPuncResource & GetInstrance();
		~EnPuncResource(void) {}
	private:
		static EnPuncResource ms_en_punc_resource;

		EnPuncResource(void)
		{
			init_punc_resource();
		}

		void init_punc_resource();
	};

}

#endif //PUNC_RESOURCE_H
