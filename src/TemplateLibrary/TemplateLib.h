#ifndef TEMPLATE_LIB_H
#define TEMPLATE_LIB_H

#include "TemplateDef.h"
#include "leonICTTemplate.h"

//BOOST
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/regex.hpp>

//STL
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <utility>
using namespace std;

class TemplateVariable
{
public:
	TemplateVariable(): src_beg(0), src_end(0), tgt_beg(0), tgt_end(0) {}


	bool ResetSrc(const string & s_id,
			   const string & s_len_limit,
			   const string & s_include_or_not,
			   const string & s_include_or_not_content,
			   const string & s_match_last);

	bool IsSetTgt() const 
	{
		return !(0 == tgt_beg && 0 == tgt_end);
	}

	bool IsSetSrc() const 
	{
		return !(0 == src_beg && 0 == src_end);
	}

	void SetSrcPos(const size_t beg, const size_t end)
	{
		assert(beg <= end);

		src_beg = beg;
		src_end = end;
	}

	void SetTgtPos(const size_t beg, const size_t end)
	{
		//assert(beg <= end);

		tgt_beg = beg;
		tgt_end = end;
	}

	void Print() const;

	void escpae();


	size_t id;  //模板变量编号
	size_t len_min;  //长度限制  最小值
	size_t len_max;  //长度限制  最大值
	size_t include_or_not;  //包含或不包含  0 无  1 包含  2 不包含
	vector<string> include_or_not_vec;  //包含或不包含的词组
	bool match_last;            //是否匹配最后出现 “？” 

	size_t src_beg;
	size_t src_end;
	size_t tgt_beg;
	size_t tgt_end;

	string src_regex;

private:
	bool create_src_regex();
};

typedef boost::shared_ptr<TemplateVariable> tvar_share_ptr;

class src_range_cmp
{
public:
	bool operator() (tvar_share_ptr sp_a, tvar_share_ptr sp_b)
	{
		assert(sp_a && sp_b);

		return ( sp_a->src_end <= sp_b->src_beg ); 

	}
};

class tgt_range_cmp
{
public:
	bool operator() (tvar_share_ptr sp_a, tvar_share_ptr sp_b)
	{
		assert(sp_a && sp_b);

		return ( sp_a->tgt_end <= sp_b->tgt_beg ); 

	}
};


class TemplateItem
{
public:
	TemplateItem(const TemplateInfo & _template_info, const bool is_en = false);

	bool Init();

	bool m_is_en;
	TemplateInfo template_info;
	leonICTTemplate::ICTtemplate ict_template;

	//模板变量
	vector<tvar_share_ptr> tvar_vec;

	//索引key
	set<string> key_set;

private:
	//解析
	bool extract_variable(const string & src, 
						  const string & tgt, 
						  vector<tvar_share_ptr> & tvar_vec);

	bool conv_2_std_template(const string & src, 
							 const string & tgt, 
							 const vector<tvar_share_ptr> & tvar_vec, 
							 string & std_src, 
							 string & std_tgt);

	//抽取索引
	void extract_index_key_by_words(const string & src,
									const vector<tvar_share_ptr> & tvar_vec,
									set<string> & key_set);

	void extract_index_key_by_characters(const string & src,
										 const vector<tvar_share_ptr> & tvar_vec,
										 set<string> & key_set);

	size_t get_word_num(const string & src);
	size_t get_utf8_char_num(const string & src);

	string & std_key_by_words(string & key);
	string & std_key_by_char(string & key);
};

typedef boost::shared_ptr<TemplateItem> titem_share_ptr;

class TemplateLib
{
public:
	~TemplateLib(void) {}


	static bool Initialize() { return true; }

	static bool LoadAll(vector<TemplateLib *> & templatelib_vec); //从持久层中读取系统中所有用户的模板库
	static TemplateLib * New(TemplateLibInfo & template_info); //此方法会在持久层中创建
	static void Destroy(TemplateLib * p_templatelib);  //此方法会在持久层中删除
	static void Delete(TemplateLib * p_templatelib);

	int InsertTemplate(TemplateInfo & template_info); //插入一条模板
	int InsertTemplateGroup(vector<TemplateInfo> & template_info_vec); //插入一组模板
	int DeleteTemplate(const TemplateID & template_id); //删除一条模板
	int ModifyTemplate(TemplateInfo & template_info); //修改某条模板
	
	void MatchAppend(const string & sent, vector<string> & result_vec);  //输入句子，匹配此模板库中的所有模板

	int ModifyTemplateLibInfo(const TemplateLibInfo & templatelib_info); //修改模板库的信息

	const TemplateLibInfo & GetInfo() const
	{
		return m_templatelib_info;
	}

	void Print() {}

	int GetTemplateInfo(vector<TemplateInfo> & template_vec);

	int RecoverTemplate(const TemplateID & template_id);
	static TemplateLib * Recover(TemplateLibInfo & templatelib_info);
	int RecoverTemplateGroup(vector<TemplateInfo> & template_info_vec);

private:

	TemplateLib(const TemplateLibInfo & templatelib_info);
	TemplateLibInfo m_templatelib_info;

private:
	//模板条目
	typedef map<TemplateID, titem_share_ptr> template_map_t;
	template_map_t m_template_map;
	
	titem_share_ptr mount_template(const TemplateInfo & template_info);
	
	set<pair<string,string> > m_template_filter_set;  //用于过滤重复的模板, key = src + tgt;
	bool register_template(const TemplateInfo & template_info);
	void unregister_template(const TemplateInfo & template_info);

	static TemplateID m_global_templateid;
	

private:
	
	//索引
	typedef map<string, set<titem_share_ptr> > template_index_t;
	template_index_t m_template_index_map;
	set<titem_share_ptr> m_unindex_set;

	void create_index(titem_share_ptr sp_item);
	void delete_index(titem_share_ptr sp_item);
	void get_template_from_index_by_characters(const string & src, set<titem_share_ptr> & sp_item_set);
	void get_template_from_index_by_words(const string & src, set<titem_share_ptr> & sp_item_set);
	bool comp_key_set(const set<string> & a, const set<string> & b);

};

#endif //TEMPLATE_LIB_H
