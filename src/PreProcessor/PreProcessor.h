#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H

/*
	这里是前处理部分
	关键词：前处理，函数绑定，前处理函数
*/

//MAIN
#include "EventEngine/EventEngine.h"
#include "PreProcReq.h"
#include "PreProcRes.h"
#include "Log/LogStream.h"
#include "Common/ErrorCode.h"

class PreProcessor
	: public EventEngine
{
public:
	PreProcessor(void);
	~PreProcessor(void);

	static bool StartGroup(const size_t rtt_num, const size_t olt_num);

	static void PostRTTProcess(TransText * p_trans_text, const CallID cid, EventEngine * p_caller);
	static void PostOLTProcess(TransText * p_trans_text, const CallID cid, EventEngine * p_caller);

	bool pre_process(TransText & trans_text);

private:


	static PreProcessor ms_rtt_instance;
	static PreProcessor ms_olt_instance;

	
	//实现基类事件处理函数
	void on_event(Event & e);

	//预处理
	EventData * on_pre_process(EventData * p_edata);

};

#endif //PRE_PROCESSOR_H

