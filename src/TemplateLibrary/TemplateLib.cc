#include "TemplateLib.h"
#include "Common/f_utility.h"
#include "Common/ErrorCode.h"
#include "TemplatePersistent.h"
#include "Common/ICUConverter.h"
#include "Log/LogStream.h"
#include "Common/Utf8Stream.h"

TemplateID TemplateLib::m_global_templateid = 0;

TemplateItem::TemplateItem(const TemplateInfo & _template_info, const bool is_en) : template_info(_template_info), m_is_en(is_en)
{
}

TemplateLib::TemplateLib(const TemplateLibInfo & templatelib_info): m_templatelib_info(templatelib_info)
{
}


bool TemplateVariable::create_src_regex()
{
	escpae();

	stringstream ss;

	ss << "(" ;

	if(include_or_not > 0)
	{

		for(size_t i=0; i<include_or_not_vec.size(); ++i)
		{
			if(include_or_not == 1)
				ss << "(?=";
			else
				ss << "(?!";

			ss << include_or_not_vec[i] << ")";
		}
	}
	
	ss << ".";

	if(len_min == len_max)
	{
		if(len_min > 0)
			ss << "{" << len_min << "}";
		else
			ss << "*";
	}else if(len_min > 0 && len_max == 0)
	{
		ss << "{" << len_min << ",}";
	}else if(len_min < len_max)
	{
		ss << "{0," << len_max << "}";
	}else
	{
		cerr << "len_max < len_min" << endl;
		return false;
	}

	
	if(match_last)
		ss << "?";

	ss << ")";

	src_regex = ss.str();

	return true;
	
}

void TemplateVariable::Print() const
{
	cout << "TemplateVariable:" << endl;
	cout << "    id = " << id << endl;
	cout << "    len_limit = (" << len_min << "," << len_max << ")" << endl;
	cout << "    is_include = " << include_or_not << endl;
	for(size_t i=0; i<include_or_not_vec.size(); ++i)
	{
		cout << "      [" << i << "]:" << include_or_not_vec[i];
	}

	cout << endl;
	cout << "    match_last = " << match_last << endl;

	cout << "src pos : " << src_beg << " , " << src_end << endl;
	cout << "tgt pos : " << tgt_beg << " , " << tgt_end << endl;

	cout << "src regex : " << src_regex << endl;
}

void TemplateVariable::escpae()
{
	
	for(size_t i=0; i<include_or_not_vec.size(); ++i)
	{
		string & s = include_or_not_vec[i];

		if(s == "." || s == "^" || s == "$"
			|| s == "*" || s == "+" || s == "?"
			|| s == "[" || s == "]" || s == "{" || s == "}"
			|| s == "(" || s == ")" )
			s = "\\" + s;
	}
}

bool TemplateVariable::ResetSrc(const string & s_id,
							 const string & s_len_limit,
							 const string & s_include_or_not,
							 const string & s_include_or_not_content,
							 const string & s_match_last)
{
	
	id = str_2_num(s_id);
	//cout << "id = " << id << endl;
	
	vector<string> len_limit_vec;
	split_sequence_by_tag(s_len_limit.c_str(), len_limit_vec, ',');

	if(len_limit_vec.size() == 1)
	{
		len_min = len_max = str_2_num(filter_head_tail(len_limit_vec[0]));
	}else if(len_limit_vec.size() == 2)
	{
		filter_head_tail(len_limit_vec[0]);
		filter_head_tail(len_limit_vec[1]);

		len_min = (len_limit_vec[0].size() == 0) ? 0 : str_2_num(len_limit_vec[0]);
		len_max = (len_limit_vec[1].size() == 0) ? 0 : str_2_num(len_limit_vec[1]);
	}else
	{
		cerr << "Set s_len_limt[" << s_len_limit << "] failed." << endl;
		return false;
	}

	if(s_include_or_not == "0")
	{
		include_or_not = 0;
		include_or_not_vec.clear();
	}else if(s_include_or_not == "+" || s_include_or_not == "-")
	{
		split_string_by_blank(s_include_or_not_content.c_str(), include_or_not_vec);

		if(include_or_not_vec.size() == 0)
			include_or_not = 0;
		else
			include_or_not = (s_include_or_not == "+") ? 1 : 2;

	}else
	{
		cerr << "Set include flag[" << s_include_or_not << "] failed. " << endl; 
	}

	match_last = (s_match_last == "?") ? true : false;

	return create_src_regex();
}

