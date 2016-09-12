
#ifndef TRANS_TEXT_H
#define TRANS_TEXT_H

//MAIN
#include "Log/LogStream.h"
#include "Common/BasicType.h"
#include "Common/f_utility.h"
#include "TextFormat.h"
#include "Common/CSequence.h"
#include "TinyXml/tinyxml.h"
#include "TimeLog.h"
#include "Common/BaseMutex.h"

//BOOST
#include <boost/function.hpp>

//STL
#include <vector>
#include <iostream>
#include <map>
#include <utility>
#include <string>
using namespace std;


#define TEXT_TYPE_TXT "txt"
#define TEXT_TYPE_DOC "doc"
#define TEXT_TYPE_DOCX "docx"
#define TEXT_TYPE_HTML "html"
#define TEXT_TYPE_PDF  "pdf"

//对齐关系
class BaseAlign
{
public:
	BaseAlign(const size_t _src_offset = 0,
			  const size_t _src_size = 0,
			  const size_t _tgt_offset = 0,
			  const size_t _tgt_size = 0): src_offset(_src_offset), src_size(_src_size), tgt_offset(_tgt_offset), tgt_size(_tgt_size)
	{}

	size_t src_offset;
	size_t src_size;

	size_t tgt_offset;
	size_t tgt_size;

	static string & Serialize(const vector<BaseAlign> & align_vec, string & tgt)
	{
		stringstream ss;
		
		for(size_t i=0; i<align_vec.size(); ++i)
		{
			const BaseAlign & align = align_vec[i];

			ss << align.src_offset << " " << align.src_size << " " << align.tgt_offset << " " << align.tgt_size << "\t";
		}

		tgt = ss.str();

		return tgt;
	}

	static void UnSerialize(const string & tgt, vector<BaseAlign> & align_vec)
	{
		align_vec.clear();

		vector<string> line_vec;
		split_string_by_tag(tgt.c_str(), line_vec, '\t');

		for(size_t i=0; i<line_vec.size(); ++i)
		{
			vector<string> tmp_vec;
			split_string_by_tag(line_vec[i].c_str(), tmp_vec, ' ');

			if(tmp_vec.size() == 4)
			{
				align_vec.push_back(BaseAlign(str_2_num(tmp_vec[0]), str_2_num(tmp_vec[1]), str_2_num(tmp_vec[2]), str_2_num(tmp_vec[3])));
			}
		}
	}
};

class TransAlignment
{
public:

	//原文
	string src;

	//原文 - a2b结果
	vector<BaseAlign> src_a2b_vec;  //长度为字符数

	//a2b结果 - 分词结果
	vector<BaseAlign> a2b_seg_vec; //长度为词数

	//分词结果 - 断句结果
	vector<BaseAlign> seg_splite_vec; //长度为句子数

	class SentAlignment
	{
	public:
		string src;
		string tgt;
		vector<BaseAlign> align_vec; //原文到词的对齐
		vector<BaseAlign> final_align_vec;
	};

	vector<SentAlignment> sent_vec;
};

//  翻译单元
class TransSrc
{
public:
	TransSrc(const TextID & text_id = "NULL", 
			 const size_t cell_id = 0): _text_id(text_id), 
									_cell_id(cell_id),
									_trans_times(0){} ;
	~TransSrc(void) {};



	TextID _text_id;
	size_t _cell_id;

	string _src_str;
	string _pre_proc_result;  //前处理结果
	vector<string> _dict_result_vec;

	string Rules() const
	{
		string rules;

		for(size_t i=0; i<_dict_result_vec.size(); ++i)
		{
			rules += _dict_result_vec[i] + " ; ";
		}

		return rules;
	}

	//被翻译的次数
	mutable size_t _trans_times;
} ;


class TransTgt
{
public:
	TransTgt(const TextID & text_id, 
			 const size_t cell_id): _text_id(text_id), 
									_cell_id(cell_id) {} ;
	~TransTgt(void) {};


	TextID _text_id;
	size_t _cell_id;

	string _decode_result; //解码后的结果

	string _tgt_str;
	string _assist_tgt;
	string _assist_str;
	string _assist2_str;

	vector<BaseAlign> _decode_align_vec;

	vector<string> _oov_vec;
	string _oov_sent;

	#ifdef OUTPUT_TRANS_TIME
	double _trans_time;
	#endif
} ;



