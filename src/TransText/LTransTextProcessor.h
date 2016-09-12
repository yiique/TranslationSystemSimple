#ifndef L_TRANS_TEXT_PROCESSOR_H
#define L_TRANS_TEXT_PROCESSOR_H


//Local 本地文件处理器 只处理txt  用于即时翻译


//MAIN
#include "EventEngine/EventEngine.h"
#include "TransText.h"
#include "TextProcReq.h"
#include "TextProcRes.h"
#include "NetDriver/NetInterface.h"
#include "Common/BasicType.h"
#include "Common/MsgType.h"
#include "TextParser/TxtParser.h"
#include "LanguageTools/CLanguage.h"
#include "Common/ErrorCode.h"

//STL
#include <map>
#include <utility>
#include <string>
#include <deque>
using namespace std;


class LTransTextProcessor
	: public EventEngine
{
public:
	LTransTextProcessor(void);
	~LTransTextProcessor(void);

	static LTransTextProcessor * GetInstance();

	bool LocalProcess(TransText * p_text, 
					   const TransType trans_type,
					   bool need_language, 
					   const CallID & cid,
					   EventEngine * p_caller);

	//重写基类start函数
	bool Start();

private:

	static LTransTextProcessor ms_instance;

	//实现基类事件处理函数
	void on_event(Event & e);

	//事件处理函数
	void on_submit_process(EventData * p_edata, EventEngine * p_caller);
	
};




#endif //L_TRANS_TEXT_PROCESSOR_H
