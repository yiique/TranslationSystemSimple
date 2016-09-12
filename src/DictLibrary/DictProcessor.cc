#include "DictProcessor.h"
#include "DictManager.h"
#include "Common/ICUConverter.h"
using namespace dictionary;

DictProcessor DictProcessor::ms_instance;

DictProcessor * DictProcessor::GetInstance()
{
	return &ms_instance;
}

bool DictProcessor::Start(const string & default_file_path)
{
	m_default_file_path = default_file_path;

	return EventEngine::Start();
}

void DictProcessor::on_event(Event & e)
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
	case EDTYPE_DICT_CREATE_REQ:
		p_edata_re = on_create_dict(e._p_edata);
		break;
	case EDTYPE_DICT_MODIFY_REQ:
		p_edata_re = on_modify_dict(e._p_edata);
		break;
	case EDTYPE_DICT_DELETE_REQ:
		p_edata_re = on_delete_dict(e._p_edata);
		break;
	case EDTYPE_DICT_WORDS_INSERT_REQ:
		p_edata_re = on_insert_word(e._p_edata);
		break;
	case EDTYPE_DICT_WORDS_DELETE_REQ:
		p_edata_re = on_delete_word(e._p_edata);
		break;
	case EDTYPE_DICT_WORDS_MODIFY_REQ:
		p_edata_re = on_modify_word(e._p_edata);
		break;
	case EDTYPE_DICT_WORDS_LOAD_REQ:
		p_edata_re = on_load_word_file(e._p_edata);
		break;
	case EDTYPE_DICT_TOSYSTEM_REQ:
		p_edata_re = on_to_sys_dict(e._p_edata);
		break;
	case EDTYPE_DICT_WORDS_RECOVER_REQ:
		p_edata_re = on_recover_word(e._p_edata);
		break;
	case EDTYPE_DICT_RECOVER_REQ:
		p_edata_re = on_recover_dict(e._p_edata);
		break;
	default:
		lerr << "DictProcessor can't handle this event , type = " << e_type << endl;
	}
	

	if(p_edata_re)
	{
		//���ؽ��
		Event re(p_edata_re, this);
		e._p_caller->PostEvent(re);
	}else
	{
		lerr << "DictProcessor handle faield. set p_edata_re is NULL." << endl;
	}
	
	//����Req
	delete e._p_edata;
}



EventData * DictProcessor::on_create_dict(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪDictCreateReq
	DictCreateReq * p_req = dynamic_cast<DictCreateReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to DictCreateReq failed." << endl;
		return NULL;
	}

	//����res
	DictCreateRes * p_res = new DictCreateRes(p_req->GetCallID());

	//�����ʵ�
	DictInfo dict_info = p_req->GetDictInfo();
	int ret = DictManager::GetInstance()->CreateDict(dict_info);

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * DictProcessor::on_modify_dict(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪDictModifyReq
	DictModifyReq * p_req = dynamic_cast<DictModifyReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to DictModifyfReq failed." << endl;
		return NULL;
	}

	//����res
	DictModifyRes * p_res = new DictModifyRes(p_req->GetCallID());

	//�޸Ĵʵ�
	int ret = DictManager::GetInstance()->ModifyDict(p_req->GetDictInfo());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * DictProcessor::on_delete_dict(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪDictDeleteReq
	DictDeleteReq * p_req = dynamic_cast<DictDeleteReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to DictDeleteReq failed." << endl;
		return NULL;
	}

	//����res
	DictDeleteRes * p_res = new DictDeleteRes(p_req->GetCallID());

	//ɾ���ʵ�
	set<DictID>::const_iterator iter = p_req->GetDictIDList().begin();
	for(; iter != p_req->GetDictIDList().end(); ++iter)
	{
		//lout << "Del dict id = " << *iter << endl;
		int ret = DictManager::GetInstance()->DeleteDict(*iter);

		//lout << "ret = " << ret << endl;

		if(SUCCESS != ret)
			p_res->AddFailure(*iter, ret);
	}

	//�������

	return p_res;

}

