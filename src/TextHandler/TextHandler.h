#ifndef TEXT_HANDLER_H
#define TEXT_HANDLER_H

#include "EventEngine/EventEngine.h"
#include "Log/LogStream.h"
#include "Common/ByteBuffer.h"
#include "TransText/TransText.h"
#include "TextHandler/TextHandlerEvent.h"
#include "TextParser/TxtParser.h"
#include "DataBase/DBOperation.h"

#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

class TextHandler
	: public EventEngine
{
public:
	
	~TextHandler(void) {}

	static TextHandler * GetInstance() { return &ms_instance;}

	bool Start(const size_t thread_num = 1, const string & default_file_path = "./");

	void LoadTransText(TransText * p_trans_text, const CallID cid, EventEngine * p_caller);
	void CreateResult(const TextID & tid, const CallID cid, const string & download_type, const bool is_bilingual, EventEngine * p_caller);
	void SubmitResult(TransText * p_text, const CallID cid, EventEngine * p_caller);

private:
	TextHandler(): m_thread_num(0) {}

	size_t m_thread_num;
	string m_default_file_path;
	string m_word_workspace;

	static TextHandler ms_instance;

	//禁用基类start函数，本类重新实现，用于启动多个线程
	bool Start(void) { return false; }


	//实现基类事件处理函数
	void on_event(Event & e);

	EventData * on_load_text(EventData * p_edata);
	EventData * on_create_result(EventData * p_edata);
	EventData * on_submit_result(EventData * p_edata);

	int build_base_source(const string & text, TransText & trans_text);
	int convert_txt_to_text(const string & filepath, string & text);
	int convert_pdf_to_text(const string & filepath, const TextID & tid, string & text);
	int convert_html_to_text(const string & filepath, const TextID & tid, string & text);
	int convert_word_to_text(const string & filepath, const TextID & tid, const string & file_type, string & text);

	int build_result_file(const TextID & tid, const bool is_bilingual, string & result_file_name);
	int build_tmx_result_file(const TextID & tid, string & result_file_name);
	int build_detail_result_file(const TextID & tid, string & result_file_name);

	int build_base_result_file(const TextID & tid, 
							   const string & build_id,
							   const vector<boost::tuple<string, string, string, size_t> > & result_vec,
							   const bool is_blank_between_sent,
							   const bool is_bilingual,
							   string & base_result_file_path,
							   string & result_text);
	
	int build_txt_result_file(const TextID & tid, 
							  const string & build_id,
							  const string & result_text,
							  string & result_file_name);

	int build_pdf_result_file(const TextID & tid, 
							  const string & build_id,
							  const string & result_text,
							  string & result_file_name);

	int build_html_result_file(const TextID & tid, 
							   const string & build_id,
							   const string & base_result_file_path,
							   string & result_file_name);


	int build_word_result_file(const TextID & tid, 
									   const string & build_id,
									   const string & file_type,
									   const string & result_text,
									   string & result_file_name);

	int build_text_result_file(const TextID & tid, 
							   const bool is_bilingual, 
							   string & result_file_name); //所有格式的均生成txt类型的结果


	void charset_handle(string & text);
	string & del_utf8_bom(string & src);

	bool read_file(const string & file_path, ByteBuffer & data);
	bool write_file(const string & file_path, const char * p_data, const size_t len);

	
};


#endif //TEXT_HANDLER_H