bool TemplateItem::extract_variable(const string & src, const string & tgt, vector<tvar_share_ptr> & tvar_vec)
{
	map<size_t, tvar_share_ptr> tvar_map;
	tvar_vec.clear();

	try
	{

		//抽取源端
		boost::regex expression("##(\\d+) *((\\[( *\\d+ *, *\\d+ *)\\])|(\\[( *\\d+ *)\\])|(\\[( *, *\\d+ *)\\])|(\\[( *\\d+ *, *)\\])) *((\\{(\\+|\\-)(.+?)\\})|(\\{ *(0) *\\})) *(\\??)");
		boost::smatch what;

		string::const_iterator beg = src.begin();
		string::const_iterator end = src.end();
		
		size_t src_max_id = 0;

		while( boost::regex_search(beg, end, what, expression) )
		{

			tvar_share_ptr sp_tvar(new TemplateVariable());

			string s_id;
			string s_len_limit;
			string s_include_or_not;
			string s_include_or_not_content;
			string s_match_last;

			s_id = src.substr(what[1].first - src.begin(), what[1].second-what[1].first);

			s_len_limit = src.substr(what[6].first - src.begin(), what[6].second-what[6].first);

			if((s_len_limit = src.substr(what[4].first - src.begin(), what[4].second-what[4].first)).size() !=0)
			{
			}else if((s_len_limit = src.substr(what[6].first - src.begin(), what[6].second-what[6].first)).size() !=0)
			{
			}else if((s_len_limit = src.substr(what[8].first - src.begin(), what[8].second-what[8].first)).size() !=0)
			{
			}else if((s_len_limit = src.substr(what[10].first - src.begin(), what[10].second-what[10].first)).size() !=0)
			{}

			if((s_include_or_not_content = src.substr(what[14].first - src.begin(), what[14].second-what[14].first)).size() != 0)
			{
				s_include_or_not = src.substr(what[13].first - src.begin(), what[13].second-what[13].first);
			}else
			{
				s_include_or_not = src.substr(what[16].first - src.begin(), what[16].second-what[16].first);
			}

			s_match_last = src.substr(what[17].first - src.begin(), what[17].second-what[17].first);

			sp_tvar->ResetSrc(s_id, s_len_limit, s_include_or_not, s_include_or_not_content, s_match_last);
			sp_tvar->SetSrcPos(what[0].first - src.begin(), what[0].second -  src.begin());


			if(sp_tvar->id == 0)
			{
				cerr << "Var id can not be zero." << endl;
				return false;
			}

			if(false == tvar_map.insert(make_pair(sp_tvar->id, sp_tvar)).second)
			{
				cerr << "Same var id(" << sp_tvar->id << ") ." << endl;
				return false;
			}

			//check
			if(sp_tvar->id > src_max_id)
				src_max_id = sp_tvar->id;


			

			//cout << src.substr(what[0].first - src.begin(), what[0].second-what[0].first) << endl;

			beg = what[0].second;
		
		}

		//cout << tvar_map.size() << endl;

		if(tvar_map.size() == 0)
		{
			cerr << "No var in src template." << endl;
			return false;
		}

		//检查var是否连续
		if(src_max_id != tvar_map.size())
		{
			cerr << "Tempate vars' id(src) is not continuous" << endl;
			return false;
		}

		//cout << "ex tgt." << endl;

		//抽取目标端
		boost::regex expression2("##(\\d+)");

		beg = tgt.begin();
		end = tgt.end();

		size_t tgt_max_id = 0;
		size_t tgt_var_cnt = 0;

		while( boost::regex_search(beg, end, what, expression2) )
		{

			size_t id = str_2_num(tgt.substr(what[1].first - tgt.begin(), what[1].second-what[1].first));

			//cout << "tgt _id " << id << endl;

			map<size_t, tvar_share_ptr>::iterator iter = tvar_map.find(id);

			if(tvar_map.end() == iter)
			{
				cerr << "Tgt var id(" << id << ") not match." << endl;
				return false;
			}

			tvar_share_ptr sp_tvar = iter->second;

			if(sp_tvar->IsSetTgt())
			{
				cerr << "Tgt var id(" << id << ") can not be same." << endl;
				return false;
			}

			sp_tvar->SetTgtPos(what[0].first - tgt.begin(), what[0].second - tgt.begin());

			/*if(false == sp_tvar->ConvSrc2Regex())
			{
				cerr << "Conv src to Regex failed." << endl;
				return false;
			}*/

			if(id > tgt_max_id)
				tgt_max_id = id;

			++tgt_var_cnt;

			beg = what[0].second;
		}


		if(tgt_var_cnt == 0)
		{
			cerr << "No var in tgt template." << endl;
			return false;
		}

		if(tgt_var_cnt != tgt_max_id)
		{
			cerr << "Tempate vars' id(tgt) is not continuous" << endl;
			return false;
		}

		//cout << "src_max_id " << src_max_id << endl;
		//cout << "tgt max _id " << tgt_max_id << endl;

		if(tgt_max_id != src_max_id)
		{
			cerr << "Template vars id(src and tgt) is not match." << endl;
			return false;
		}

		//copy to vector
		for(map<size_t, tvar_share_ptr>::iterator iter = tvar_map.begin(); iter != tvar_map.end(); ++iter)
		{
			tvar_vec.push_back(iter->second);
		}

		//按照源端变量位置排序， 并重置ID
		sort(tvar_vec.begin(), tvar_vec.end(), src_range_cmp());

		//重置ID
		for(size_t i=0; i<tvar_vec.size(); ++i)
		{
			tvar_vec[i]->id = i+1;
		}

	}catch(...)
	{
		cerr << "Extract TemplateVariable Failed." << endl;
		return false;
	}

	return true;
}


