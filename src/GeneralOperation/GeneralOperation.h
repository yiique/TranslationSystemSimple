#ifndef GENERAL_OPERATION_H
#define GENERAL_OPERATION_H


//SYS
#include <assert.h>

//MAIN
#include "EventEngine/EventEngine.h"
#include "Log/LogStream.h"
#include "Common/ErrorCode.h"

#include "LanguageTools/chinese/ChSegmentEvent.h"
#include "PostProcessor/AfterTreatment/AfterTreatEvent.h"
#include "PostProcessor/AfterTreatment/AfterTreatManager.h"

class GeneralOperation
	: public EventEngine
{
public:
	GeneralOperation(void);
	~GeneralOperation(void);

	static GeneralOperation * GetInstance();

	bool Start(const size_t num);

private:

	static GeneralOperation ms_instance;

	void on_event(Event & e);

	//中文分词词典
	InsertChSegWordRes * on_chseg_insert(InsertChSegWordReq * p_req);
	DeleteChSegWordRes * on_chseg_delete(DeleteChSegWordReq * p_req);
	ModifyChSegWordRes * on_chseg_modify(ModifyChSegWordReq * p_req);
	LoadChSegWordRes * on_chseg_load(LoadChSegWordReq * p_req);
	RecoveryChSegWordRes * on_chseg_recovery(RecoveryChSegWordReq * p_req);

	//后处理词典
	AfterDictCreateRes * on_create_dict(AfterDictCreateReq * p_req);
	AfterDictModifyRes * on_modify_dict(AfterDictModifyReq * p_req);
	AfterDictDeleteRes * on_delete_dict(AfterDictDeleteReq * p_req);
	AfterWordInsertRes * on_insert_word(AfterWordInsertReq * p_req);
	AfterWordModifyRes * on_modify_word(AfterWordModifyReq * p_req);
	AfterWordDeleteRes * on_delete_word(AfterWordDeleteReq * p_req);
	AfterWordLoadRes * on_load_word(AfterWordLoadReq * p_req);
	AfterDictRecoverRes * on_dict_recover(AfterDictRecoverReq * p_req);
	AfterWordRecoverRes * on_word_recover(AfterWordRecoverReq * p_req);

};


#endif //GENERAL_OPERATION_H
