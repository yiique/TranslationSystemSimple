#include "TemplateProcessor.h"

#include "Common/ErrorCode.h"

TemplateLibProcessor TemplateLibProcessor::ms_instance;

TemplateLibProcessor * TemplateLibProcessor::GetInstance()
{
	return &ms_instance;
}

bool TemplateLibProcessor::Start(const string & default_file_path)
{
	m_default_file_path = default_file_path;

	return EventEngine::Start();
}

void TemplateLibProcessor::on_event(Event & e)
{
	if(!e._p_edata)
	{
		lerr << "e._data is null." << endl;
		return;
	}

	if(!e._p_caller)
	{
		lerr << "e.caller is null." << endl;
		delete e._p_edata;
		return;
	}


	const EDType e_type = e._p_edata->GetType();
	EventData * p_edata_re = NULL;

	switch(e_type)
	{
	case EDTYPE_TEMPLATELIB_CREATE_REQ:
        p_edata_re = on_create_templatelib(e._p_edata);
		break;
	case EDTYPE_TEMPLATELIB_MODIFY_REQ:
		p_edata_re = on_modify_templatelib(e._p_edata);
		break;
	case EDTYPE_TEMPLATELIB_DELETE_REQ:
		p_edata_re = on_delete_templatelib(e._p_edata);
		break;
	case EDTYPE_TEMPLATELIB_TEMPLATES_INSERT_REQ:
        p_edata_re = on_insert_template(e._p_edata);
		break;
	case EDTYPE_TEMPLATELIB_TEMPLATES_DELETE_REQ:
		p_edata_re = on_delete_template(e._p_edata);
		break;
	case EDTYPE_TEMPLATELIB_TEMPLATES_MODIFY_REQ:
		p_edata_re = on_modify_template(e._p_edata);
		break;
	case EDTYPE_TEMPLATELIB_TEMPLATES_LOAD_REQ:
		p_edata_re = on_load_template_file(e._p_edata);
		break;
	case EDTYPE_TEMPLATELIB_TOSYSTEM_REQ:
		p_edata_re = on_to_sys_templatelib(e._p_edata);
		break;
	case EDTYPE_TEMPLATELIB_RECOVER_REQ:
		p_edata_re = on_recover_templatelib(e._p_edata);
		break;
	case EDTYPE_TEMPLATE_RECOVER_REQ:
		p_edata_re = on_recover_template(e._p_edata);
		break;
	default:
		lerr << "TemplateProcessor can't handle this event , type = " << e_type << endl;
	}


	if(p_edata_re)
	{
		//���ؽ��
		Event re(p_edata_re, this);
		e._p_caller->PostEvent(re);
	}else
	{
		lerr << "TemplateProcessor handle faield. set p_edata_re is NULL." << endl;
	}

	//����Req
	delete e._p_edata;
}



EventData * TemplateLibProcessor::on_create_templatelib(EventData * p_edata)
{
	//lout << "on_create_templatelib ." << endl;

	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪTemplateLibCreateReq
	TemplateLibCreateReq * p_req = dynamic_cast<TemplateLibCreateReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TemplateLibCreateReq failed." << endl;
		return NULL;
	}

	//����res
	TemplateLibCreateRes * p_res = new TemplateLibCreateRes(p_req->GetCallID());

	//����ģ���
	TemplateLibInfo templatelib_info = p_req->GetTemplateLibInfo();
	//
	if(templatelib_info.usr_id == SYSMANAGER)
	{
		templatelib_info.is_system = 1;
	}
	int ret = UsrTemplateLibManager::GetInstance()->CreateTemplateLib(templatelib_info);

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * TemplateLibProcessor::on_modify_templatelib(EventData * p_edata)
{
	//lout << "on_modify_templatelib ." << endl;

	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪTemplateLibModifyReq
	TemplateLibModifyReq * p_req = dynamic_cast<TemplateLibModifyReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TemplateLibModifyfReq failed." << endl;
		return NULL;
	}

	//����res
	TemplateLibModifyRes * p_res = new TemplateLibModifyRes(p_req->GetCallID());

	//�޸�ģ���
	int ret = UsrTemplateLibManager::GetInstance()->ModifyTemplateLib(p_req->GetTemplateLibInfo());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * TemplateLibProcessor::on_delete_templatelib(EventData * p_edata)
{

	//lout << "on_delete_templatelib ." << endl;

	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪTemplateLibDeleteReq
	TemplateLibDeleteReq * p_req = dynamic_cast<TemplateLibDeleteReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TemplateLibDeleteReq failed." << endl;
		return NULL;
	}

	//����res
	TemplateLibDeleteRes * p_res = new TemplateLibDeleteRes(p_req->GetCallID());

	//ɾ��ģ���
	set<TemplateLibID>::const_iterator iter = p_req->GetTemplateLibIDList().begin();
	for(; iter != p_req->GetTemplateLibIDList().end(); ++iter)
	{
		//lout << "Del templatelib id = " << *iter << endl;
		int ret = UsrTemplateLibManager::GetInstance()->DeleteTemplateLib(*iter);

		//lout << "ret = " << ret << endl;

		if(SUCCESS != ret)
			p_res->AddFailure(*iter, ret);
	}

	//�������

	return p_res;

}

