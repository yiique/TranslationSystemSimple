#ifndef DB_OPERATION_H
#define DB_OPERATION_H


//MAIN
#include "EventEngine/BlockingDeque.h"
#include "Common/BasicType.h"
#include "Common/f_utility.h"
#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>
#include "Common/BaseMutex.h"
#include "Common/ErrorCode.h"
#include "Log/LogStream.h"
#include "TransText/TransText.h"
#include "DictLibrary/DictDef.h"
#include "TemplateLibrary/TemplateDef.h"
#include "LanguageTools/chinese/ChSegmentDef.h"
#include "PostProcessor/AfterTreatment/AfterTreatDef.h"

//boost
#include <boost/tuple/tuple.hpp>

//STL
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <list>

using namespace std;



class DBOperation
{
public:

	//翻译相关
	static int GetTransFileInfo(const TextID & tid, string & file_type, string & file_path);
	static int GetPermission(const string & usr_id, const size_t text_character);
    static int SubmitTransResult(TransText * p_text);
	static int SubmitOOV(TransText & trans_text);

	static int GetTransResult(const TextID & tid, vector< boost::tuple<string, string, string, size_t> > & result_vec, string & file_type, string & tgt_language);
	static int GetTransResultPair(const TextID & tid, vector<boost::tuple<string, string, string> > & result_vec, string & tgt_language);
	static int GetTransResultDetail(const TextID & tid, vector<boost::tuple<string, string, string, string, string> > & result_vec, string & domain, string & src_language, string & tgt_language);
	static int ReportTransPercent(const TextID & tid, const size_t percent);
	static int ReportTransState(const TextID & tid, const string & state);
	static int ReportTransStateAndPercent(const TextID & tid, const string & state, const size_t percent);
	static int ReportTransError(const TextID & tid, const int err_code, const string & state = "ERROR");

	static int UpdateTransInfo(const TextID & tid, const string & domain, const string & src_language, const string & tgt_language, const size_t words_num);


	//词典相关操作
	static int GetMaxDictWordID(DictID & dict_id, WordID & word_id);
	static int InsertWord(const DictID dict_id, const WordInfo & word_info);
	static int InsertWord(const DictID dict_id, const vector<WordInfo> & word_info_vec);
	static int DeleteWord(const WordID word_id);
	static int ModifyWord(const WordInfo & word_info);
	
	static int CreateDict(DictInfo & dict_info);
	static int ModifyDict(const DictInfo & dict_info);
	static int DeleteDict(const DictID & dict_id);
	
	static int LoadAllDict(vector<DictInfo> & dict_info_vec);
	static int LoadAllWord(vector<WordInfo> & word_info_vec);
	static int LoadWord(const vector<WordID> & word_id_vec, vector<WordInfo> & word_info_vec);

	//词典导入
	static int ModifyImportStatus(const vector<size_t> & import_word_id);
	//词典导出
	static int ModefyExportStatus(const DictID dict_id, const vector<WordInfo> & word_info_vec);

	//-------------抛弃---------------------//
	//恢复词典词条操作
	static int GetRecoverDictWordInfo(const DictID & dict_id, vector<WordInfo> & dict_vec);
	static int GetRecoverDictInfo(const DictID & dict_id, DictInfo & dict_info);
	static int GetRecoverWordInfo(const WordID & word_id, WordInfo & word_info);
	static int RecoverDict(const DictID & dict_id);
	static int RecoverWord(const WordID & word_id);

	
	//------------------------------------//

	//后处理相关操作
	static int InsertAfterWord(const AfterDictID dict_id, AfterWordInfo & word_info);
	static int InsertAfterWordGroup(const AfterDictID dict_id, const vector<bool> & filter_vec, vector<AfterWordInfo> & word_info_vec);
	static int DeleteAfterWord(const AfterWordID word_id);
	static int ModifyAfterWord(const AfterWordInfo & word_info);
	
	static int CreateAfterDict(AfterDictInfo & dict_info);
	static int ModifyAfterDict(const AfterDictInfo & dict_info);
	static int DeleteAfterDict(const AfterDictInfo & dict_info);
	
	static int LoadAllAfterDict(vector<AfterDictInfo> & dict_info_vec);
	static int LoadAfterWord(const AfterDictInfo & dict_info, vector<AfterWordInfo> & word_info_vec);

	//后处理词典恢复
	static int GetRecoverAfterDictWordInfo(const AfterDictID & dict_id, vector<AfterWordInfo> & dict_vec);
	static int GetRecoverAfterDictInfo(const AfterDictID & dict_id, AfterDictInfo & dict_info);
	static int GetRecoverAfterWordInfo(const AfterWordID & word_id, AfterWordInfo & word_info);
	static int RecoverAfterDict(const AfterDictID & dict_id);
	static int RecoverAfterWord(const AfterWordID & word_id);
	
	//模板相关操作
	static int InsertTemplate(const TemplateLibID templatelib_id, TemplateInfo & template_info);
	static int InsertTemplateGroup(const TemplateLibID templatelib_id, const vector<bool> & filter_vec, vector<TemplateInfo> & template_info_vec);
	static int DeleteTemplate(const TemplateID template_id);
	static int ModifyTemplate(const TemplateInfo & template_info);
	
