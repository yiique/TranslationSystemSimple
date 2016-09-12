#include "InsertSysTemplateLibProcess.h"


InsertSysTemplateLibProcess::InsertSysTemplateLibProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

InsertSysTemplateLibProcess::~InsertSysTemplateLibProcess(void)
{
}


ProcessRes InsertSysTemplateLibProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	//发送预处理
	string s;
	m_input_packet.GetMsgType(s);

	ToSysTemplateLibReq * p_templatelib_sys_insert = new ToSysTemplateLibReq(m_cid);

	if( !parse_packet(p_templatelib_sys_insert))
	{
		delete p_templatelib_sys_insert;
		ldbg2 << "Parse input packet failed, bulid error packet." << endl;
		return on_error(-1);
	}

	Event e(p_templatelib_sys_insert, mp_owner);
	TemplateLibProcessor::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes InsertSysTemplateLibProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}

	ToSysTemplateLibRes * p_data = dynamic_cast<ToSysTemplateLibRes*>(p_edata);

	if(!p_data)
	{
		ldbg2 << "Convert to ToSysTemplateLibRes failed." << endl;
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

bool InsertSysTemplateLibProcess::parse_packet(ToSysTemplateLibReq * p_templatelib_sys_insert_req)
{
	if(!p_templatelib_sys_insert_req)
	{
		ldbg2 << "Input p_templatelibinfo is null." << endl;
		return false;
	}

	//获得报文数据
	string content;
	vector<int> src_templatelib_id;
	TemplateLibID tgt_templatelib_id;
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
		//节点SrcTemplateID
		elem = docHandle.FirstChild().FirstChild("SrcTemplateID").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse ToSysTemplateLibReq failed: SrcTemplateID. " << endl;
			throw -1;
		}

		TiXmlAttribute* pAttr = NULL;

		for(pAttr = elem->FirstAttribute(); pAttr; pAttr = pAttr->Next())
		{
			string tmp = pAttr->Value();
			src_templatelib_id.push_back(atoi(tmp.c_str()));
		}
		
		//节点TgtTemplateLibId
		elem = docHandle.FirstChild().FirstChild("TgtTemplateLibID").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse ToSysTemplateLibReq failed: TgtTemplateLibID. " << endl;
			throw -1;
		}

		const char * _tgt_templatelibid = elem->GetText();
		if( !_tgt_templatelibid)
		{
            ldbg2 << "Parse ToSysTemplateLibReq failed: tgt_templatelibid is null." << endl;
			throw -1;
		}
		//filter_head_tail(_tgt_usrid);
		tgt_templatelib_id = atoi(_tgt_templatelibid);

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	p_templatelib_sys_insert_req->SetSrcTemplateID(src_templatelib_id);
	p_templatelib_sys_insert_req->SetTgtTemplateLibID(tgt_templatelib_id);

	return res;
}

bool InsertSysTemplateLibProcess::package_packet(ToSysTemplateLibRes * p_templatelib_sys_insert_res)
{
	if(!p_templatelib_sys_insert_res)
	{
		ldbg2 << "ERROR:  InsertSysTemplateLibProcess::package_packet() input p_templatelib_sys_insert_res is null." << endl;
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


		TiXmlElement * submitListElem = new TiXmlElement("ToSysTemplateLibRes");   		
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_templatelib_sys_insert_res->GetRes());


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

	if(SUCCESS != p_templatelib_sys_insert_res->GetRes())
	{
		lerr << "ToSysTemplateLibRes Error: " << xmldata << endl;
	}

	return true;
}

