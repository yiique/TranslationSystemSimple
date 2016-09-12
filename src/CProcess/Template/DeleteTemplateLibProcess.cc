#include "DeleteTemplateLibProcess.h"

DeleteTemplateLibProcess::DeleteTemplateLibProcess(EventEngine * p_owner, 
	const CallID & cid, 
	NetPacket & inpacket, 
	NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

DeleteTemplateLibProcess::~DeleteTemplateLibProcess(void)
{
}


ProcessRes DeleteTemplateLibProcess::Begin()
{

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);

	TemplateLibDeleteReq * p_templatelib_delete = new TemplateLibDeleteReq(m_cid);
	if( !parse_packet(p_templatelib_delete))
	{
		delete p_templatelib_delete;
		ldbg2 << "Parse input packet failed, bulid error packet." << endl;
		return on_error(-1);
	}

	Event e(p_templatelib_delete, mp_owner);
	TemplateLibProcessor::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}



ProcessRes DeleteTemplateLibProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	//lout<<"DeleteTemplateLibProcess:: Work() "<< _event_data->GetType() << endl;
	//转换为PulseResponse
	TemplateLibDeleteRes * p_res = dynamic_cast<TemplateLibDeleteRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to TemplateLibDeleteRes failed." << endl;
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

bool DeleteTemplateLibProcess::parse_packet(TemplateLibDeleteReq * p_delete_req)
{

	if(!p_delete_req)
	{
		ldbg2 << "Input p_templatelib_delete is null." << endl;
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
	<TemplateLibID>xxx</TemplateLibID>
	<TemplateLibID>xxx</TemplateLibID>
	<TemplateLibID>xxx</TemplateLibID>
	</Msg>
	*/
	try
	{
		TiXmlElement* elem;

		elem = docHandle.FirstChild().ToElement();
		TiXmlNode * pchild = elem->FirstChild("TemplateLibID");
		while(pchild)
		{
			TiXmlElement * child_elem = pchild->ToElement();
			const char * _tmp_templatelibid = child_elem->GetText();
			if( !_tmp_templatelibid)
			{
				ldbg2 << "Parse TemplateLibDelete failed: usrid is null." << endl;
				pchild = pchild->NextSibling();
				continue;
			}

			string _tmp_templatelibid_s = _tmp_templatelibid;
			filter_head_tail(_tmp_templatelibid_s);
			p_delete_req->AddTemplateLibID(str_2_num(_tmp_templatelibid));
			pchild = pchild->NextSibling();
		}
	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	//lout<< "--> DeleteTemplateLibProcess::parse_packet() end." << endl;

	return res;
}

bool DeleteTemplateLibProcess::package_packet(TemplateLibDeleteRes * p_delete_res)
{
	if(!p_delete_res)
	{
		ldbg2 << "Input p_delete_res is null." << endl;
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
		TiXmlElement * msgElem = new TiXmlElement("Msg");   
		xmlDocs->LinkEndChild(msgElem);   

		//生成ResCode节点
		TiXmlElement * codeElem = new TiXmlElement("ResCode");
		TiXmlText * codeText = new TiXmlText("0"); 
		codeElem->LinkEndChild(codeText);
		msgElem->LinkEndChild(codeElem);
		TiXmlElement * contElem_content = new TiXmlElement("Content");   
		msgElem->LinkEndChild(contElem_content);

		TiXmlElement * submitListElem = new TiXmlElement("DeleteTemplateLibRes");   		
		contElem_content->LinkEndChild(submitListElem);

		stringstream ss;
		map<TemplateLibID, int>::const_iterator iter = p_delete_res->GetFailureList().begin();
		for(; iter != p_delete_res->GetFailureList().end(); ++iter)
			ss << "(" << iter->first << "," << iter->second << ");";

		submitListElem->SetAttribute("failureID", ss.str());

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

	return true;
}