class SentProcCell
{
public:
	const string & usr_id;
	const DomainType & domain;
	TransSrc & trans_src;
	TransTgt & trans_tgt;

public:
	SentProcCell(const string & _usr_id, 
				 const DomainType & _domain, 
				 TransSrc & _trans_src,
				 TransTgt & _trans_tgt): usr_id(_usr_id),
										 domain(_domain),
										 trans_src(_trans_src),
										 trans_tgt(_trans_tgt) {}
	~SentProcCell() {}
};

typedef boost::function<bool (string &, TransAlignment &)> PreTextHandler;
typedef boost::function<bool (const string & , vector<string> &, TransAlignment &)> SpliteSentHandler;
typedef boost::function<bool (SentProcCell &)> SentHandler;



class TransInfo
{
private:
	typedef struct sentlog
	{
		string _sent_src;
		string _trans_res;

#ifdef OUTPUT_TRANS_TIME
		double m_trans_time;
#endif

	} SentLog;

	typedef struct paralog
	{
		string _para_src;
		string _pre_res;
		string _seg_token_res;
		vector<SentLog> _sub_sent_vec;
	} ParaLog;

public:
	TransInfo(const TextID & tid): m_text_id(tid) {}
	~TransInfo(void) {}

	void Serialize(string & str);

	void SetSrc(const string & src)
	{
		m_src = src;
	}

	void SetDomain(const string & domain)
	{
		m_domain = domain;
	}

	void SetSrcLanguage(const string & src_language)
	{
		m_src_language = src_language;
	}

	void SetTgtLanguage(const string & tgt_language)
	{
		m_tgt_language = tgt_language;
	}

	void BuildPara(const vector<string> & para_str_vec)
	{
		m_para_vec.clear();

		for(size_t i = 0; i<para_str_vec.size(); ++i)
		{
			ParaLog para;
			m_para_vec.push_back(para);
			m_para_vec.rbegin()->_para_src = para_str_vec[i];
		}
	}

	bool SetPreRes(const size_t para_idx, const string & pre_res)
	{
		//lout << "para_idx = " << para_idx << "   m_para_vec.size() = " <<  m_para_vec.size() << endl;
		if(para_idx >= m_para_vec.size()) 
			return false;

		m_para_vec[para_idx]._pre_res = pre_res;
		return true;
	}

	bool SetSegTokenRes(const size_t para_idx, const string & seg_token_res)
	{
		//lout << "para_idx = " << para_idx << "   m_para_vec.size() = " <<  m_para_vec.size() << endl;
		if(para_idx >= m_para_vec.size()) 
			return false;

		m_para_vec[para_idx]._seg_token_res = seg_token_res;
		return true;
	}


	bool BuildSubSent(const size_t para_idx, const vector<string> & sub_sent_vec)
	{
		//lout << "para_idx = " << para_idx << "   m_para_vec.size() = " <<  m_para_vec.size() << endl;
		if(para_idx >= m_para_vec.size()) 
			return false;

		m_para_vec[para_idx]._sub_sent_vec.clear();

		for(size_t i=0; i<sub_sent_vec.size(); ++i)
		{
			SentLog sent;
			m_para_vec[para_idx]._sub_sent_vec.push_back(sent);
			m_para_vec[para_idx]._sub_sent_vec.rbegin()->_sent_src = sub_sent_vec[i];
		}

		return true;
	}

	void SetTransResult(const string & trans_res)
	{
		m_tgt = trans_res;
	}

	bool SetTransResult(const size_t para_idx, const size_t sent_idx, const string & trans_res)
	{
		//lout << "para_idx = " << para_idx << "   m_para_vec.size() = " <<  m_para_vec.size() << endl;
		if(para_idx >= m_para_vec.size()) 
			return false;

		//lout << "sent_idx = " << sent_idx << "   m_para_vec["<< para_idx << "]._sub_sent_vec.size() = " <<  m_para_vec[para_idx]._sub_sent_vec.size() << endl;
		if(sent_idx >= m_para_vec[para_idx]._sub_sent_vec.size())
			return false;

		m_para_vec[para_idx]._sub_sent_vec[sent_idx]._trans_res = trans_res;

		return true;
	}

