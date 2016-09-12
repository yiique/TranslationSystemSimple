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
	//��������������֮ǰ�� ��������̰߳�ȫ����
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

	//ǿ��ת��ΪEventData
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
	// ɾ��ʱ����Ҫ֪ͨ���߷�������� ��Ϊ������гɹ��� session�Ѿ�ɾ��

	default :
		lerr << "ERROR: unmatch event type = " << type << endl;
		break;
	};


	//����eventdata
	delete p_edata;

}




void OltTaskManager::on_olt_submit(EventData * p_edata, EventEngine * p_caller)
{
	if(!p_edata || !p_caller)
	{
		lerr << "ERROR: input is null." << endl;
		return;
	}

	//ת��ΪOltSubmitReq
	OltSubmitReq * p_submit_req = dynamic_cast<OltSubmitReq*>(p_edata);

	if(!p_submit_req)
	{
		lerr << "ERROR: convert to OltSubmitReq failed." << endl;
		return;
	}

	//���ɻظ�
	const vector<TextID> & textid_vec = p_submit_req->GetTextIDVec();
	vector<int> result_vec;

	//��������transaction
	for(size_t i=0; i<textid_vec.size(); ++i)
	{
		//�������߷�������
		const TextID & curr_text_id = textid_vec[i];
		const CallID cid = this->generate_call_id(); //����һ��callid���ڱ�ʶ������


		transaction_map_t::iterator iter = create_transaction(curr_text_id,
															  cid,
															  p_submit_req->GetUsrID(),
															  p_submit_req->GetDomainName(),
															  p_submit_req->GetSrcLanguage(),
															  p_submit_req->GetTgtLanguage());

		if(iter != m_transaction_map.end())
		{
			//����������ļ�
			OltTransaction & transaction = iter->second;
			TextHandler::GetInstance()->LoadTransText(transaction.p_tmp_trans_text, cid, this);

			//���õ�ǰ״̬
			transaction._proc_state = OLT_PROC_LOAD;

			//���ûظ���ֵ
			result_vec.push_back(SUCCESS);

		}else
		{
			//���ûظ���ֵ
			result_vec.push_back(ERR_CREATE_OLT_TRANSACTION);
		}
	}

	//������Ӧ
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

	//���Ҷ�Ӧ��OltTransaction
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
		//δ�ҵ���transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = " << p_trans_text->GetID() << "   , cid = " << p_trans_result->GetCallID() << endl;
		delete p_trans_text; //ɾ����Ӧ�е�TransText����
	}

	transaction_map_t::iterator iter = m_transaction_map.find(tc_iter->second);

	if(iter == m_transaction_map.end())
	{
		//δ�ҵ���transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = " << p_trans_text->GetID() << "   , cid = " << p_trans_result->GetCallID() << endl;
		delete p_trans_text; //ɾ����Ӧ�е�TransText����
		return;
	
	}

	//���͸��������������н���ύ
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

	//���Ҷ�Ӧ��OltTransaction
	const CallID & cid = p_proc_res->GetCallID();
	TransText * p_trans_text = p_proc_res->GetTransText();
	transaction_map_t::iterator iter = m_transaction_map.find(cid);

	if(iter == m_transaction_map.end())
	{
		//δ�ҵ���transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = [" << p_trans_text->GetID() << "]   , cid = " << p_proc_res->GetCallID() << endl;
		delete p_trans_text; //ɾ����Ӧ�е�TransText����
		return;
	}

	//���OltTransaction������
	OltTransaction & transaction = iter->second;

	//�б�״̬
	if(OLT_PROC_POSTPROC != transaction._proc_state)
	{
		lerr << "ERROR: transaction state is not match . curr state = " << transaction._proc_state << endl;
		on_error(iter, ERR_INTER_STATE);
		return;
	}

	//�жϽ��
	int result = p_proc_res->GetResult();
	

	if(result != SUCCESS)
	{
		//�����ݿⱨ��״̬ʧ�ܲű����״̬
		//DBConnector::ReportPreProc(p_trans_text->GetID(), (ErrorCode)result);
		lerr << "ERROR: result is mean failed. res = " << result << endl;
		return on_error(iter, result);
	}

	//���͸��ļ������������ύ
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


	//���Ҷ�Ӧ��OltTransaction
	const CallID & cid = p_submit_res->GetCallID();

	//find in map
	transaction_map_t::iterator iter = m_transaction_map.find(cid);

	if(iter == m_transaction_map.end())
	{
		//δ�ҵ���transaction
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


	//���Ҷ�Ӧ��OltTransaction
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
		//δ�ҵ���transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = " << p_trans_text->GetID() << "   , cid = " << p_load_res->GetCallID() << endl;
		delete p_trans_text; //ɾ����Ӧ�е�TransText����
		return;
	}

	//���OltTransaction������
	OltTransaction & transaction = iter->second;

	//�б�״̬
	if(OLT_PROC_LOAD != transaction._proc_state)
	{
		lerr << "ERROR: transaction state is not match . curr state = " << transaction._proc_state << endl;
		on_error(iter, ERR_INTER_STATE); //TODO ���ô������
		return;
	}

	//�жϽ��
	int result = p_load_res->GetResult();

	//�������ݿ�
	//DBConnector::ReportTextProc(p_trans_text->GetID(), p_trans_text->GetDomain().second, result);

	if(result != SUCCESS)
	{
		lerr << "ERROR: result is mean failed. res = " << result << endl;
		on_error(iter, result);
		return;
	}
	
	//����Ԥ����
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

	//���Ҷ�Ӧ��OltTransaction
	const CallID & cid = p_proc_res->GetCallID();
	TransText * p_trans_text = p_proc_res->GetTransText();
	transaction_map_t::iterator iter = m_transaction_map.find(cid);

	if(iter == m_transaction_map.end())
	{
		//δ�ҵ���transaction
		lerr << "ERROR: can't find this id in transaction, this transtext will be delete. Text id = [" << p_trans_text->GetID() << "]   , cid = " << p_proc_res->GetCallID() << endl;
		delete p_trans_text; //ɾ����Ӧ�е�TransText����
		return;
	}

	//���OltTransaction������
	OltTransaction & transaction = iter->second;

	//�б�״̬
	if(OLT_PROC_PREPORC != transaction._proc_state)
	{
		lerr << "ERROR: transaction state is not match . curr state = " << transaction._proc_state << endl;
		on_error(iter, ERR_INTER_STATE);
		return;
	}

	//�жϽ��
	int result = p_proc_res->GetResult();
	

	if(result != SUCCESS)
	{
		//�����ݿⱨ��״̬ʧ�ܲű����״̬
		//DBConnector::ReportPreProc(p_trans_text->GetID(), (ErrorCode)result);
		lerr << "ERROR: result is mean failed. res = " << result << endl;
		return on_error(iter, result);
	}

	
	//���͸����������
	TransController::GetOltInstance()->PostTransSubmit(p_trans_text);  //���﷢���ļ��������
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
			//��������ɹ� ��ʼ���������ݿ���ȡ�ļ�
			OltTransaction & transaction = (res.first)->second;

			//�����ļ�ID, ���� transtextΪ0��һ��Ҫ��Ϊ0����on_error�Դ����ж��Ƿ�ɾ��
			transaction._sess_cid = cid;
			transaction._text_id = tid;
			transaction._domain_name = domain_name;
			transaction._src_language = src_language;
			transaction._tgt_language = tgt_language;
			transaction._usr_id = usrid;

			//����TransText����װ�����ݿ������
			transaction.p_tmp_trans_text = new TransText(TRANS_TYPE_OLT, tid);

			transaction.p_tmp_trans_text->SetUsrID(usrid);
			
			//������������Է���
			transaction.p_tmp_trans_text->SetDomainName(domain_name);//��������
			transaction.p_tmp_trans_text->SetSrcLanguage(src_language); //����Դ����
			transaction.p_tmp_trans_text->SetTgtLanguage(tgt_language); //����Ŀ������

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

	//�����Transaction��Ϣ
	OltTransaction & transaction = iter->second;

	/*ldbg1 << "transaction suc. text_id = " << transaction._text_id << endl;
	ldbg1 << "             domain_name = " << transaction._domain_name << endl;
	ldbg1 << "                  usr_id = " << transaction._usr_id << endl;*/


	//ɾ����transaction
	//ɾ��TransText
	if(transaction.p_tmp_trans_text)
		delete transaction.p_tmp_trans_text;

	m_tid_cid_map.erase(iter->second._text_id);
	this->release_call_id(transaction._sess_cid); //����callid
	m_transaction_map.erase(iter);
	

}


