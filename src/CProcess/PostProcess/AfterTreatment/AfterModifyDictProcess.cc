#include "AfterModifyDictProcess.h"

AfterModifyDictProcess::AfterModifyDictProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

AfterModifyDictProcess::~AfterModifyDictProcess(void)
{
}


ProcessRes AfterModifyDictProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);
	//lout<<"ModifyDictProcess Begin():  MsgType"<<s<<endl;
	AfterDictModifyReq * p_req=new AfterDictModifyReq(m_cid);
	if( !parse_packet(p_req))
	{
		delete p_req;
		ldbg2 << "Parse input packet failed, build error packet." << endl;
		return on_error(-1);
	}

	//lout << "Post insert dict event to PreProcessor..." << endl;
	Event e(p_req,mp_owner);
	GeneralOperation::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes AfterModifyDictProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	AfterDictModifyRes * p_res = dynamic_cast<AfterDictModifyRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to AfterDictModifyRes failed." << endl;
		return PROCESS_KEEP;
	}

	//将结果打包到m_output_packet
	if( !package_packet(p_res) )
	{
		ldbg2 << "Package_packet failed, delete default error packet." << endl;
		//TODO delete_error_repacket();
	}
	
	//根据当前状态验证是否为
	return PROCESS_FINISH;//DEBUG
}

bool AfterModifyDictProcess::parse_packet(AfterDictModifyReq * p_modify_req)
{

	if(!p_modify_req)
	{
		ldbg2 << "Input p_modify_req is null." << endl;
		return false;
	}

	//获得报文数据
	string content;
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
	/*
	<Msg>
		<AfterDictID> dict_id </AfterDictID>
		<Domain>news</Domain>
		<Language src="english" tgt="chinese" />
		<AfterDictDescription>xxx</AfterDictDescription>
		<AfterDictName>xxx</AfterDictName>
		<IsActive>1<IsActive>
	</Msg>
	*/
	try
	{
		TiXmlElement* elem;

		
		
		//节点DictID
	
		elem =docHandle.FirstChild().FirstChild("AfterDictID").ToElement();
		if( !elem )
		{
			ldbg2 << "Parse Msg failed: AfterDictID. " << endl;
			throw -1;
		}
		const char * _tmp_dictid = elem->GetText();
		if( !_tmp_dictid)
		{
            ldbg2 << "Parse Msg failed: usrid is null." << endl;
			throw -1;
		}

		string _tmp_dictid_s = _tmp_dictid;
		filter_head_tail(_tmp_dictid_s);
		p_modify_req->GetDictInfo().dict_id = str_2_num(_tmp_dictid_s);


		//节点Domain
		elem = docHandle.FirstChild().FirstChild("Domain").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse Msg failed: Domain. " << endl;
			throw -1;
		}

		const char * _tmp_domain = elem->GetText();
		if( !_tmp_domain)
		{
            ldbg2 << "Parse Msg failed: Domain is null." << endl;
			throw -1;
		}

		p_modify_req->GetDictInfo().domain_info.first = _tmp_domain;
		filter_head_tail(p_modify_req->GetDictInfo().domain_info.first);


		//节点LanguageSrc
		elem = docHandle.FirstChild().FirstChild("Language").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse Msg failed: Language. " << endl;
			throw -1;
		}

		string _tmp_language_src = elem->Attribute("src");
		string _tmp_language_tgt = elem->Attribute("tgt");
		filter_head_tail(_tmp_language_src);
		filter_head_tail(_tmp_language_tgt);

		if( "" == _tmp_language_src)
		{
            ldbg2 << "Msg failed: LanguageSrc is null." << endl;
			throw -1;
		}
		if( "" == _tmp_language_tgt)
		{
            ldbg2 << "Msg failed: LanguageTgt is null." << endl;
			throw -1;
		}

		p_modify_req->GetDictInfo().domain_info.second.first = _tmp_language_src;
		filter_head_tail(p_modify_req->GetDictInfo().domain_info.second.first);
		p_modify_req->GetDictInfo().domain_info.second.second = _tmp_language_tgt;
		filter_head_tail(p_modify_req->GetDictInfo().domain_info.second.second);

		//节点DictName
		elem = docHandle.FirstChild().FirstChild("AfterDictName").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse Msg failed: AfterDictName. " << endl;
			throw -1;
		}

		const char * _tmp_dictname = elem->GetText();
		if( !_tmp_dictname)
		{
            ldbg2 << "Msg failed: AfterDictName is null." << endl;
			throw -1;
		}

		
		p_modify_req->GetDictInfo().dict_name=_tmp_dictname;
		filter_head_tail(p_modify_req->GetDictInfo().dict_name);

		//DictDescription
		try
		{
			elem = docHandle.FirstChild().FirstChild("AfterDictDescription").ToElement();
			if( !elem ) 
			{
				ldbg2 << "Parse Msg failed: AfterDictDescription. " << endl;
				throw -1;
			}

			const char * _tmp_description = elem->GetText();
			if( !_tmp_description)
			{
			    ldbg2 << "Parse Msg failed: _tmp_description is null." << endl;
				throw -1;
			}

			//设置词典描述
			p_modify_req->GetDictInfo().description = _tmp_description;
			filter_head_tail(p_modify_req->GetDictInfo().description);
		}catch(...)
		{
			p_modify_req->GetDictInfo().description = "";
		}

		//isactive
		elem = docHandle.FirstChild().FirstChild("IsActive").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse Msg failed: IsActive. " << endl;
			throw -1;
		}

		const char * _tmp_isactive = elem->GetText();
		if( !_tmp_isactive)
		{
		    ldbg2 << "Parse Msg failed: _tmp_isactive is null." << endl;
			throw -1;
		}

		//设置词条启用
		string _tmp_isactive_s = _tmp_isactive;
		filter_head_tail(_tmp_isactive_s);

		if("1" == _tmp_isactive_s)
			p_modify_req->GetDictInfo().is_active = 1;
		else
			p_modify_req->GetDictInfo().is_active = 0;

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	//lout<< "--> ModifyDictProcess::parse_packet() end." << endl;

	return res;
}

bool AfterModifyDictProcess::package_packet(AfterDictModifyRes * p_modify_res)
{
	if(!p_modify_res)
	{
		ldbg2 << "Input p_modify_res is null." << endl;
		return false;
	}

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
		TiXmlElement * submitListElem = new TiXmlElement("AfterModifyDictRes");   
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_modify_res->GetRes());
		
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


	if(SUCCESS != p_modify_res->GetRes())
	{
		lerr << "AfterModifyDict Error: " << xmldata << endl;
	}

	return true;
}

