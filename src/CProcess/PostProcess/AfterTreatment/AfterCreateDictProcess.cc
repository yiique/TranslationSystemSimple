#include "AfterCreateDictProcess.h"

AfterCreateDictProcess::AfterCreateDictProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

AfterCreateDictProcess::~AfterCreateDictProcess(void)
{
}


ProcessRes AfterCreateDictProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	//发送预处理
	string s;
	m_input_packet.GetMsgType(s);
	//lout<<"CreateDictProcess Begin():  MsgType"<<s<<endl;
	AfterDictCreateReq * p_dict_create = new AfterDictCreateReq(m_cid);

	if( !parse_packet(p_dict_create))
	{
		delete p_dict_create;
		ldbg2 << "Parse input packet failed, bulid error packet." << endl;
		return on_error(-1);
	}

	Event e(p_dict_create, mp_owner);
	GeneralOperation::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes AfterCreateDictProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	//lout<<"CreateDictProcess:: Work() "<< _event_data->GetType() << endl;
	//转换为PulseResponse
	AfterDictCreateRes * p_data = dynamic_cast<AfterDictCreateRes*>(p_edata);

	if(!p_data)
	{
		ldbg2 << "Convert to AfterDictCreateRes failed." << endl;
		return PROCESS_KEEP;
	}

	//将结果打包到m_output_packet
	if( !package_packet(p_data) )
	{
		ldbg2 << "Package_packet failed, create default error packet." << endl;
		//TODO create_error_repacket();
	}
	
	//根据当前状态验证是否为
	return PROCESS_FINISH;//DEBUG
}

bool AfterCreateDictProcess::parse_packet(AfterDictCreateReq * p_dict_create)
{

	//lout<< "--> CreateDictProcess::parse_packet() start." << endl;
	if(!p_dict_create)
	{
		ldbg2 << "Input p_dictinfo is null." << endl;
		return false;
	}

	//获得报文数据
	string content;
	AfterDictInfo _info;
	if(!m_input_packet.GetBody(content))
	{
		ldbg2 << "Get packet's body failed." << endl;
		return false;
	}

	//解析XML
	TiXmlDocument xmldoc;
	xmldoc.Parse(content.c_str());
	TiXmlHandle docHandle( &xmldoc );

	bool res = true;

	try
	{
		TiXmlElement* elem;
		//节点UsrID
		elem = docHandle.FirstChild().FirstChild("UsrID").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse AfterCreateDictMsg failed: UsrID. " << endl;
			throw -1;
		}

		const char * _tmp_usrid = elem->GetText();
		if( !_tmp_usrid)
		{
            ldbg2 << "Parse AfterCreateDictMsg failed: usrid is null." << endl;
			throw -1;
		}
		_info.usr_id = _tmp_usrid;
		filter_head_tail(_info.usr_id);

		//节点Domain
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

		//节点LanguageSrc
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
            ldbg2 << "CreateDictMsgParse AfterCreateDictMsg failed: LanguageSrc is null." << endl;
			throw -1;
		}
		if( "" == _tmp_language_tgt)
		{
            ldbg2 << "CreateDictMsgParse AfterCreateDictMsg failed: LanguageTgt is null." << endl;
			throw -1;
		}

		_info.domain_info.second.first = _tmp_language_src;
		filter_head_tail(_info.domain_info.second.first);
		_info.domain_info.second.second = _tmp_language_tgt;
		filter_head_tail(_info.domain_info.second.second);

	
		//节点DictName
		elem = docHandle.FirstChild().FirstChild("AfterDictName").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse CreateDictMsg failed: AfterDictName. " << endl;
			throw -1;
		}

		const char * _tmp_dictname = elem->GetText();
		if( !_tmp_dictname)
		{
            ldbg2 << "CreateDictMsg failed: AfterDictName is null." << endl;
			throw -1;
		}

		//设置词典名称
		_info.dict_name=_tmp_dictname;
		filter_head_tail(_info.dict_name);


		//设置词典是否启用 可选  此为空的话 设置为启用
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

			//设置词典启用
			string _tmp_isactive_s = _tmp_isactive;
			filter_head_tail(_tmp_isactive_s);

			if("1" == _tmp_isactive_s)
				_info.is_active = 1;
			else
				_info.is_active = 0;
		}catch(...)
		{
			//设置词典启用
			_info.is_active = 1;

		}

		//词典描述 可选
		
		try{
			elem = docHandle.FirstChild().FirstChild("AfterDictDescription").ToElement();
			if( !elem ) 
			{
				ldbg2 << "Parse CreateDictMsg failed: AfterDictDescription. " << endl;
				throw -1;
			}

			const char * _tmp_description = elem->GetText();
			if( !_tmp_description)
			{
			    ldbg2 << "Parse CreateDictMsg failed: _tmp_description is null." << endl;
				throw -1;
			}

			//设置词典描述
			_info.description = _tmp_description;
			filter_head_tail(_info.description);
		}catch(...)
		{

			//设置词典描述
			_info.description = "";

		}


	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	p_dict_create->SetDictInfo(_info);

	return res;
}

bool AfterCreateDictProcess::package_packet(AfterDictCreateRes * p_dict_res)
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
		//msg 节点
		TiXmlElement * msgElem = new TiXmlElement("Msg");   
		xmlDocs->LinkEndChild(msgElem);   

		//生成ResCode节点
		TiXmlElement * codeElem = new TiXmlElement("ResCode");
		TiXmlText * codeText = new TiXmlText("0"); 
		codeElem->LinkEndChild(codeText);
		msgElem->LinkEndChild(codeElem);

		TiXmlElement * contElem_content = new TiXmlElement("Content");   
		msgElem->LinkEndChild(contElem_content);


		TiXmlElement * submitListElem = new TiXmlElement("AfterCreateDictRes");   		
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_dict_res->GetRes());


		//输出到xmldata
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
		lerr << "AfterCreateDict Error: " << xmldata << endl;
	}

	return true;
}