void OltTaskManager::on_error(transaction_map_t::iterator iter, int err_code)
{
	//lout<< "--> OltTaskManager::on_error() start." << endl;

	//�����Transaction��Ϣ
	OltTransaction & transaction = iter->second;

	lerr<< "ERROR: transaction failed. text_id = " << transaction._text_id << endl;
	lerr<< "                       domain_name = " << transaction._domain_name << endl;
	lerr<< "                            usr_id = " << transaction._usr_id << endl;
	lerr<< "                             error = " << err_code << endl;

	//�������ݿ�
	if(err_code == ERR_PERMISSION_TIME || err_code == ERR_PERMISSION_CHARACTER)
        DBOperation::ReportTransError(transaction._text_id, err_code, TRANS_STATE_PERMISSION);
    else
        DBOperation::ReportTransError(transaction._text_id, err_code, TRANS_STATE_CANCEL);

	//ɾ��TransText
	if(transaction.p_tmp_trans_text)
		delete transaction.p_tmp_trans_text;

	//ɾ����transaction
	m_tid_cid_map.erase(transaction._text_id);
	this->release_call_id(transaction._sess_cid); //����callid
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
//	//��ȡ���ݿ�
//	vector<DBTextInfo> text_info_vec;
//	if( false == DBConnector::ReadUnFinishTextID(text_info_vec) )
//	{
//		lwrn << "Read un finish text from db failed." << endl;
//		return false;
//	}
//
//	//�ؽ�transaction
//	for(size_t i=0; i<text_info_vec.size(); ++i)
//	{
//		//�������߷�������
//		const TextID & curr_text_id = text_info_vec[i].tid;
//		const CallID cid = this->generate_call_id(); //����һ��callid���ڱ�ʶ������
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
//			//��Զ���ı���������ȡ
//			OltTransaction & transaction = iter->second;
//
//			bool need_language;
//			if(LANGUAGE_CHINESE == transaction._src_language || LANGUAGE_ENGLISH == transaction._src_language)
//				need_language = false;
//			else
//				need_language = true;
//			TransTextProcessor::GetInstance()->RemoteProcess(transaction.p_tmp_trans_text, TRANS_TYPE_OLT, need_language, cid, this);
//
//			//���õ�ǰ״̬
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
