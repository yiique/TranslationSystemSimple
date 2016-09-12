#include "CreateDictProcess.h"

CreateDictProcess::CreateDictProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

CreateDictProcess::~CreateDictProcess(void)
{
}


ProcessRes CreateDictProcess::Begin()
{
	
	//����inpacket

	//�������

	//����Ԥ����
	string s;
	m_input_packet.GetMsgType(s);
	//lout<<"CreateDictProcess Begin():  MsgType"<<s<<endl;
	DictCreateReq * p_dict_create = new DictCreateReq(m_cid);

	if( !parse_packet(p_dict_create))
	{
		delete p_dict_create;
		ldbg2 << "Parse input packet failed, bulid error packet." << endl;
		return on_error(-1);
	}

	Event e(p_dict_create, mp_owner);
	DictProcessor::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes CreateDictProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	//lout<<"CreateDictProcess:: Work() "<< _event_data->GetType() << endl;
	//ת��ΪPulseResponse
	DictCreateRes * p_data = dynamic_cast<DictCreateRes*>(p_edata);

	if(!p_data)
	{
		ldbg2 << "Convert to DictCreateRes failed." << endl;
		return PROCESS_KEEP;
	}

	//����������m_output_packet
	if( !package_packet(p_data) )
	{
		ldbg2 << "Package_packet failed, create default error packet." << endl;
		//TODO create_error_repacket();
	}
	
	//���ݵ�ǰ״̬��֤�Ƿ�Ϊ
	return PROCESS_FINISH;//DEBUG
}

bool CreateDictProcess::parse_packet(DictCreateReq * p_dict_create)
{

	//lout<< "--> CreateDictProcess::parse_packet() start." << endl;
	if(!p_dict_create)
	{
		ldbg2 << "Input p_dictinfo is null." << endl;
		return false;
	}

	//��ñ�������
	string content;
	DictInfo _info;
	if(!m_input_packet.GetBody(content))
	{
		ldbg2 << "Get packet's body failed." << endl;
		return false;
	}

	//����XML
	TiXmlDocument xmldoc;
	xmldoc.Parse(content.c_str());
	TiXmlHandle docHandle( &xmldoc );

	bool res = true;

	try
	{
		TiXmlElement* elem;
		//�ڵ�UsrID
		elem = docHandle.FirstChild().FirstChild("UsrID").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse CreateDictMsg failed: UsrID. " << endl;
			throw -1;
		}

		const char * _tmp_usrid = elem->GetText();
		if( !_tmp_usrid)
		{
            ldbg2 << "Parse CreateDictMsg failed: usrid is null." << endl;
			throw -1;
		}
		_info.usr_id = _tmp_usrid;
		filter_head_tail(_info.usr_id);

		//�ڵ�Domain
		elem = docHandle.FirstChild().FirstChild("Domain").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse CreateDictMsg failed: Domain. " << endl;
			throw -1;
		}

		const char * _tmp_domain = elem->GetText();
		if( !_tmp_domain)
		{
            ldbg2 << "Parse CreateDictMsg failed: Domain is null." << endl;
			throw -1;
		}
		_info.domain_info.first = _tmp_domain;
		filter_head_tail(_info.domain_info.first);

		//�ڵ�LanguageSrc
		elem = docHandle.FirstChild().FirstChild("Language").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse CreateDictMsg failed: Language. " << endl;
			throw -1;
		}

		string _tmp_language_src = elem->Attribute("src");
		string _tmp_language_tgt = elem->Attribute("tgt");
		filter_head_tail(_tmp_language_src);
		filter_head_tail(_tmp_language_tgt);

		if( "" == _tmp_language_src)
		{
            ldbg2 << "CreateDictMsgParse CreateDictMsg failed: LanguageSrc is null." << endl;
			throw -1;
		}
		if( "" == _tmp_language_tgt)
		{
            ldbg2 << "CreateDictMsgParse CreateDictMsg failed: LanguageTgt is null." << endl;
			throw -1;
		}

		_info.domain_info.second.first = _tmp_language_src;
		filter_head_tail(_info.domain_info.second.first);
		_info.domain_info.second.second = _tmp_language_tgt;
		filter_head_tail(_info.domain_info.second.second);

	
		//�ڵ�DictName
		elem = docHandle.FirstChild().FirstChild("DictName").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse CreateDictMsg failed: DictName. " << endl;
			throw -1;
		}

		const char * _tmp_dictname = elem->GetText();
		if( !_tmp_dictname)
		{
            ldbg2 << "CreateDictMsg failed: DictName is null." << endl;
			throw -1;
		}

		//���ôʵ�����
		_info.dict_name=_tmp_dictname;
		filter_head_tail(_info.dict_name);

		//���ôʵ�����
		try{
			elem = docHandle.FirstChild().FirstChild("IsSystem").ToElement();
			if( !elem ) 
			{
				ldbg2 << "Parse CreateDictMsg failed: IsSystem. " << endl;
				throw -1;
			}

			const char * _tmp_issystem = elem->GetText();
			if( !_tmp_issystem)
			{
			    ldbg2 << "Parse CreateDictMsg failed: _tmp_issystem is null." << endl;
				throw -1;
			}

			//���ôʵ�����
			string _tmp_issystem_s = _tmp_issystem;
			filter_head_tail(_tmp_issystem_s);

			if("1" == _tmp_issystem_s)
				_info.type = DICT_TYPE_SYS;
			else if("0" == _tmp_issystem_s)
				_info.type = DICT_TYPE_USR;
			else 
				_info.type = DICT_TYPE_PRO;
		}catch(...)
		{
			//���ôʵ�����
			_info.type = DICT_TYPE_PRO;

		}


		//���ôʵ��Ƿ����� ��ѡ  ��Ϊ�յĻ� ����Ϊ����
		try{
			elem = docHandle.FirstChild().FirstChild("IsActive").ToElement();
			if( !elem ) 
			{
				ldbg2 << "Parse CreateDictMsg failed: IsActive. " << endl;
				throw -1;
			}

			const char * _tmp_isactive = elem->GetText();
			if( !_tmp_isactive)
			{
			    ldbg2 << "Parse CreateDictMsg failed: _tmp_isactive is null." << endl;
				throw -1;
			}

			//���ôʵ�����
			string _tmp_isactive_s = _tmp_isactive;
			filter_head_tail(_tmp_isactive_s);

			if("1" == _tmp_isactive_s)
				_info.is_active = 1;
			else
				_info.is_active = 0;
		}catch(...)
		{
			//���ôʵ�����
			_info.is_active = 1;

		}

		//�ʵ����� ��ѡ
		
		try{
			elem = docHandle.FirstChild().FirstChild("DictDescription").ToElement();
			if( !elem ) 
			{
				ldbg2 << "Parse CreateDictMsg failed: DictDescription. " << endl;
				throw -1;
			}

			const char * _tmp_description = elem->GetText();
			if( !_tmp_description)
			{
			    ldbg2 << "Parse CreateDictMsg failed: _tmp_description is null." << endl;
				throw -1;
			}

			//���ôʵ�����
			_info.description = _tmp_description;
			filter_head_tail(_info.description);
		}catch(...)
		{

			//���ôʵ�����
			_info.description = "";

		}


	}catch (...)
	{
		res = false;
	}

	//����XML��Դ
	xmldoc.Clear();
	p_dict_create->SetDictInfo(_info);

	return res;
}

