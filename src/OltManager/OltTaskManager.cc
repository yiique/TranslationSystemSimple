#include "OltTaskManager.h"


OltTaskManager theOltTaskManager;

OltTaskManager * OltTaskManager::GetInstance()
{
	return &theOltTaskManager;
}

OltTaskManager::OltTaskManager(void)
{
}

OltTaskManager::~OltTaskManager(void)
{
}


bool OltTaskManager::Start()
{
	//必须在引擎启动之前做 否则会有线程安全问题
	load_unfinish_text();


	if( !EventEngine::Start() )
		return false;

	lout << " OltTaskManager this_ptr = " << (unsigned long) this << endl;


	return true;
}

void OltTaskManager::on_event(Event & e)
{
	//lout<< "--> OltTaskManager::on_event() start." << endl;

	if(!e._p_edata)
	{
		lerr << "ERROR: e.data is null." << endl;
		return;
	}

	if(!e._p_caller)
	{
		lerr << "ERROR: e.caller is null." << endl;
		return;
	}

	//强制转换为EventData
	EventData * p_edata = e._p_edata;

	EDType type = p_edata->GetType();


	switch(type)
	{
	case EDTYPE_OLT_SUBMIT_REQ :
		on_olt_submit(p_edata, e._p_caller);
		break;
	case EDTYPE_PREPROC_RES :
		on_preprocess_end(p_edata);
		break;
	case EDTYPE_POSTPROC_RES :
		on_postprocess_end(p_edata);
		break;
	case EDTYPE_LOAD_TEXT_RES:
		on_load_file_end(p_edata);
		break;
	case EDTYPE_TRANS_RESULT:
		on_trans_end(p_edata);
		break;
	case EDTYPE_SUBMIT_RESULT_RES: 
		on_submit_result_end(p_edata);
		break;
	// 删除时不需要通知离线翻译管理器 因为插入队列成功后 session已经删除

	default :
		lerr << "ERROR: unmatch event type = " << type << endl;
		break;
	};


	//销毁eventdata
	delete p_edata;

}




void OltTaskManager::on_olt_submit(EventData * p_edata, EventEngine * p_caller)
{
	if(!p_edata || !p_caller)
	{
		lerr << "ERROR: input is null." << endl;
		return;
	}

	//转换为OltSubmitReq
	OltSubmitReq * p_submit_req = dynamic_cast<OltSubmitReq*>(p_edata);

	if(!p_submit_req)
	{
		lerr << "ERROR: convert to OltSubmitReq failed." << endl;
		return;
	}

	//生成回复
	const vector<TextID> & textid_vec = p_submit_req->GetTextIDVec();
	vector<int> result_vec;

	//遍历建立transaction
	for(size_t i=0; i<textid_vec.size(); ++i)
	{
		//建立离线翻译事务
		const TextID & curr_text_id = textid_vec[i];
		const CallID cid = this->generate_call_id(); //产生一个callid用于标识此事务


		transaction_map_t::iterator iter = create_transaction(curr_text_id,
															  cid,
															  p_submit_req->GetUsrID(),
															  p_submit_req->GetDomainName(),
															  p_submit_req->GetSrcLanguage(),
															  p_submit_req->GetTgtLanguage());

		if(iter != m_transaction_map.end())
		{
			//载入待翻译文件
			OltTransaction & transaction = iter->second;
			TextHandler::GetInstance()->LoadTransText(transaction.p_tmp_trans_text, cid, this);

			//设置当前状态
			transaction._proc_state = OLT_PROC_LOAD;

			//设置回复的值
			result_vec.push_back(SUCCESS);

		}else
		{
			//设置回复的值
			result_vec.push_back(ERR_CREATE_OLT_TRANSACTION);
		}
	}

	//返回响应
	OltSubmitRes * p_submit_res = new OltSubmitRes(p_submit_req->GetCallID(), result_vec);
	Event e(p_submit_res, this);
	p_caller->PostEvent(e);

}