	static int CreateTemplateLib(TemplateLibInfo & templatelib_info);
	static int ModifyTemplateLib(const TemplateLibInfo & templatelib_info);
	static int DeleteTemplateLib(const TemplateLibInfo & templatelib_info);

	static int LoadAllTemplateLib(vector<TemplateLibInfo> & templatelib_info_vec);
	static int LoadTemplate(const TemplateLibInfo & templatelib_info, vector<TemplateInfo> & template_info_vec);

	//中文分词词典
	static int LoadSegWord(vector<seg_word_share_ptr> & sp_vec);
	static int InsertSegWord(seg_word_share_ptr sp_word);
	static int DeleteSegWord(const int word_id);
	static int UpdateSegWord(seg_word_share_ptr sp_word);
	static int InsertSegWord(const vector<seg_word_share_ptr> & sp_word_vec, vector<bool> & filter_res_vec);
	static int DeleteSegWord(const vector<int> & word_id_vec, vector<bool> & filter_res_vec);
	static int RecoverySegWord(const vector<int> & word_id_vec, vector<seg_word_share_ptr> & sp_vec);

	//恢复模版操作
	static int GetRecoverTemplateLibTemplateInfo(const TemplateLibID & templatelib_id, vector<TemplateInfo> & template_vec);
	static int GetRecoverTemplateLibInfo(const TemplateLibID & templatelib_id, TemplateLibInfo & templatelib_info);
	static int GetRecoverTemplateInfo(const TemplateID & template_id, TemplateInfo & template_info);
	static int RecoverTemplateLib(const TemplateLibID & templatelib_id);
	static int RecoverTemplate(const TemplateID & template_id);

	static int GetMaxTemplateID(TemplateLibID & templatelib_id, TemplateID & template_id);
	//模版导入
	static int GetImportTemplateInfo(const vector<int> & import_template_id, vector<TemplateInfo> & import_template_info);
	static int ModifyStatus_T(const vector<int> & import_template_id);

	static int SubmitAbstract(const TextID & tid, const string & abstracts);

	static int GetMaxSentID(long & max_sent_id);

	//
	static int GetSplitStatus(const string & src_language, int & split_status);

private:
	static int get_trans_file_info(mysqlpp::Query & query, 
												const TextID & tid, 
												string & file_type, 
												string & file_path);

	static int get_permission(mysqlpp::Query & query, const string & usr_id, const size_t text_character);
	static int submit_trans_result(mysqlpp::Query & query, TransText * p_text);
	static int submit_oov(mysqlpp::Query & query, TransText & trans_text);
	static int get_trans_result(mysqlpp::Query & query, const TextID & tid, 
								vector< boost::tuple<string, string, string, size_t> > & result_vec, 
								string & file_type, 
								string & tgt_language);
	static int get_trans_result_pair(mysqlpp::Query & query, const TextID & tid, 
									 vector<boost::tuple<string, string, string> > & result_vec, 
									 string & tgt_language);

