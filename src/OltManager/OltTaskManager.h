#ifndef OLT_TASK_MANAGER_H
#define OLT_TASK_MANAGER_H

//MAIN
#include "EventEngine/EventEngine.h"
#include "EventEngine/EventData.h"
#include "TransText/TransText.h"
#include "OltSubmitReq.h"
#include "OltSubmitRes.h"
#include "PreProcessor/PreProcessor.h"
#include "PostProcessor/PostProcessor.h"
#include "Log/LogStream.h"
#include "TransController/TransController.h"
#include "TextHandler/TextHandler.h"


//STL
#include <iostream>
#include <string>
#include <utility>
#include <map>
using namespace std;

typedef enum
{
	OLT_PROC_PREPORC,   //0
	OLT_PROC_LOAD,    //1
	OLT_PROC_TRANS,   //2
	OLT_PROC_SUBMIT_RESULT,
	OLT_PROC_POSTPROC,
	OLT_PROC_END         
} OltProcState;


typedef struct
{
	CallID _sess_cid;  //session 的call id
	TextID _text_id;
	string _domain_name;
	string _src_language;
	string _tgt_language;
	string _usr_id;
	TransText * p_tmp_trans_text; //先保存此值，如果加入队列失败，则delete，否则不delete
	OltProcState _proc_state;

} OltTransaction;

class OltTaskManager
	: public EventEngine
{
public:
	OltTaskManager(void);
	~OltTaskManager(void);

	static OltTaskManager * GetInstance();

	bool Start();

	typedef map<CallID, OltTransaction> transaction_map_t;

private:

	//实现基类on_event
	void on_event(Event & e);

	transaction_map_t m_transaction_map;
	map<TextID, CallID> m_tid_cid_map;

	pair<transaction_map_t::iterator, bool> insert_transaction(const CallID & cid, const TextID & tid, const OltTransaction & transaction);

	void on_olt_submit(EventData * p_edata, EventEngine * p_caller); //STEP 1
	void on_load_file_end(EventData * p_edata);                    //STEP 2
	void on_preprocess_end(EventData * p_edata);                     //STEP 3    
	void on_trans_end(EventData * p_edata);  //STEP 4
	void on_postprocess_end(EventData * p_edata);
	void on_submit_result_end(EventData * p_edata);
	
	void on_error(transaction_map_t::iterator iter, int err_code);
	void on_success(transaction_map_t::iterator iter);

	//载入未翻译完成的文件
	bool load_unfinish_text();

	transaction_map_t::iterator create_transaction(const TextID & tid,
												   const CallID & cid,
												   const UsrID  & usrid,
												   const string & domain_name,
												   const string & src_language,
												   const string & tgt_language
												   );
};






#endif //OLT_TASK_MANAGER_H