void OltTaskManager::on_trans_end(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input event data is null." << endl;
		return ;
	}

	TransResult * p_trans_result = dynamic_cast<TransResult*>(p_edata);

	if(!p_trans_result)
	{
		lerr << "Convert to TransResult failed." << endl;
		return ;
	}

	if(!p_trans_result->GetTransText())
	{	
		lerr << "TransResult's text is null." << endl;
		return ;
	}

	//查找对应的OltTransaction
	TransText * p_trans_text = p_trans_result->GetTransText();

	if(!p_trans_text)
	{
		lerr << "TextProcRes's transtext is null. ignore." << endl;
		return;
	}

	//find in map
	map<TextID, CallID>::iterator tc_iter = m_tid_cid_map.find(p_trans_text->GetID());

	if(tc_iter == m_tid_cid_map.end())
	{
		//未找到此transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = " << p_trans_text->GetID() << "   , cid = " << p_trans_result->GetCallID() << endl;
		delete p_trans_text; //删除响应中的TransText数据
	}

	transaction_map_t::iterator iter = m_transaction_map.find(tc_iter->second);

	if(iter == m_transaction_map.end())
	{
		//未找到此transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = " << p_trans_text->GetID() << "   , cid = " << p_trans_result->GetCallID() << endl;
		delete p_trans_text; //删除响应中的TransText数据
		return;
	
	}

	//发送给后处理生成器进行结果提交
	if(SUCCESS == p_trans_result->GetResultCode())
	{
		PostProcReq *p_proc_req = new PostProcReq(iter->second._sess_cid, p_trans_text);
		Event e(p_proc_req, this);

		PostProcessor::GetInstance()->PostEvent(e);

		iter->second._proc_state = OLT_PROC_POSTPROC;

	}else
	{
		on_error(iter, p_trans_result->GetResultCode());
	}


}

void OltTaskManager::on_postprocess_end(EventData * p_edata)
{
	//lout<< "--> OltTaskManager::on_postprocess_end()." << endl;

	if(!p_edata)
	{
		lerr << "ERROR: input event data is null." << endl;
		return ;
	}

	PostProcRes * p_proc_res = dynamic_cast<PostProcRes*>(p_edata);
	if(!p_proc_res)
	{
		lerr << "ERROR: convert to PostProcRes failed." << endl;
		return ;
	}

	//查找对应的OltTransaction
	const CallID & cid = p_proc_res->GetCallID();
	TransText * p_trans_text = p_proc_res->GetTransText();
	transaction_map_t::iterator iter = m_transaction_map.find(cid);

	if(iter == m_transaction_map.end())
	{
		//未找到此transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = [" << p_trans_text->GetID() << "]   , cid = " << p_proc_res->GetCallID() << endl;
		delete p_trans_text; //删除响应中的TransText数据
		return;
	}

	//获得OltTransaction的引用
	OltTransaction & transaction = iter->second;

	//判别状态
	if(OLT_PROC_POSTPROC != transaction._proc_state)
	{
		lerr << "ERROR: transaction state is not match . curr state = " << transaction._proc_state << endl;
		on_error(iter, ERR_INTER_STATE);
		return;
	}

	//判断结果
	int result = p_proc_res->GetResult();
	

	if(result != SUCCESS)
	{
		//向数据库报告状态失败才报告此状态
		//DBConnector::ReportPreProc(p_trans_text->GetID(), (ErrorCode)result);
		lerr << "ERROR: result is mean failed. res = " << result << endl;
		return on_error(iter, result);
	}

	//发送给文件处理器进行提交
	TextHandler::GetInstance()->SubmitResult(p_trans_text, transaction._sess_cid, this);
	transaction._proc_state  = OLT_PROC_SUBMIT_RESULT;

}

void OltTaskManager::on_submit_result_end(EventData * p_edata)
{

	if(!p_edata)
	{
		lerr << "ERROR: input event data is null." << endl;
		return ;
	}

	TextSubmitRes * p_submit_res = dynamic_cast<TextSubmitRes*>(p_edata);
	if(!p_submit_res)
	{
		lerr << "ERROR: convert to TextCreateRes failed." << endl;
		return ;
	}


	//查找对应的OltTransaction
	const CallID & cid = p_submit_res->GetCallID();

	//find in map
	transaction_map_t::iterator iter = m_transaction_map.find(cid);

	if(iter == m_transaction_map.end())
	{
		//未找到此transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete.  cid = " << p_submit_res->GetCallID() << endl;
		return;
	}


	if(SUCCESS == p_submit_res->GetResult())
	{
		on_success(iter);
	}else
	{
		on_error(iter, p_submit_res->GetResult());
	}
}


