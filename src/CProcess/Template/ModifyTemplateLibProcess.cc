#include "ModifyTemplateLibProcess.h"

ModifyTemplateLibProcess::ModifyTemplateLibProcess(EventEngine * p_owner, 
	const CallID & cid, 
	NetPacket & inpacket, 
	NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

ModifyTemplateLibProcess::~ModifyTemplateLibProcess(void)
{
}


ProcessRes ModifyTemplateLibProcess::Begin()
{

	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);
	//lout<<"ModifyTemplateLibProcess Begin():  MsgType"<<s<<endl;
	TemplateLibModifyReq * p_req=new TemplateLibModifyReq(m_cid);
	if( !parse_packet(p_req))
	{
		delete p_req;
		ldbg2 << "Parse input packet failed, build error packet." << endl;
		return on_error(-1);
	}

	//lout << "Post insert templatelib event to PreProcessor..." << endl;
	Event e(p_req,mp_owner);
	TemplateLibProcessor::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes ModifyTemplateLibProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}

	TemplateLibModifyRes * p_res = dynamic_cast<TemplateLibModifyRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to TemplateLibModifyRes failed." << endl;
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

bool ModifyTemplateLibProcess::parse_packet(TemplateLibModifyReq * p_modify_req)
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
	<TemplateLibID> templatelib_id </TemplateLibID>
	<Domain>news</Domain>
	<Language src="english" tgt="chinese" />
	<TemplateLibDescription>xxx</TemplateLibDescription>
	<TemplateLibName>xxx</TemplateLibName>
	<IsActive>1<IsActive>

	</Msg>
	*/
	try
	{
		TiXmlElement* elem;



		//节点TemplateLibID

		elem =docHandle.FirstChild().FirstChild("TemplateLibID").ToElement();
		if( !elem )
		{
			ldbg2 << "Parse Msg failed: TemplateLibID. " << endl;
			throw -1;
		}
		const char * _tmp_templatelibid = elem->GetText();
		if( !_tmp_templatelibid)
		{
			ldbg2 << "Parse Msg failed: usrid is null." << endl;
			throw -1;
		}

		string _tmp_templatelibid_s = _tmp_templatelibid;
		filter_head_tail(_tmp_templatelibid_s);
		p_modify_req->GetTemplateLibInfo().templatelib_id = str_2_num(_tmp_templatelibid_s);


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

		p_modify_req->GetTemplateLibInfo().domain_info.first = _tmp_domain;
		filter_head_tail(p_modify_req->GetTemplateLibInfo().domain_info.first);


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

		p_modify_req->GetTemplateLibInfo().domain_info.second.first = _tmp_language_src;
		filter_head_tail(p_modify_req->GetTemplateLibInfo().domain_info.second.first);
		p_modify_req->GetTemplateLibInfo().domain_info.second.second = _tmp_language_tgt;
		filter_head_tail(p_modify_req->GetTemplateLibInfo().domain_info.second.second);

		//节点TemplateLibName
		elem = docHandle.FirstChild().FirstChild("TemplateLibName").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse Msg failed: TemplateLibName. " << endl;
			throw -1;
		}

		const char * _tmp_templatelibname = elem->GetText();
		if( !_tmp_templatelibname)
		{
			ldbg2 << "Msg failed: TemplateLibName is null." << endl;
			throw -1;
		}


		p_modify_req->GetTemplateLibInfo().templatelib_name=_tmp_templatelibname;
		filter_head_tail(p_modify_req->GetTemplateLibInfo().templatelib_name);

		//TemplateLibDescription
		try
		{
			elem = docHandle.FirstChild().FirstChild("TemplateLibDescription").ToElement();
			if( !elem ) 
			{
				ldbg2 << "Parse Msg failed: TemplateLibDescription. " << endl;
				throw -1;
			}

			const char * _tmp_description = elem->GetText();
			if( !_tmp_description)
			{
				ldbg2 << "Parse Msg failed: _tmp_description is null." << endl;
				throw -1;
			}

			//设置模版库描述
			p_modify_req->GetTemplateLibInfo().description = _tmp_description;
			filter_head_tail(p_modify_req->GetTemplateLibInfo().description);
		}catch(...)
		{
			p_modify_req->GetTemplateLibInfo().description = "";
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

		//设置模版启用
		string _tmp_isactive_s = _tmp_isactive;
		filter_head_tail(_tmp_isactive_s);

		if("1" == _tmp_isactive_s)
			p_modify_req->GetTemplateLibInfo().is_active = 1;
		else
			p_modify_req->GetTemplateLibInfo().is_active = 0;

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	//lout<< "--> ModifyTemplateLibProcess::parse_packet() end." << endl;

	return res;
}

bool ModifyTemplateLibProcess::package_packet(TemplateLibModifyRes * p_modify_res)
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
		TiXmlElement * submitListElem = new TiXmlElement("ModifyTemplateLibRes");   
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
		lerr << "ModifyTemplateLib Error: " << xmldata << endl;
	}

	return true;
}

