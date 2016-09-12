#include "GeneralOperation.h"
#include "LanguageTools/chinese/ChSegmentManager.h"

GeneralOperation GeneralOperation::ms_instance;


GeneralOperation * GeneralOperation::GetInstance()
{
	return &ms_instance;
}

GeneralOperation::GeneralOperation(void)
{
}

GeneralOperation::~GeneralOperation(void)
{
}

bool GeneralOperation::Start(const size_t num)
{

	if( !EventEngine::Start(num) )
		return false;

	lout << " GeneralOperation this_ptr = " << (unsigned long) this << " thread num = " << num << endl;
	return true;
}

void GeneralOperation::on_event(Event & e)
{
	//lout<< "--> GeneralOperation::on_event()." << endl;

	if(!e._p_edata)
	{
		lerr << "GeneralOperation::on_event() e._data is null." << endl;
		return;
	}

	if(!e._p_caller)
	{
		lerr << "GeneralOperation::on_event() e.caller is null." << endl;
		delete e._p_edata;
		return;
	}


	const EDType e_type = e._p_edata->GetType();
	EventData * p_edata_re = NULL;

	
	switch(e_type)
	{
	case EDTYPE_CH_SEG_INSERT_REQ:
		p_edata_re =  on_chseg_insert( dynamic_cast<InsertChSegWordReq*>(e._p_edata) );
		break;
	case EDTYPE_CH_SEG_DELETE_REQ:
		p_edata_re =  on_chseg_delete( dynamic_cast<DeleteChSegWordReq*>(e._p_edata) );
		break;
	case EDTYPE_CH_SEG_MODIFY_REQ:
		p_edata_re =  on_chseg_modify( dynamic_cast<ModifyChSegWordReq*>(e._p_edata) );
		break;
	case EDTYPE_CH_SEG_LOAD_REQ:
		p_edata_re =  on_chseg_load( dynamic_cast<LoadChSegWordReq*>(e._p_edata) );
		break;
	case EDTYPE_CH_SEG_RECOVERY_REQ:
		p_edata_re =  on_chseg_recovery( dynamic_cast<RecoveryChSegWordReq*>(e._p_edata) );
		break;
	case EDTYPE_AFTER_DICT_CREATE_REQ:
		p_edata_re = on_create_dict( dynamic_cast<AfterDictCreateReq*>(e._p_edata) );
		break;
	case EDTYPE_AFTER_DICT_MODIFY_REQ:
		p_edata_re = on_modify_dict( dynamic_cast<AfterDictModifyReq*>(e._p_edata) );
		break;
	case EDTYPE_AFTER_DICT_DELETE_REQ:
		p_edata_re = on_delete_dict( dynamic_cast<AfterDictDeleteReq*>(e._p_edata) );
		break;
	case EDTYPE_AFTER_DICT_WORDS_INSERT_REQ:
		p_edata_re = on_insert_word( dynamic_cast<AfterWordInsertReq*>(e._p_edata) );
		break;
	case EDTYPE_AFTER_DICT_WORDS_DELETE_REQ:
		p_edata_re = on_delete_word( dynamic_cast<AfterWordDeleteReq*>(e._p_edata) );
		break;
	case EDTYPE_AFTER_DICT_WORDS_MODIFY_REQ:
		p_edata_re = on_modify_word( dynamic_cast<AfterWordModifyReq*>(e._p_edata) );
		break;
	case EDTYPE_AFTER_DICT_WORDS_LOAD_REQ:
		p_edata_re = on_load_word( dynamic_cast<AfterWordLoadReq*>(e._p_edata) );
		break;
	case EDTYPE_DICT_AFTER_RECOVER_REQ:
		p_edata_re = on_dict_recover( dynamic_cast<AfterDictRecoverReq*>(e._p_edata) );
		break;
	case EDTYPE_DICT_AFTER_WORDS_RECOVER_REQ:
		p_edata_re = on_word_recover( dynamic_cast<AfterWordRecoverReq*>(e._p_edata) );
		break;
	default:
		lerr << "GeneralOperation Can't handle this event , type = " << e_type << endl;
	}
	
	if(p_edata_re)
	{
		//返回结果
		Event re(p_edata_re, this);
		e._p_caller->PostEvent(re);
	}else
	{
		lerr << "GeneralOperation handle faield. set p_edata_re is NULL." << endl;
	}
	
	//销毁Req
	delete e._p_edata;

}