void OltTaskManager::on_load_file_end(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "ERROR: input event data is null." << endl;
		return ;
	}

	TextLoadRes * p_load_res = dynamic_cast<TextLoadRes*>(p_edata);
	if(!p_load_res)
	{
		lerr << "ERROR: convert to TextLoadRes failed." << endl;
		return ;
	}


	//查找对应的OltTransaction
	const CallID & cid = p_load_res->GetCallID();
	TransText * p_trans_text = p_load_res->GetTransText();

	if(!p_trans_text)
	{
		lerr << "TextProcRes's transtext is null. ignore." << endl;
		return;
	}

	//find in map
	transaction_map_t::iterator iter = m_transaction_map.find(cid);
	lout << "cid = " << cid << endl;
 
	if(iter == m_transaction_map.end())
	{
		//未找到此transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = " << p_trans_text->GetID() << "   , cid = " << p_load_res->GetCallID() << endl;
		delete p_trans_text; //删除响应中的TransText数据
		return;
	}

	//获得OltTransaction的引用
	OltTransaction & transaction = iter->second;

	//判别状态
	if(OLT_PROC_LOAD != transaction._proc_state)
	{
		lerr << "ERROR: transaction state is not match . curr state = " << transaction._proc_state << endl;
		on_error(iter, ERR_INTER_STATE); //TODO 设置错误代码
		return;
	}

	//判断结果
	int result = p_load_res->GetResult();

	//报告数据库
	//DBConnector::ReportTextProc(p_trans_text->GetID(), p_trans_text->GetDomain().second, result);

	if(result != SUCCESS)
	{
		lerr << "ERROR: result is mean failed. res = " << result << endl;
		on_error(iter, result);
		return;
	}
	
	//发送预处理
	lout << "transaction._sess_cid = " << transaction._sess_cid << endl;
	PreProcessor::PostOLTProcess(p_trans_text, transaction._sess_cid, this);

	transaction._proc_state = OLT_PROC_PREPORC;

	
}


void OltTaskManager::on_preprocess_end(EventData * p_edata)
{
	//lout<< "--> OltTaskManager::on_preprocess_end()." << endl;

	if(!p_edata)
	{
		lerr << "ERROR: input event data is null." << endl;
		return ;
	}

	PreProcRes * p_proc_res = dynamic_cast<PreProcRes*>(p_edata);
	if(!p_proc_res)
	{
		lerr << "ERROR: convert to PreProcRes failed." << endl;
		return ;
	}

	//查找对应的OltTransaction
	const CallID & cid = p_proc_res->GetCallID();
	TransText * p_trans_text = p_proc_res->GetTransText();
	transaction_map_t::iterator iter = m_transaction_map.find(cid);

	if(iter == m_transaction_map.end())
	{
		//未找到此transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = [" << p_trans_text->GetID() << "]   , cid = " << p_proc_res->GetCallID() << endl;
		delete p_trans_text; //删除响应中的TransText数据
		return;
	}

	//获得OltTransaction的引用
	OltTransaction & transaction = iter->second;

	//判别状态
	if(OLT_PROC_PREPORC != transaction._proc_state)
	{
		lerr << "ERROR: transaction state is not match . curr state = " << transaction._proc_state << endl;
		on_error(iter, ERR_INTER_STATE);
		return;
	}

	//判断结果
	int result = p_proc_res->GetResult();
	

	if(result != SUCCESS)
	{
		//向数据库报告状态失败才报告此状态
		//DBConnector::ReportPreProc(p_trans_text->GetID(), (ErrorCode)result);
		lerr << "ERROR: result is mean failed. res = " << result << endl;
		return on_error(iter, result);
	}

	
	//发送给翻译控制器
	TransController::GetOltInstance()->PostTransSubmit(p_trans_text);  //这里发给文件翻译队列
	transaction._proc_state = OLT_PROC_TRANS;


}

pair<OltTaskManager::transaction_map_t::iterator, bool> 
OltTaskManager::insert_transaction(const CallID & cid, const TextID & tid, const OltTransaction & transaction)
{
	pair<transaction_map_t::iterator, bool> res = m_transaction_map.insert(make_pair(cid, transaction));

	if(true == res.second)
	{
		pair<map<TextID, CallID>::iterator, bool> res2 = m_tid_cid_map.insert(make_pair(tid, cid));

		if(true == res.second )
			return make_pair(res.first, true);
	}

	return make_pair(m_transaction_map.end(), false);
}

OltTaskManager::transaction_map_t::iterator 
OltTaskManager::create_transaction(const TextID & tid,
								   const CallID & cid,
								   const UsrID  & usrid,
								   const string & domain_name,
								   const string & src_language,
								   const string & tgt_language)
{
		OltTransaction tmp_transa;

		pair<transaction_map_t::iterator, bool> res = insert_transaction(cid, tid, tmp_transa);

		if(true == res.second)
		{
			//建立事务成功 则开始处理，从数据库提取文件
			OltTransaction & transaction = (res.first)->second;

			//设置文件ID, 领域， transtext为0！一定要设为0，由on_error以此来判断是否删除
			transaction._sess_cid = cid;
			transaction._text_id = tid;
			transaction._domain_name = domain_name;
			transaction._src_language = src_language;
			transaction._tgt_language = tgt_language;
			transaction._usr_id = usrid;

			//生成TransText用于装载数据库的数据
			transaction.p_tmp_trans_text = new TransText(TRANS_TYPE_OLT, tid);

			transaction.p_tmp_trans_text->SetUsrID(usrid);
			
			//设置领域和语言方向
			transaction.p_tmp_trans_text->SetDomainName(domain_name);//设置领域
			transaction.p_tmp_trans_text->SetSrcLanguage(src_language); //设置源语言
			transaction.p_tmp_trans_text->SetTgtLanguage(tgt_language); //设置目标语言

			//ldbg1 << "TransText Domain = " << transaction.p_tmp_trans_text->GetDomain() << endl;

			return res.first;

			
		}else
		{
			lwrn << "Same cid when create transaction. cid = " << cid << endl;

			return m_transaction_map.end();
		}

		
}


