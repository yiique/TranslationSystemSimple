#ifndef DICT_PROCESSOR_H
#define DICT_PROCESSOR_H

//MAIN
#include "EventEngine/EventEngine.h"
#include "Log/LogStream.h"
#include "DictEvent.h"

//STL
#include <utility>
using namespace std;

class DictProcessor
	: public EventEngine
{
public:
	~DictProcessor() {}

	static DictProcessor * GetInstance();

	bool Start(const string & default_file_path = "./");

private:

	DictProcessor(){}

	static DictProcessor ms_instance;

	//禁掉基类函数
	bool Start(void) { return false; }


	//实现基类事件处理函数
	void on_event(Event & e);

	EventData * on_create_dict(EventData * p_edata);
	EventData * on_modify_dict(EventData * p_edata);
	EventData * on_delete_dict(EventData * p_edata);
	EventData * on_insert_word(EventData * p_edata);
	EventData * on_delete_word(EventData * p_edata);
	EventData * on_modify_word(EventData * p_edata);
	EventData * on_load_word_file(EventData * p_edata);
	EventData * on_to_sys_dict(EventData * p_edata);
	EventData * on_recover_word(EventData * p_edata);
	EventData * on_recover_dict(EventData * p_edata);

private:
	string m_default_file_path;
	bool load_word_file(const string & file_path, const bool is_utf8, vector<pair<string, string> > & word_vec);

};


#endif //DICT_PROCESSOR_H