bool CreateDictProcess::package_packet(DictCreateRes * p_dict_res)
{
	if(!p_dict_res)
	{
		ldbg2 << "ERROR:  CreateDictProcess::package_packet() input p_cmd_res is null." << endl;
		return false;
	}
	/*
	string content;
	content = "<Msg><Result>";
	content += num_2_str(pCmdRes->m_result);
	content += "</Result></Msg>";
	*/
	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 
	
	try
    {
		//msg �ڵ�
		TiXmlElement * msgElem = new TiXmlElement("Msg");   
		xmlDocs->LinkEndChild(msgElem);   

		//����ResCode�ڵ�
		TiXmlElement * codeElem = new TiXmlElement("ResCode");
		TiXmlText * codeText = new TiXmlText("0"); 
		codeElem->LinkEndChild(codeText);
		msgElem->LinkEndChild(codeElem);

		TiXmlElement * contElem_content = new TiXmlElement("Content");   
		msgElem->LinkEndChild(contElem_content);


		TiXmlElement * submitListElem = new TiXmlElement("CreateDictRes");   		
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_dict_res->GetRes());


		//�����xmldata
		TiXmlPrinter print;
		xmlDocs->Accept(&print);
		xmldata = print.CStr();

    }
	
	 catch (...)
    {
        xmldata = "<Msg><ResCode>";
		xmldata += num_2_str(ERR_USR_XML_PARSER);
		xmldata += "</ResCode></Msg>";
    }
    delete xmlDocs;
	string type;
	m_input_packet.GetMsgType(type);

	m_output_packet.Write(HEAD_RESPONSE, type, xmldata);

	if(SUCCESS != p_dict_res->GetRes())
	{
		lerr << "CreateDict Error: " << xmldata << endl;
	}

	return true;
}