	static int get_trans_result_detail(mysqlpp::Query & query, const TextID & tid, 
	  								  vector<boost::tuple<string, string, string, string, string> > & result_vec, 
	  								  string & domain, 
	  								  string & src_language, 
	  								  string & tgt_language);
	static int report_trans_percent(mysqlpp::Query & query, const TextID & tid, const size_t percent);
	static int report_trans_state(mysqlpp::Query & query, const TextID & tid, const string & state);
	static int report_trans_state_and_percent(mysqlpp::Query & query, const TextID & tid, const string & state, const size_t percent);
	static int report_trans_error(mysqlpp::Query & query, const TextID & tid, const int err_code, const string & state);
	static int update_trans_info(mysqlpp::Query & query, const TextID & tid, const string & domain, const string & src_language, const string & tgt_language, const size_t words_num);
	static int get_max_dict_word_id(mysqlpp::Query & query, DictID & dict_id, WordID & word_id);
	static int insert_word(mysqlpp::Query & query, const DictID dict_id, const WordInfo & word_info);
	static int insert_word_group(mysqlpp::Query & query, const DictID dict_id, const vector<WordInfo> & word_info_vec);
	static int delete_word(mysqlpp::Query & query, const WordID word_id);
	static int modify_word(mysqlpp::Query & query, const WordInfo & word_info);
	static int create_dict(mysqlpp::Query & query, DictInfo & dict_info);
	static int modify_dict(mysqlpp::Query & query, const DictInfo & dict_info);
	static int delete_dict(mysqlpp::Query & query, const DictID & dict_id);
	static int load_all_dict(mysqlpp::Query & query, vector<DictInfo> & dict_info_vec);
	static int load_all_word(mysqlpp::Query & query, vector<WordInfo> & word_info_vec);
	static int load_word(mysqlpp::Query & query, const vector<WordID> & word_id_vec, vector<WordInfo> & word_info_vec);
	static int modify_import_status(mysqlpp::Query & query, const vector<size_t> & import_word_id);
	static int get_recover_dict_word_info(mysqlpp::Query & query, const DictID & dict_id, vector<WordInfo> & dict_vec);
	static int get_recover_dict_info(mysqlpp::Query & query, const DictID & dict_id, DictInfo & dict_info);
	static int get_recover_word_info(mysqlpp::Query & query, const WordID & word_id, WordInfo & word_info);
	static int recover_dict(mysqlpp::Query & query, const DictID & dict_id);
	static int recover_word(mysqlpp::Query & query, const WordID & word_id);
	static int insert_after_word(mysqlpp::Query & query, const AfterDictID dict_id, AfterWordInfo & word_info);
	static int insert_after_word_group(mysqlpp::Query & query, const AfterDictID dict_id, const vector<bool> & filter_vec, vector<AfterWordInfo> & word_info_vec);
	static int delete_after_word(mysqlpp::Query & query, const AfterWordID word_id);
	static int modify_after_word(mysqlpp::Query & query, const AfterWordInfo & word_info);
	static int create_after_dict(mysqlpp::Query & query, AfterDictInfo & dict_info);
	static int modify_after_dict(mysqlpp::Query & query, const AfterDictInfo & dict_info);
	static int delete_after_dict(mysqlpp::Query & query, const AfterDictInfo & dict_info);
	static int load_all_after_dict(mysqlpp::Query & query, vector<AfterDictInfo> & dict_info_vec);
	static int load_after_word(mysqlpp::Query & query, const AfterDictInfo & dict_info, vector<AfterWordInfo> & word_info_vec);
	static int get_recover_after_dict_word_info(mysqlpp::Query & query, const AfterDictID & dict_id, vector<AfterWordInfo> & dict_vec);
	static int get_recover_after_dict_info(mysqlpp::Query & query, const AfterDictID & dict_id, AfterDictInfo & dict_info);
	static int get_recover_after_word_info(mysqlpp::Query & query, const AfterWordID & word_id, AfterWordInfo & word_info);
	static int recover_after_dict(mysqlpp::Query & query, const AfterDictID & dict_id);
	static int recover_after_word(mysqlpp::Query & query, const AfterWordID & word_id);
	static int insert_template(mysqlpp::Query & query, const TemplateLibID templatelib_id, TemplateInfo & template_info);
	static int insert_template_group(mysqlpp::Query & query, const TemplateLibID templatelib_id, const vector<bool> & filter_vec, vector<TemplateInfo> & template_info_vec);
	static int delete_template(mysqlpp::Query & query, const TemplateID template_id);
	static int modify_template(mysqlpp::Query & query, const TemplateInfo & template_info);
	static int create_template_lib(mysqlpp::Query & query, TemplateLibInfo & templatelib_info);
	static int modify_template_lib(mysqlpp::Query & query, const TemplateLibInfo & templatelib_info);
	static int delete_template_lib(mysqlpp::Query & query, const TemplateLibInfo & templatelib_info);
	static int load_all_template_lib(mysqlpp::Query & query, vector<TemplateLibInfo> & templatelib_info_vec);
	static int load_template(mysqlpp::Query & query, const TemplateLibInfo & templatelib_info, vector<TemplateInfo> & template_info_vec);
	static int load_seg_word(mysqlpp::Query & query, vector<seg_word_share_ptr> & sp_vec);
	static int insert_seg_word(mysqlpp::Query & query, seg_word_share_ptr sp_word);
	static int delete_seg_word(mysqlpp::Query & query, const int word_id);
	static int update_seg_word(mysqlpp::Query & query, seg_word_share_ptr sp_word);
	static int recovery_seg_word(mysqlpp::Query & query, const vector<int> & word_id_vec, vector<seg_word_share_ptr> & sp_vec);

	static int get_recover_template_lib_template_info(mysqlpp::Query & query, const TemplateLibID & templatelib_id, vector<TemplateInfo> & template_vec);
	static int get_recover_template_lib_info(mysqlpp::Query & query, const TemplateLibID & templatelib_id, TemplateLibInfo & templatelib_info);
	static int get_recover_template_info(mysqlpp::Query & query, const TemplateID & template_id, TemplateInfo & template_info);
	static int recover_template_lib(mysqlpp::Query & query, const TemplateLibID & templatelib_id);
	static int recover_template(mysqlpp::Query & query, const TemplateID & template_id);

	static int get_import_template_info(mysqlpp::Query & query, const vector<int> & import_template_id, vector<TemplateInfo> & import_template_info);
	static int modify_status_t(mysqlpp::Query & query, const vector<int> & import_template_id);
	static int get_max_dict_wordid(mysqlpp::Query & query, DictID & dict_id, WordID & word_id);
	static int get_recover_after_dict_Info(mysqlpp::Query & query, const AfterDictID & dict_id, AfterDictInfo & dict_info);
	static int get_max_template_id(mysqlpp::Query & query, TemplateLibID & templatelib_id, TemplateID & template_id);

	static int submit_abstract(mysqlpp::Query & query, const TextID & tid, const string & abstracts);
	static int get_max_sent_id(mysqlpp::Query & query, long & max_sent_id);

	static int get_split_status(mysqlpp::Query & query, const string & src_language, int & split_status);
};


#endif //DB_OPERATION_H