bool TemplateItem::conv_2_std_template(const string & src, 
									   const string & tgt, 
									   const vector<tvar_share_ptr> & tvar_vec, 
									   string & std_src, 
									   string & std_tgt)
{
	//生成源端
	if(tvar_vec.size() == 0)
		return false;

	size_t beg = 0;
	for(size_t i=0; i<tvar_vec.size() && beg < src.size(); ++i)
	{
		size_t curr_regex_beg = tvar_vec[i]->src_beg;
		size_t curr_regex_end = tvar_vec[i]->src_end;

		assert(curr_regex_beg <= curr_regex_end && curr_regex_end <= src.size());


		if(beg < curr_regex_beg)
			std_src += src.substr(beg, curr_regex_beg-beg);

		std_src += tvar_vec[i]->src_regex;

		beg = curr_regex_end;
	}

	if(beg < src.size())
		std_src += src.substr(beg, src.size()-beg);

	//生成目标端
	vector<tvar_share_ptr> tgt_tvar_vec = tvar_vec;

	sort(tgt_tvar_vec.begin(), tgt_tvar_vec.end(), tgt_range_cmp());

	beg = 0;
	for(size_t i=0; i<tvar_vec.size() && beg < tgt.size(); ++i)
	{

		
		size_t curr_var_beg = tgt_tvar_vec[i]->tgt_beg;
		size_t curr_var_end = tgt_tvar_vec[i]->tgt_end;
		assert(curr_var_beg <= curr_var_end && curr_var_end <= tgt.size());

		/*cout << "Convtgt : beg = " << beg << endl;
		cout << "Convtgt : curr_var_beg " << curr_var_beg << endl;
		cout << "Convtgt : curr_var_end " << curr_var_end << endl;*/

		if(beg < curr_var_beg)
		{
			//cout << tgt.substr(beg, curr_var_beg-beg) << endl;
			std_tgt += tgt.substr(beg, curr_var_beg-beg);
		}

		std_tgt += " ##" + num_2_str(tgt_tvar_vec[i]->id) + " ";

		beg = curr_var_end;
	}

	if(beg < tgt.size())
		std_tgt += tgt.substr(beg, tgt.size()-beg);

	return true;
}

bool TemplateItem::Init()
{
	lout << "tsrc : " << template_info.src << endl;
	lout << "ttgt : " << template_info.tgt << endl;

	//抽取模板中的变量
	if(false == extract_variable(template_info.src, template_info.tgt, tvar_vec))
	{
		lerr << "Extract_variable failed , src = " << template_info.src << " , tgt = " << template_info.tgt << endl;
		return false;
	}

	string std_src, std_tgt;

	//转换为标准写法（正则表达式）
	if(false == conv_2_std_template(template_info.src, template_info.tgt, tvar_vec, std_src, std_tgt))
	{
		lerr << "Conv_2_std_template failed , src = " << template_info.src << " , tgt = " << template_info.tgt << endl;
		return false;
	}

	//抽取索引key
	if(m_is_en)
		extract_index_key_by_words(template_info.src, tvar_vec, key_set);
	else
		extract_index_key_by_characters(template_info.src, tvar_vec, key_set);

	string template_str;

	template_str += "1 ";
	template_str += std_src;
	template_str += "|||";
	template_str += std_tgt;

	lout << "template str : " << template_str << endl;

	if(m_is_en)
		return ict_template.Reset(template_str, "1");
	else
		return ict_template.Reset(template_str, "0");
}

