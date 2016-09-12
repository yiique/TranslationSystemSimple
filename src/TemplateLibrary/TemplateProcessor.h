#ifndef TEMPLATE_PROCESSOR_H
#define TEMPLATE_PROCESSOR_H

//MAIN
#include "EventEngine/EventEngine.h"
#include "UsrTemplateManager.h"
#include "Log/LogStream.h"
#include "TemplateEvent.h"

class TemplateLibProcessor : public EventEngine
{
public:
	~TemplateLibProcessor() {}

	static TemplateLibProcessor * GetInstance();

	bool Start(const string & default_file_path = "./");

private:

	TemplateLibProcessor(){}

	static TemplateLibProcessor ms_instance;

	//禁掉基类函数
	bool Start(void) { return false; }


	//实现基类事件处理函数
	void on_event(Event & e);

	EventData * on_create_templatelib(EventData * p_edata);
	EventData * on_modify_templatelib(EventData * p_edata);
	EventData * on_delete_templatelib(EventData * p_edata);
	EventData * on_insert_template(EventData * p_edata);
	EventData * on_delete_template(EventData * p_edata);
	EventData * on_modify_template(EventData * p_edata);
	EventData * on_load_template_file(EventData * p_edata);
	EventData * on_to_sys_templatelib(EventData * p_edata);
	EventData * on_recover_templatelib(EventData * p_edata);
	EventData * on_recover_template(EventData * p_edata);

	string m_default_file_path;

};



#endif //TEMPLATE_PROCESSOR_H