	bool SetSubSrc(const size_t para_idx, const size_t sent_idx, const string & sub_src)
	{
		//lout << "para_idx = " << para_idx << "   m_para_vec.size() = " <<  m_para_vec.size() << endl;

		if(para_idx >= m_para_vec.size()) 
			return false;

		//lout << "sent_idx = " << sent_idx << "   m_para_vec["<< para_idx << "]._sub_sent_vec.size() = " <<  m_para_vec[para_idx]._sub_sent_vec.size() << endl;

		if(sent_idx >= m_para_vec[para_idx]._sub_sent_vec.size())
			return false;

		m_para_vec[para_idx]._sub_sent_vec[sent_idx]._sent_src = sub_src;

		return true;
	}

	void SetTextTransTime(const double trans_time)
	{
		#ifdef OUTPUT_TRANS_TIME
		m_trans_time = trans_time;
		#endif
	}

	void SetSentTransTime(const size_t para_idx, const size_t sent_idx, const double trans_time)
	{
		#ifdef OUTPUT_TRANS_TIME
		if(para_idx >= m_para_vec.size()) 
			return ;

		if(sent_idx >= m_para_vec[para_idx]._sub_sent_vec.size())
			return ;

		m_para_vec[para_idx]._sub_sent_vec[sent_idx].m_trans_time = trans_time;
		#endif
	}
private:

	const TextID m_text_id;
	string m_src;
	string m_tgt;
	string m_domain;
	string m_src_language;
	string m_tgt_language;

	vector<ParaLog> m_para_vec;
	
#ifdef OUTPUT_TRANS_TIME
	double m_trans_time;
#endif

};


class TransText
{
protected:

	//翻译单元
	class TransCell
	{
	public:
		TransCell(const TextID & text_id, 
				  const SentUniqID sent_id,
				  const size_t cell_id, 
				  const pair<size_t, size_t> para_idx): _sent_uid(sent_id),
														_trans_src(text_id, cell_id),
														_trans_tgt(text_id, cell_id),
														_cell_id(cell_id),
														_para_idx(para_idx),
														_is_trans(false) {};
		~TransCell(void) {};

		TransSrc _trans_src;
		TransTgt _trans_tgt;

		size_t _cell_id;
		pair<size_t, size_t> _para_idx; //段落偏移量
		SentUniqID _sent_uid;
		bool _is_trans;
	
	} ;

public:
	TransText(const TransType trans_type, const TextID & tid);
	~TransText(void);

	void setFakeString(string str);

	static bool String2TextID(const string & str, TextID & tid)
	{
		tid = str;
		del_head_tail_blank(tid);
		return true;
	};

	//生成全局唯一TextID --》uuid
	static TextID GenerateTextUUID(); 

	static void ResetBaseSentID(const long id);

	bool PreProcess(PreTextHandler    pre_text_handler, 
					SpliteSentHandler splite_sent_handler, 
					SentHandler    pre_sent_handler);

	bool PostProcess(SentHandler  post_sent_handler);

	bool CreateAlignment(string & format_str, string & align_str);

	void PrintInfo() const ;

	void ResetSrcData(const string & src_data);
	
	void SetFormatSrc(const vector<string> & para_vec, const vector<pair<size_t, size_t> > & pos_vec);

	bool SetTransResult(const TransTgt & trans_tgt);

	string Abstract() const;

	bool SetSentUniqID(const size_t idx, const SentUniqID suid);
	bool GetTgtText(string & tgt_text) const;  
	bool GetTgtDetail(string & tgt_text) const; 
	bool GetTgtDetail2(string & tgt_text) const;
	bool GetFormatDescribe(string & describe) const;

	bool getPreResult(string& tgt_text) const;

	size_t GetTransSize() const
	{
		return m_trans_cell_vec.size();
	}

	size_t GetCurrTransSize() const
	{
		return m_trans_cnt;
	}

	const TransSrc & GetTransSrc(const size_t idx) const;
	const TransTgt & GetTransTgt(const size_t idx) const;

	void SetAllUnTrans();

	void SetAllTrans()
	{
		m_trans_cnt = m_trans_cell_vec.size();
	}

	bool IsAllTrans() const 
	{
		return (m_trans_cnt >= m_trans_cell_vec.size());
	}


	const DomainType & GetDomain() const
	{
		return m_domain;
	}

	void SetDomain(const string & domain_name, const string & src_language, const string & tgt_language)
	{
		m_domain.first = domain_name;
		m_domain.second.first = src_language;
		m_domain.second.second = tgt_language;

		m_trans_info.SetDomain(domain_name);
		m_trans_info.SetSrcLanguage(src_language);
		m_trans_info.SetTgtLanguage(tgt_language);
	}

	void SetDomainName(const string & domain_name)
	{
		m_domain.first = domain_name;
		m_trans_info.SetDomain(domain_name);
	}