void TemplateLib::MatchAppend(const string & sent, vector<string> & result_vec)
{
	lout << "index map size : " << m_template_index_map.size() << endl;
	lout << "unindex size : " << m_unindex_set.size() << endl;

	set<titem_share_ptr> sp_item_set;

	bool is_by_char;
	const string & slang = m_templatelib_info.domain_info.second.first;
	if(slang == LANGUAGE_CHINESE || slang == LANGUAGE_TIBET || slang == LANGUAGE_KOREAN)
	{
		is_by_char = true;
	}else
	{
		is_by_char = false;
	}

	//bool is_by_char = (m_templatelib_info.domain_info.second.first == LANGUAGE_CHINESE) ? true : false;

	if(is_by_char)
		get_template_from_index_by_characters(sent, sp_item_set);
	else
		get_template_from_index_by_words(sent, sp_item_set);

	set<titem_share_ptr>::iterator iter = sp_item_set.begin();

	lout << "template lib info , domain : " << m_templatelib_info.domain_info.first << endl;

	lout << "src sent = " << sent << endl;

	lout << "sp_item_set.size : " << sp_item_set.size() << endl;
	
	
	for(; iter != sp_item_set.end(); ++iter)
	{
		titem_share_ptr sp_item = *iter;

		if(0 == sp_item->template_info.is_active)
			continue;

		leonICTTemplate::NeedResult result;

		lout << sp_item->ict_template.pattern.e_pattern << endl;
		lout << sp_item->ict_template.pattern.c_pattern << endl;
		lout << "is_by_char : " << is_by_char << endl;

		if(is_by_char)
			leonICTTemplate::match_all(sp_item->ict_template, sent, result,"0");
		else
			leonICTTemplate::match_all(sp_item->ict_template, sent, result,"1");

		if(result.formatResult != "" && result.flag == true)
		{
			lout << result.formatResult << endl;
			result_vec.push_back(result.formatResult);
		}

	}
}

bool TemplateLib::register_template(const TemplateInfo & template_info)
{
	string tmp_src = template_info.src;
	string tmp_tgt = template_info.tgt;
	filter_head_tail(tmp_src);
	filter_head_tail(tmp_tgt);

	//查询filter_set, 检测重复模板

	return m_template_filter_set.insert(make_pair(tmp_src, tmp_tgt)).second;

}

void TemplateLib::unregister_template(const TemplateInfo & template_info)
{
	string tmp_src = template_info.src;
	string tmp_tgt = template_info.tgt;
	filter_head_tail(tmp_src);
	filter_head_tail(tmp_tgt);

	m_template_filter_set.erase(make_pair(tmp_src, tmp_tgt));

}

int TemplateLib::InsertTemplate(TemplateInfo & template_info)
{
	
	//过滤重复模板
	if(false == register_template(template_info) )
		return ERR_TEMPLATE_INSERT_SAME;

	//创建TemplateItem实例
	bool is_en = false;

	if(LANGUAGE_CHINESE == m_templatelib_info.domain_info.second.first
		|| LANGUAGE_TIBET == m_templatelib_info.domain_info.second.first
		|| LANGUAGE_KOREAN == m_templatelib_info.domain_info.second.first)
		is_en = false;
	else 
		is_en = true;

	titem_share_ptr sp_item(new TemplateItem(template_info, is_en));

	if(false == sp_item->Init() )
	{
		unregister_template(template_info);
		return ERR_TEMPLATE_FORMATE_ERROR;
	}

	m_global_templateid++;
	template_info.template_id = m_global_templateid;

	//持久化 获得Template id
	int ret = TemplatePersistent::InsertTemplate(m_templatelib_info.templatelib_id, template_info);

	if(SUCCESS != ret)
	{
		unregister_template(template_info);
		return ret;
	}

	//插入模板实例
	sp_item->template_info.template_id = template_info.template_id;

	if(false == m_template_map.insert(make_pair(template_info.template_id, sp_item)).second )
	{
		unregister_template(template_info);
		TemplatePersistent::DeleteTemplate(template_info.template_id);
		return ERR_TEMPLATE_INSERT_SAME_ID;
	}

	//建立索引
	create_index(sp_item);

	return SUCCESS;

}