InsertChSegWordRes * GeneralOperation::on_chseg_insert(InsertChSegWordReq * p_req)
{
	assert(p_req);

	InsertChSegWordRes * p_res = new InsertChSegWordRes(p_req->GetCallID());

	if(ChSegmentManager::GetInstance()->InsertSegWord(p_req->GetWordVec()).first == true )
	{
		p_res->SetResult(SUCCESS);
	}else
	{
		p_res->SetResult(ERR_CHSEG_INSERT);
	}
	
	return p_res;
}

DeleteChSegWordRes * GeneralOperation::on_chseg_delete(DeleteChSegWordReq * p_req)
{
	assert(p_req);

	DeleteChSegWordRes * p_res = new DeleteChSegWordRes(p_req->GetCallID());

	if(ChSegmentManager::GetInstance()->DeleteSegWord(p_req->GetWordIdVec()) > 0 )
	{
		p_res->SetResult(SUCCESS);
	}else
	{
		p_res->SetResult(ERR_CHSEG_DELETE);
	}
	
	return p_res;
}

ModifyChSegWordRes * GeneralOperation::on_chseg_modify(ModifyChSegWordReq * p_req)
{
	assert(p_req);

	ModifyChSegWordRes * p_res = new ModifyChSegWordRes(p_req->GetCallID());

	if(ChSegmentManager::GetInstance()->ModifySegWord(p_req->GetWord()) == true )
	{
		p_res->SetResult(SUCCESS);
	}else
	{
		p_res->SetResult(ERR_CHSEG_MODIFY);
	}
	
	return p_res;
}

LoadChSegWordRes * GeneralOperation::on_chseg_load(LoadChSegWordReq * p_req)
{
	assert(p_req);

	LoadChSegWordRes * p_res = new LoadChSegWordRes(p_req->GetCallID());

	if(ChSegmentManager::GetInstance()->InsertSegWord(p_req->GetFileName(), p_req->IsActive(), p_req->IsUTF8()).first == true )
	{
		p_res->SetResult(SUCCESS);
	}else
	{
		p_res->SetResult(ERR_CHSEG_LOAD);
	}
	
	return p_res;
}

RecoveryChSegWordRes * GeneralOperation::on_chseg_recovery(RecoveryChSegWordReq * p_req)
{
	assert(p_req);

	RecoveryChSegWordRes * p_res = new RecoveryChSegWordRes(p_req->GetCallID());

	if(ChSegmentManager::GetInstance()->RecoverySegWord(p_req->GetWordIdVec()) > 0 )
	{
		p_res->SetResult(SUCCESS);
	}else
	{
		p_res->SetResult(ERR_CHSEG_RECOVERY);
	}
	
	return p_res;
}

AfterDictCreateRes * GeneralOperation::on_create_dict(AfterDictCreateReq * p_req)
{
	assert(p_req);

	AfterDictCreateRes * p_res = new AfterDictCreateRes(p_req->GetCallID());

	//创建词典
	AfterDictInfo dict_info = p_req->GetDictInfo();

	int ret = AfterTreatManager::GetInstance()->CreateDict(dict_info);

	//处理完毕
	p_res->SetRes(ret);

	return p_res;
	
	return p_res;
}