void OltTaskManager::on_success(transaction_map_t::iterator iter)
{
	//lout<< "--> OltTaskManager::on_suc() start." << endl;

	//先输出Transaction信息
	OltTransaction & transaction = iter->second;

	/*ldbg1 << "transaction suc. text_id = " << transaction._text_id << endl;
	ldbg1 << "             domain_name = " << transaction._domain_name << endl;
	ldbg1 << "                  usr_id = " << transaction._usr_id << endl;*/


	//删除此transaction
	//删除TransText
	if(transaction.p_tmp_trans_text)
		delete transaction.p_tmp_trans_text;

	m_tid_cid_map.erase(iter->second._text_id);
	this->release_call_id(transaction._sess_cid); //回收callid
	m_transaction_map.erase(iter);
	

}


void OltTaskManager::on_error(transaction_map_t::iterator iter, int err_code)
{
	//lout<< "--> OltTaskManager::on_error() start." << endl;

	//先输出Transaction信息
	OltTransaction & transaction = iter->second;

	lerr<< "ERROR: transaction failed. text_id = " << transaction._text_id << endl;
	lerr<< "                       domain_name = " << transaction._domain_name << endl;
	lerr<< "                            usr_id = " << transaction._usr_id << endl;
	lerr<< "                             error = " << err_code << endl;

	//报告数据库
	if(err_code == ERR_PERMISSION_TIME || err_code == ERR_PERMISSION_CHARACTER)
        DBOperation::ReportTransError(transaction._text_id, err_code, TRANS_STATE_PERMISSION);
    else
        DBOperation::ReportTransError(transaction._text_id, err_code, TRANS_STATE_CANCEL);

	//删除TransText
	if(transaction.p_tmp_trans_text)
		delete transaction.p_tmp_trans_text;

	//删除此transaction
	m_tid_cid_map.erase(transaction._text_id);
	this->release_call_id(transaction._sess_cid); //回收callid
	m_transaction_map.erase(iter);
}

bool OltTaskManager::load_unfinish_text()
{
	//todo
	lout << "Disable load unfinish text." << endl;
	return true;
}
//{
//	lout << "Load unfinish text from db..." << endl;
//	//TODO 
//	//读取数据库
//	vector<DBTextInfo> text_info_vec;
//	if( false == DBConnector::ReadUnFinishTextID(text_info_vec) )
//	{
//		lwrn << "Read un finish text from db failed." << endl;
//		return false;
//	}
//
//	//重建transaction
//	for(size_t i=0; i<text_info_vec.size(); ++i)
//	{
//		//建立离线翻译事务
//		const TextID & curr_text_id = text_info_vec[i].tid;
//		const CallID cid = this->generate_call_id(); //产生一个callid用于标识此事务
//
//
//		transaction_map_t::iterator iter = create_transaction(curr_text_id,
//															  cid,
//															  text_info_vec[i].usrid,
//															  text_info_vec[i].domain_name,
//															  text_info_vec[i].src_language,
//															  text_info_vec[i].tgt_language,
//															  text_info_vec[i].dict_id_vec,
//															  text_info_vec[i].tplt_id_vec);
//
//		if(iter != m_transaction_map.end())
//		{
//			//由远程文本处理器读取
//			OltTransaction & transaction = iter->second;
//
//			bool need_language;
//			if(LANGUAGE_CHINESE == transaction._src_language || LANGUAGE_ENGLISH == transaction._src_language)
//				need_language = false;
//			else
//				need_language = true;
//			TransTextProcessor::GetInstance()->RemoteProcess(transaction.p_tmp_trans_text, TRANS_TYPE_OLT, need_language, cid, this);
//
//			//设置当前状态
//			transaction._proc_state = OLT_PROC_REMOTE_TEXT;
//
//			//ldbg1 << "Reload text " << curr_text_id << " success." << endl;
//		}
//	}
//
//	lout << "Load unfinish text over." << endl;
//
//	return true;
//}