int TemplateLib::InsertTemplateGroup(vector<TemplateInfo> & template_info_vec)
{
	bool is_en = false;

	if(LANGUAGE_ENGLISH == m_templatelib_info.domain_info.second.first)
		is_en = true;
	else 
		is_en = false;

	vector<bool> filter_vec;

	for(size_t i=0; i<template_info_vec.size(); ++i)
	{

		TemplateItem tmp(template_info_vec[i], is_en);

		if(tmp.Init())
		{
			lout << "init success." << endl;
			filter_vec.push_back( register_template( template_info_vec[i] ) );
		}else
		{
			lout << "init false." << endl;
			filter_vec.push_back(false);
		}

		if(true == filter_vec[i])
		{
			lout << "rfilter " << i << " is true" << endl; 
			m_global_templateid++;
			template_info_vec[i].template_id = m_global_templateid;
		}
	}

	//持久化  必须全部成功才算成功
	int ret = TemplatePersistent::InsertTemplateGroup(m_templatelib_info.templatelib_id, filter_vec, template_info_vec);
	if(SUCCESS != ret)
	{
		for(size_t i=0; i < template_info_vec.size(); ++i)
		{
			if(true == filter_vec[i])
				unregister_template(template_info_vec[i]);
		}

		return ret;
	}

	

	for(size_t i=0; i<template_info_vec.size(); ++i)
	{
		if(filter_vec[i])
		{
			titem_share_ptr sp_item(new TemplateItem(template_info_vec[i], is_en));

			sp_item->Init();
			
			m_template_map.insert(make_pair(sp_item->template_info.template_id, sp_item));

			//建立索引
			create_index(sp_item);
		}

	}

	return SUCCESS;

}

int TemplateLib::DeleteTemplate(const TemplateID & template_id)
{
	template_map_t::iterator iter = m_template_map.find(template_id);
	//lout << "delete template id = " << template_id << endl;

	if(m_template_map.end() == iter)
		return ERR_TEMPLATE_NOT_FIND_TEMPLATE_ID;

	//lout << "find delete template id = " << template_id << endl;

	unregister_template(iter->second->template_info);

	//删除索引
	delete_index(iter->second);

	m_template_map.erase(iter);

	TemplatePersistent::DeleteTemplate(template_id);

	return SUCCESS;
}

int TemplateLib::ModifyTemplate(TemplateInfo & template_info)
{
	template_map_t::iterator iter = m_template_map.find(template_info.template_id);

	if(m_template_map.end() == iter)
		return ERR_TEMPLATE_NOT_FIND_TEMPLATE_ID;

	titem_share_ptr sp_item = iter->second;

	if(false == register_template(template_info) )
	{
		if(sp_item->template_info.is_active == template_info.is_active)
			return ERR_TEMPALTE_MODIFY_SAME;
		
		sp_item->template_info.is_active = template_info.is_active;
		return TemplatePersistent::ModifyTemplate(template_info);
	}

	unregister_template(sp_item->template_info);
	//删除索引
	delete_index(iter->second);

	bool is_en = false;

	if(LANGUAGE_ENGLISH == m_templatelib_info.domain_info.second.first)
		is_en = true;
	else 
		is_en = false;

	titem_share_ptr sp_new_item(new TemplateItem(template_info, is_en));

	if(false == sp_new_item->Init() )
		return ERR_TEMPLATE_FORMATE_ERROR;

	iter->second = sp_new_item;

	//建立索引
	create_index(sp_new_item);

	return TemplatePersistent::ModifyTemplate(template_info);
}


int TemplateLib::ModifyTemplateLibInfo(const TemplateLibInfo & templatelib_info)
{
	if(templatelib_info == this->m_templatelib_info)
		return SUCCESS;

	m_templatelib_info = templatelib_info;

	return TemplatePersistent::ModifyTemplateLib(templatelib_info);
}


