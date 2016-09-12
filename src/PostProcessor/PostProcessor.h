#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H


//MAIN
#include "EventEngine/EventEngine.h"
#include "PostProcEvent.h"
#include "Log/LogStream.h"
#include "Common/ErrorCode.h"

class PostProcessor
	: public EventEngine
{
public:
	PostProcessor(void);
	~PostProcessor(void);

	static PostProcessor * GetInstance();	

	bool Start(const size_t num);

private:

	static PostProcessor ms_instance;
	
	//实现基类事件处理函数
	void on_event(Event & e);

	//预处理
	EventData * on_post_process(EventData * p_edata);

	bool post_process(TransText & trans_text);

	bool post_process_handler(SentProcCell & sent_cell);
};

#endif //POST_PROCESSOR_H