EventData * TemplateLibProcessor::on_insert_template(EventData * p_edata)
{
	//lout << "on_insert_template ." << endl;

	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪTemplateInsertReq
	TemplateInsertReq * p_req = dynamic_cast<TemplateInsertReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TemplateInsertReq failed." << endl;
		return NULL;
	}

	//����res
	TemplateInsertRes * p_res = new TemplateInsertRes(p_req->GetCallID());

	//����ģ��
	int ret = UsrTemplateLibManager::GetInstance()->InsertTemplate(p_req->GetTemplateLibID(), p_req->GetTemplateInfo());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * TemplateLibProcessor::on_modify_template(EventData * p_edata)
{

	//lout << "on_modify_template" << endl;

	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪTemplateModifyReq
	TemplateModifyReq * p_req = dynamic_cast<TemplateModifyReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TemplateModifyReq failed." << endl;
		return NULL;
	}

	//����res
	TemplateModifyRes * p_res = new TemplateModifyRes(p_req->GetCallID());

	//�޸�ģ��
	int ret = UsrTemplateLibManager::GetInstance()->ModifyTemplate(p_req->GetTemplateLibID(), p_req->GetTemplateInfo());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * TemplateLibProcessor::on_delete_template(EventData * p_edata)
{
	//lout << "on delete template ." << endl;

	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪTemplateDeleteReq
	TemplateDeleteReq * p_req = dynamic_cast<TemplateDeleteReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TemplateDeleteReq failed." << endl;
		return NULL;
	}

	//����res
	TemplateDeleteRes * p_res = new TemplateDeleteRes(p_req->GetCallID());

	//ɾ��ģ��
	int ret = UsrTemplateLibManager::GetInstance()->DeleteTemplate(p_req->GetTemplateLibID(), p_req->GetTemplateID());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * TemplateLibProcessor::on_load_template_file(EventData * p_edata)
{
	//lout << "on_load_template_file" << endl;

	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪTemplateLoadReq
	TemplateLoadReq * p_req = dynamic_cast<TemplateLoadReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TemplateLoadReq failed." << endl;
		return NULL;
	}

	//����res
	TemplateLoadRes * p_res = new TemplateLoadRes(p_req->GetCallID());

	//����ģ��
	int ret = UsrTemplateLibManager::GetInstance()->LoadTemplateFile(p_req->GetTemplateLibID(), m_default_file_path + p_req->GetTemplateFileName(), p_req->GetIsActive(), p_req->IsUTF8());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * TemplateLibProcessor::on_to_sys_templatelib(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪToSysTemplateLibReq
	ToSysTemplateLibReq * p_req = dynamic_cast<ToSysTemplateLibReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to ToSysTemplateLibReq failed." << endl;
		return NULL;
	}

	//����res
	ToSysTemplateLibRes * p_res = new ToSysTemplateLibRes(p_req->GetCallID());

	//����ϵͳ��ģ���
	int ret = UsrTemplateLibManager::GetInstance()->ToSysTemplateLib(p_req->GetSrcTemplateID(), p_req->GetTgtTemplateLibID());

	//�������
	p_res->SetRes(ret);

	return p_res;
}



EventData * TemplateLibProcessor::on_recover_templatelib(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪTemplateLibRecoverReq
	TemplateLibRecoverReq * p_req = dynamic_cast<TemplateLibRecoverReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TemplateLibRecoverReq failed." << endl;
		return NULL;
	}

	//����res
	TemplateLibRecoverRes * p_res = new TemplateLibRecoverRes(p_req->GetCallID());

	//�ָ�ģ���
	int ret = UsrTemplateLibManager::GetInstance()->RecoverTemplateLib(p_req->GetTemplateLibID());

	//�������
	p_res->SetRes(ret);

	return p_res;
}


EventData * TemplateLibProcessor::on_recover_template(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪTemplateRecoverReq
	TemplateRecoverReq * p_req = dynamic_cast<TemplateRecoverReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TemplateRecoverReq failed." << endl;
		return NULL;
	}

	//����res
	TemplateRecoverRes * p_res = new TemplateRecoverRes(p_req->GetCallID());

	//ɾ��ģ��
	int ret = UsrTemplateLibManager::GetInstance()->RecoverTemplate(p_req->GetTemplateLibID(), p_req->GetTemplateID());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

