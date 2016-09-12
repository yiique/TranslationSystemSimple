#include "RecoverTemplateProcess.h"

RecoverTemplateProcess::RecoverTemplateProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

RecoverTemplateProcess::~RecoverTemplateProcess(void)
{
}


ProcessRes RecoverTemplateProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);

	TemplateRecoverReq * p_req = new TemplateRecoverReq(m_cid);
	if( !parse_packet(p_req))
	{
		delete p_req;
		ldbg2 << "Parse input packet failed, build error packet." << endl;
		return on_error(-1);
	}

	
	Event e(p_req, mp_owner);
	TemplateLibProcessor::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes RecoverTemplateProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	TemplateRecoverRes * p_res = dynamic_cast<TemplateRecoverRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to TemplateRecoverRes failed." << endl;
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

bool RecoverTemplateProcess::parse_packet(TemplateRecoverReq * p_recover_req)
{

	if(!p_recover_req)
	{
		ldbg2 << "Input p_recover_req is null." << endl;
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
		<TemplateLibID> XXXX</TemplateLibID>
		<RecoverTemplateID>xxx</RecoverTemplateID>
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
		p_recover_req->SetTemplateLibID(str_2_num(_tmp_templatelibid_s));


		//src
		elem = docHandle.FirstChild().FirstChild("RecoverTemplateID").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: RecoverTemplateID:Src. " << endl;
			throw -1;
		}

		const char * _tmp_del_template_id = elem->GetText();
		if( !_tmp_del_template_id)
		{
            ldbg2 << "Parse Msg failed: RecoverTemplateID:Src is null." << endl;
			throw -1;
		}

		string _tmp_del_template_id_s = _tmp_del_template_id;
		filter_head_tail(_tmp_del_template_id_s);
		p_recover_req->SetTemplateID(str_2_num(_tmp_del_template_id_s));


	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();

	return res;
}

bool RecoverTemplateProcess::package_packet(TemplateRecoverRes * p_recover_res)
{
	if(!p_recover_res)
	{
		ldbg2 << "Input p_delete_res is null." << endl;
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
		TiXmlElement * submitListElem = new TiXmlElement("RecoverTemplateRes");   
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_recover_res->GetRes());
		
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

	if(SUCCESS != p_recover_res->GetRes())
	{
		lerr << "RecoverTemplate Error: " << xmldata << endl;
	}

	return true;
}