bool TemplateLib::LoadAll(vector<TemplateLib *> & templatelib_vec)
{
	//读取模板库
	vector<TemplateLibInfo> lib_info_vec;
	if(SUCCESS != TemplatePersistent::LoadAllTemplateLib(lib_info_vec))
		return false;

	for(size_t i=0; i<lib_info_vec.size(); ++i)
	{
		templatelib_vec.push_back(new TemplateLib(lib_info_vec[i]));
	}

	//读取模板
	for(size_t i=0; i<templatelib_vec.size(); ++i)
	{
		vector<TemplateInfo> template_info_vec;

		if(SUCCESS != TemplatePersistent::LoadTemplate(templatelib_vec[i]->GetInfo(), template_info_vec))
		{
			lerr << "Can't read template with templatelib id = " << templatelib_vec[i]->GetInfo().templatelib_id << endl;
			continue;
		}

		for(size_t k=0; k<template_info_vec.size(); ++k)
		{
			templatelib_vec[i]->mount_template(template_info_vec[k]);

		}
	}

	TemplateLibID lib_id;
	if(SUCCESS != TemplatePersistent::GetMaxTemplateID(lib_id, m_global_templateid))
	{
		lerr << "Can't get teamplate id . " << endl;
		return false;
	}

	lout << "Now db the largest template id is: " <<  m_global_templateid << endl;

	return true;
}

titem_share_ptr TemplateLib::mount_template(const TemplateInfo & template_info)
{
	//过滤重复模板
	if(false == register_template(template_info) )
		return titem_share_ptr();

	//创建TemplateItem实例
	bool is_en = false;

	if(LANGUAGE_ENGLISH == m_templatelib_info.domain_info.second.first)
		is_en = true;
	else 
		is_en = false;

	titem_share_ptr sp_item(new TemplateItem(template_info, is_en));

	if(false == sp_item->Init() )
		return titem_share_ptr();

	pair<template_map_t::iterator, bool> res = m_template_map.insert(make_pair(template_info.template_id, sp_item));
	if(false == res.second )
	{
		unregister_template(template_info);
		return titem_share_ptr();
	}

	//建立索引
	create_index(sp_item);

	return sp_item;
}

TemplateLib * TemplateLib::New(TemplateLibInfo & templatelib_info)
{
	if(SUCCESS != TemplatePersistent::CreateTemplateLib(templatelib_info))
		return NULL;

	TemplateLib * p_lib = new TemplateLib(templatelib_info);

	p_lib->m_templatelib_info.templatelib_id = templatelib_info.templatelib_id;

	return p_lib;
}

void TemplateLib::Destroy(TemplateLib * p_templatelib)
{
	if(NULL == p_templatelib)
		return;

	TemplateLibInfo templatelib_info;
	templatelib_info.templatelib_id = p_templatelib->m_templatelib_info.templatelib_id;

	//lout << "Destroy template lib id = " << templatelib_info.templatelib_id << endl;
	TemplatePersistent::DeleteTemplateLib(templatelib_info);

	delete p_templatelib;
}

void TemplateLib::Delete(TemplateLib * p_templatelib)
{
	if(NULL == p_templatelib)
		return;

	delete p_templatelib;
}


int TemplateLib::GetTemplateInfo(vector<TemplateInfo> & template_vec)
{
    //???此函数作用？
    //
    template_map_t::iterator it = m_template_map.begin();

	//TemplateInfo tmp;

	for(; it != m_template_map.end(); it++)
	{
		template_vec.push_back(it->second->template_info);
	}

	return 0;
}


int TemplateLib::RecoverTemplate(const TemplateID & template_id)
{
	TemplateInfo template_info;

	TemplatePersistent::RecoverTemplate(template_id);
	TemplatePersistent::GetRecoverTemplateInfo(template_id, template_info);

	//过滤重复模板
	if(false == register_template(template_info) )
		return ERR_TEMPLATE_INSERT_SAME;

	//创建TemplateItem实例
	titem_share_ptr sp_item(new TemplateItem(template_info));

	if(false == sp_item->Init() )
	{
		unregister_template(template_info);
		return ERR_TEMPLATE_FORMATE_ERROR;
	}

	//插入模板实例
	sp_item->template_info.template_id = template_info.template_id;

	if(false == m_template_map.insert(make_pair(template_info.template_id, sp_item)).second )
	{
		unregister_template(template_info);
		//TemplatePersistent::RecoverTemplate(template_info.template_id);
		return ERR_TEMPLATE_INSERT_SAME_ID;
	}

	//建立索引
	create_index(sp_item);

	return SUCCESS;
}