	void SetSrcLanguage(const string & src_language)
	{
		m_domain.second.first = src_language;
		m_trans_info.SetSrcLanguage(src_language);
	}


	void SetTgtLanguage(const string & tgt_language)
	{
		m_domain.second.second = tgt_language;
		m_trans_info.SetTgtLanguage(tgt_language);
	}

	const string & GetDomainName() const
	{
		return m_domain.first;
	}

	const string & GetSrcLanguage() const 
	{
		return m_domain.second.first;
	}

	const string & GetTgtLanguage() const
	{
		return m_domain.second.second;
	}

	const TextID & GetID() const
	{
		return m_text_id;
	}

	const string & GetSrcData() const
	{
		return m_src_data;
	}


	TransType GetTransType() const { return m_trans_type; };

	void GetTransInfoContent(string & content)
	{
		m_trans_info.Serialize(content);
	}

	void SetTransTime(const double trans_time)
	{
		m_trans_info.SetTextTransTime(trans_time);
	}

	const string & GetUsrID() const
	{
		return m_usr_id;
	}

	const string & GetTextType() const
	{
		return m_text_type;
	}

	void SetUsrID(const string & usr_id)
	{
		m_usr_id = usr_id;
	}

	void SetTextType(const string & text_type);

	const string & GetFilePath() const
	{
		return m_file_path;
	}

	void SetFilePath(const string & file_path)
	{
		//cout << "infilepath = [" << file_path << "]" << endl;
		m_file_path = file_path;
	}

	const TransCell & GetTransCell(const size_t idx) const
	{
		assert(idx < m_trans_cell_vec.size());

		return m_trans_cell_vec[idx];
	}

	const TextFormat & GetFromat() const
	{
		return m_format;
	}

	void GetFinishSent(const size_t offset, vector<pair<long, pair<string, string> > > & sent_vec);

	//去掉源端词性标注
	void RemoveSrcPosTag();

	SubLanguageType SubSrcLanguage() const
	{
		return m_sub_src_language_type;
	}

	SubLanguageType SetSubSrcLanguage(const int type)
	{
		return m_sub_src_language_type = (SubLanguageType) type;
	}

	void PrintDetail() const
	{
		lout << "[Domain] " << m_domain << endl;
		lout << "[Total Number] " << m_trans_cell_vec.size() << endl;
		lout << endl;
		lout << endl;

		for(size_t i=0; i<m_trans_cell_vec.size(); ++i)
		{
			lout << "[Sent " << i << " ]" << endl;
			lout << "   [   Pre     ] " << m_trans_cell_vec[i]._trans_src._pre_proc_result << endl;
			lout << "   [   Rules   ] " << m_trans_cell_vec[i]._trans_src.Rules() << endl;
			lout << "   [  Decode   ] " << m_trans_cell_vec[i]._trans_tgt._decode_result << endl;
			lout << "   [   Final   ] " << m_trans_cell_vec[i]._trans_tgt._tgt_str << endl;
			lout << "[-------]" << endl;
			lout << endl;

		}

		lout << "[END]" << endl;

	}

    size_t CountCharacter();
    
public:
	
	#ifdef ENABLE_TIME_LOG

	mutable TimeLog time_log;

	#endif //ENABLE_TIME_LOG

protected:

	const TransType m_trans_type;
	
	string m_src_data;   //原始文本数据
	const TextID m_text_id;   //全局唯一文本id号  与数据中的id号相同。

	//领域
	DomainType m_domain;

	//已翻译计数
	size_t m_trans_cnt;

	//文本格式信息
	/* mutable FormatText m_format_text; 改为在远端服务器处理格式 */
	mutable TextFormat m_format;

	//对齐
	vector<TransAlignment> m_align_vec; //每个元素为一个段落

	void check_format();

	vector<TransCell> m_trans_cell_vec;

	void remove_pos_tag(const string & src, string & tgt);

	//Translate 信息 记录中间结果
	mutable TransInfo m_trans_info;


	//去掉中文的词性标记
	void remove_pos_tag(string & src);


	string m_usr_id;
	string m_text_type;
	string m_file_path;

	//源端语种子类型
	SubLanguageType m_sub_src_language_type;


private:

	static long ms_base_sent_id;
	static MutexLock ms_sid_mutex;

	static SentUniqID generate_sent_id();
	string m_fake;
};





#endif //TRANS_TEXT_H