AfterDictModifyRes * GeneralOperation::on_modify_dict(AfterDictModifyReq * p_req)
{
	assert(p_req);

	//生成res
	AfterDictModifyRes * p_res = new AfterDictModifyRes(p_req->GetCallID());

	//修改词典
	int ret = AfterTreatManager::GetInstance()->ModifyDict(p_req->GetDictInfo());

	//处理完毕
	p_res->SetRes(ret);

	return p_res;
}

AfterDictDeleteRes * GeneralOperation::on_delete_dict(AfterDictDeleteReq * p_req)
{
	assert(p_req);

	//生成res
	AfterDictDeleteRes * p_res = new AfterDictDeleteRes(p_req->GetCallID());

	//删除词典
	set<AfterDictID>::const_iterator iter = p_req->GetDictIDList().begin();
	for(; iter != p_req->GetDictIDList().end(); ++iter)
	{
		//lout << "Del dict id = " << *iter << endl;
		int ret = AfterTreatManager::GetInstance()->DeleteDict(*iter);

		if(SUCCESS != ret)
			p_res->AddFailure(*iter, ret);
	}

	//处理完毕
	return p_res;
}

AfterWordInsertRes * GeneralOperation::on_insert_word(AfterWordInsertReq * p_req)
{
	assert(p_req);

	//生成res
	AfterWordInsertRes * p_res = new AfterWordInsertRes(p_req->GetCallID());

	//插入词条
	int ret = AfterTreatManager::GetInstance()->InsertWord(p_req->GetDictID(), p_req->GetWordInfo());

	//处理完毕
	p_res->SetRes(ret);

	return p_res;
}

AfterWordModifyRes * GeneralOperation::on_modify_word(AfterWordModifyReq * p_req)
{
	assert(p_req);

	//生成res
	AfterWordModifyRes * p_res = new AfterWordModifyRes(p_req->GetCallID());

	//修改词条
	int ret = AfterTreatManager::GetInstance()->ModifyWord(p_req->GetDictID(), p_req->GetWordInfo());

	//处理完毕
	p_res->SetRes(ret);

	return p_res;
}

AfterWordDeleteRes * GeneralOperation::on_delete_word(AfterWordDeleteReq * p_req)
{
	assert(p_req);

	//生成res
	AfterWordDeleteRes * p_res = new AfterWordDeleteRes(p_req->GetCallID());

	//删除词条
	int ret = AfterTreatManager::GetInstance()->DeleteWord(p_req->GetDictID(), p_req->GetWordID());

	//处理完毕
	p_res->SetRes(ret);

	return p_res;
}

AfterWordLoadRes * GeneralOperation::on_load_word(AfterWordLoadReq * p_req)
{
	assert(p_req);

	//生成res
	AfterWordLoadRes * p_res = new AfterWordLoadRes(p_req->GetCallID());

	//插入词条
	int ret = AfterTreatManager::GetInstance()->LoadWordFile(p_req->GetDictID(), p_req->GetWordFileName(), p_req->GetIsActive(), p_req->IsUTF8());

	//处理完毕
	p_res->SetRes(ret);

	return p_res;
}

AfterDictRecoverRes * GeneralOperation::on_dict_recover(AfterDictRecoverReq * p_req)
{
	assert(p_req);

	//生成res
	AfterDictRecoverRes * p_res = new AfterDictRecoverRes(p_req->GetCallID());

	//插入词条
	int ret = AfterTreatManager::GetInstance()->RecoverDict(p_req->GetDictID());

	//处理完毕
	p_res->SetRes(ret);

	return p_res;
}

AfterWordRecoverRes * GeneralOperation::on_word_recover(AfterWordRecoverReq * p_req)
{
	assert(p_req);

	//生成res
	AfterWordRecoverRes * p_res = new AfterWordRecoverRes(p_req->GetCallID());

	//插入词条
	int ret = AfterTreatManager::GetInstance()->RecoverWord(p_req->GetDictID(), p_req->GetWordID());

	//处理完毕
	p_res->SetRes(ret);

	return p_res;
}