TemplateLib * TemplateLib::Recover(TemplateLibInfo & templatelib_info)
{
	TemplateLib * p_lib = new TemplateLib(templatelib_info);

	p_lib->m_templatelib_info.templatelib_id = templatelib_info.templatelib_id;

	return p_lib;
}

int TemplateLib::RecoverTemplateGroup(vector<TemplateInfo> & template_info_vec)
{
	vector<bool> filter_vec;

	for(size_t i=0; i<template_info_vec.size(); ++i)
	{
		filter_vec.push_back( register_template( template_info_vec[i] ) );
	}

	for(size_t i=0; i<template_info_vec.size(); ++i)
	{
		titem_share_ptr sp_item(new TemplateItem(template_info_vec[i]));

		if(false == sp_item->Init() ) //TODO
			continue;
		
		m_template_map.insert(make_pair(sp_item->template_info.template_id, sp_item));

		//建立索引
		create_index(sp_item);
	}

	return SUCCESS;

}

string & TemplateItem::std_key_by_words(string & key)
{
	del_head_tail_blank(key);

	vector<string> tmp_vec;
	split_string_by_blank(key.c_str(), tmp_vec);

	key.clear();
	for(size_t k=0; k<tmp_vec.size(); ++k)
	{
		key += tmp_vec[k];
		if(k+1 < tmp_vec.size())
			key += " ";
	}

	return key;
}

string & TemplateItem::std_key_by_char(string & key)
{
	del_head_tail_blank(key);
	
	vector<string> tmp_vec;
	split_string_by_blank(key.c_str(), tmp_vec);
	
	key.clear();
	for(size_t k=0; k<tmp_vec.size(); ++k)
	{
		key += tmp_vec[k];
	}

	return key;
}

void TemplateLib::create_index(titem_share_ptr sp_item)
{
	if(sp_item->key_set.size() > 0)
	{
		set<string>::iterator iter = sp_item->key_set.begin();
		for(; iter!=sp_item->key_set.end(); ++iter)
		{
            m_template_index_map[*iter].insert(sp_item);
		}
	}else
	{
		//加入到未索引列表, 如模板中的常量全是停用词
		m_unindex_set.insert(sp_item);
	}
}

void TemplateLib::delete_index(titem_share_ptr sp_item)
{
	if(sp_item->key_set.size() > 0)
	{
		set<string>::iterator iter = sp_item->key_set.begin();
		for(; iter!=sp_item->key_set.end(); ++iter)
		{
			const string & key = *iter;
			m_template_index_map[key].erase(sp_item);

			if(m_template_index_map[key].size() == 0)
				m_template_index_map.erase(key);
		}
	}else
	{
		//删除未索引列表, 如模板中的常量全是停用词
		m_unindex_set.erase(sp_item);
	}
}

void TemplateItem::extract_index_key_by_characters(const string & src, 
												   const vector<tvar_share_ptr> & tvar_vec, 
												   set<string> & key_set)
{

	key_set.clear();
	size_t beg = 0;

	bool is_all_stop_key = true;

	for(size_t i=0; i<tvar_vec.size() && beg < src.size(); ++i)
	{	
		tvar_share_ptr sp_tvar = tvar_vec[i];

		if(beg < sp_tvar->src_beg)
		{
			string key = src.substr(beg, sp_tvar->src_beg-beg);
			if(get_utf8_char_num(key) > 1)
				key_set.insert(std_key_by_char(key));
			
			beg = sp_tvar->src_end;
		}
	}

	if(beg < src.size())
	{
		string key = src.substr(beg, src.size()-beg);
		if(get_utf8_char_num(key) > 1)
			key_set.insert(std_key_by_char(key));

	}

	
}

void TemplateItem::extract_index_key_by_words(const string & src, 
											  const vector<tvar_share_ptr> & tvar_vec, 
											  set<string> & key_set)
{
	key_set.clear();
	size_t beg = 0;

	bool is_all_stop_key = true;

	for(size_t i=0; i<tvar_vec.size() && beg < src.size(); ++i)
	{	
		tvar_share_ptr sp_tvar = tvar_vec[i];

		if(beg < sp_tvar->src_beg)
		{
			string key = src.substr(beg, sp_tvar->src_beg-beg);
			if(get_word_num(key) > 1)
				key_set.insert(std_key_by_words(key));

			beg = sp_tvar->src_end;
		}
	}

	if(beg < src.size())
	{
		string key = src.substr(beg, src.size()-beg);
		if(get_word_num(key) > 1)
			key_set.insert(std_key_by_words(key));


	}

	
}