EventData * DictProcessor::on_to_sys_dict(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪToSysDictReq
	ToSysDictReq * p_req = dynamic_cast<ToSysDictReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to ToSysDictReq failed." << endl;
		return NULL;
	}

	//����res
	ToSysDictRes * p_res = new ToSysDictRes(p_req->GetCallID());

	//����ϵͳ���ʵ�

	int ret = DictManager::GetInstance()->ToSysDict(p_req->GetSrcWordID(), p_req->GetTgtDictID());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * DictProcessor::on_insert_word(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪWordInsertReq
	WordInsertReq * p_req = dynamic_cast<WordInsertReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to WordInsertReq failed." << endl;
		return NULL;
	}

	//����res
	WordInsertRes * p_res = new WordInsertRes(p_req->GetCallID());

	//�������
	int ret = DictManager::GetInstance()->InsertWord(p_req->GetDictID(), p_req->GetWordInfo());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * DictProcessor::on_modify_word(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪWordModifyReq
	WordModifyReq * p_req = dynamic_cast<WordModifyReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to WordModifyReq failed." << endl;
		return NULL;
	}

	//����res
	WordModifyRes * p_res = new WordModifyRes(p_req->GetCallID());

	//�޸Ĵ���
	int ret = DictManager::GetInstance()->ModifyWord(p_req->GetDictID(), p_req->GetWordInfo());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * DictProcessor::on_delete_word(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪWordDeleteReq
	WordDeleteReq * p_req = dynamic_cast<WordDeleteReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to WordDeleteReq failed." << endl;
		return NULL;
	}

	//����res
	WordDeleteRes * p_res = new WordDeleteRes(p_req->GetCallID());

	//ɾ������
	int ret = DictManager::GetInstance()->DeleteWord(p_req->GetDictID(), p_req->GetWordID());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * DictProcessor::on_load_word_file(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪWordLoadReq
	WordLoadReq * p_req = dynamic_cast<WordLoadReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to WordLoadReq failed." << endl;
		return NULL;
	}

	//����res
	WordLoadRes * p_res = new WordLoadRes(p_req->GetCallID());

	int ret = SUCCESS;

	//�������
	vector<pair<string, string> > word_vec;
	if(load_word_file(m_default_file_path + p_req->GetWordFileName(), p_req->IsUTF8(), word_vec))
	{
		vector<WordInfo> word_info_vec;
		for(size_t i=0; i<word_vec.size(); ++i)
		{
			WordInfo word_info;
			word_info.src = word_vec[i].first;
			word_info.tgt = word_vec[i].second;
			word_info.is_active = p_req->GetIsActive();
			word_info.is_checked = 1;

			word_info_vec.push_back(word_info);
		}

		ret = DictManager::GetInstance()->InsertWord(p_req->GetDictID(), word_info_vec);

	}else
	{

		ret = ERROR_UNDEFINE;
	}

	//�������
	p_res->SetRes(ret);

	return p_res;
}

bool DictProcessor::load_word_file(const string & file_path, const bool is_utf8, vector<pair<string, string> > & word_vec)
{
	word_vec.clear();

	ifstream file(file_path.c_str());

	if(false == file.good())
		return false;

	string line;
	size_t idx = 0;
	while(getline(file, line))
	{	
		filter_head_tail(line);
		idx++;
		vector<string> tmp_vec;
		split_string_by_tag(line.c_str(), tmp_vec, '\t');

		if(tmp_vec.size() < 2)
		{
            lwrn << "line = [" << line << "]" << endl;
			lwrn << "Illegal line : " << idx << endl;
			continue;
		}

		pair<string, string> word;
		word.first = tmp_vec.at(0);
		word.second = tmp_vec.at(1);

		if(false == is_utf8)
		{
			ICUConverter::Convert("GB18030", "UTF-8", word.first);
			ICUConverter::Convert("GB18030", "UTF-8", word.second);
		}

		

		word_vec.push_back(word);
	}

	return true;
}


EventData * DictProcessor::on_recover_word(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪWordRecoverReq
	WordRecoverReq * p_req = dynamic_cast<WordRecoverReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to WordRecoverReq failed." << endl;
		return NULL;
	}

	//����res
	WordRecoverRes * p_res = new WordRecoverRes(p_req->GetCallID());

	//�ָ�����
	int ret = DictManager::GetInstance()->RecoverWord(p_req->GetDictID(), p_req->GetWordID());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

EventData * DictProcessor::on_recover_dict(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//ת��ΪDictRecoverReq
	DictRecoverReq * p_req = dynamic_cast<DictRecoverReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to DictRecoverReq failed." << endl;
		return NULL;
	}

	//����res
	DictRecoverRes * p_res = new DictRecoverRes(p_req->GetCallID());

	//�ָ��ʵ�
	int ret = DictManager::GetInstance()->RecoverDict(p_req->GetDictID());

	//�������
	p_res->SetRes(ret);

	return p_res;
}