size_t TemplateItem::get_utf8_char_num(const string & src)
{
	vector<string> char_vec;

	Utf8Stream us(src);

	string utf8_c;
	while(us.ReadOneCharacter(utf8_c))
	{
		char_vec.push_back(utf8_c);
	}

	return char_vec.size();
}

size_t TemplateItem::get_word_num(const string & src)
{
	vector<string> tmp_vec;
	split_string_by_blank(src.c_str(), tmp_vec);

	return tmp_vec.size();
}

void TemplateLib::get_template_from_index_by_characters(const string & src, set<titem_share_ptr> & sp_item_set)
{

	//切字
	vector<string> char_vec;
	Utf8Stream us(src);

	string utf8_c;
	while(us.ReadOneCharacter(utf8_c))
	{
		filter_head_tail(utf8_c);

		if(utf8_c.size()> 0)
			char_vec.push_back(utf8_c);
	}
	
	cout << "Sent char size : " << char_vec.size() << endl;

	

	/*for(template_index_t::const_iterator iter = m_template_index_map.begin(); iter != m_template_index_map.end(); iter++)
	{
		lout << "map key : [" << iter->first << "] " << endl;
		
	}*/

	string key;
	key.resize(src.size() + 1);

	map<titem_share_ptr, set<string> > hit_index_map;

	for(size_t i=0; i<char_vec.size(); ++i)
	{
		key.clear();
		for(size_t k=i; k<char_vec.size(); ++k)
		{
			key += char_vec[k];

			template_index_t::const_iterator iter = m_template_index_map.find(key);

			if(iter != m_template_index_map.end())
			{
				lout << "hit key : [" << key << "] " << endl;
				set<titem_share_ptr>::const_iterator iter2 = iter->second.begin();
				for(; iter2 != iter->second.end(); ++iter2) 
				{
					hit_index_map[*iter2].insert(key);
				}
			}


		}

	}

	lout << "hit_index_map.size : " << hit_index_map.size() << endl;


	//过滤命中模板
	for(map<titem_share_ptr, set<string> >::iterator iter = hit_index_map.begin(); iter != hit_index_map.end(); ++iter)
	{
		if(comp_key_set(iter->first->key_set, iter->second))
		{
			sp_item_set.insert(iter->first);
		}
	}


	//未索引模板
	set<titem_share_ptr>::const_iterator iter = m_unindex_set.begin();

	for(; iter != m_unindex_set.end(); ++iter)
	{
		sp_item_set.insert(*iter);
	}
}


void TemplateLib::get_template_from_index_by_words(const string & src, set<titem_share_ptr> & sp_item_set)
{
	//切词
	vector<string> words_vec;
	split_string_by_blank(src.c_str(), words_vec);

	string key;
	key.resize(src.size()+1);

	map<titem_share_ptr, set<string> > hit_index_map;

	for(size_t i=0; i<words_vec.size(); ++i)
	{
		key.clear();
		for(size_t k=i; k<words_vec.size(); ++k)
		{
			key += words_vec[k];
			
			if(k+1 < words_vec.size())
				key += " ";

			template_index_t::const_iterator iter = m_template_index_map.find(key);

			if(iter != m_template_index_map.end())
			{
				set<titem_share_ptr>::const_iterator iter2 = iter->second.begin();
				for(; iter2 != iter->second.end(); ++iter2) 
				{
					hit_index_map[*iter2].insert(key);
				}
			}

		}
	}


	//过滤命中模板

	for(map<titem_share_ptr, set<string> >::iterator iter = hit_index_map.begin(); iter != hit_index_map.end(); ++iter)
	{
		if(comp_key_set(iter->first->key_set, iter->second))
		{
			sp_item_set.insert(iter->first);
		}
	}


	//未索引模板
	set<titem_share_ptr>::const_iterator iter = m_unindex_set.begin();

	for(; iter != m_unindex_set.end(); ++iter)
	{
		sp_item_set.insert(*iter);
	}
}

bool TemplateLib::comp_key_set(const set<string> & a, const set<string> & b)
{
	if(a.size() != b.size())
		return false;

	set<string>::const_iterator iter = a.begin();

	for(; iter != a.end(); ++iter)
	{
		if(b.find(*iter) == b.end())
			return false;
	}

	return true;
}

