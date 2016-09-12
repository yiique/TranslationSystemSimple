#include "ModifyTemplateProcess.h"

ModifyTemplateProcess::ModifyTemplateProcess(EventEngine * p_owner, 
	const CallID & cid, 
	NetPacket & inpacket, 
	NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

ModifyTemplateProcess::~ModifyTemplateProcess(void)
{
}


ProcessRes ModifyTemplateProcess::Begin()
{

	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);
	//lout<<"ModifyTemplateProcess Begin():  MsgType"<<s<<endl;
	TemplateModifyReq * p_req=new TemplateModifyReq(m_cid);
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


ProcessRes ModifyTemplateProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}

	TemplateModifyRes * p_res = dynamic_cast<TemplateModifyRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to TemplateModifyRes failed." << endl;
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

bool ModifyTemplateProcess::parse_packet(TemplateModifyReq * p_modify_req)
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
	<ModifyTemplate>
	<TemplateID>XXX</TemplateID>
	<Src>xxx</Src>
	<Tgt>xxx</Tgt>
	<IsActive>1<IsActive>
	</ModifyTemplate>
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
		p_modify_req->SetTemplateLibID(str_2_num(_tmp_templatelibid_s));

		//TemplateID
		elem = docHandle.FirstChild().FirstChild("ModifyTemplate").FirstChild("TemplateID").ToElement();
		if( !elem )
		{
			ldbg2 << "Parse Msg failed: TemplateID. " << endl;
			throw -1;
		}
		const char * _tmp_templateid = elem->GetText();
		if( !_tmp_templateid)
		{
			ldbg2 << "Parse Msg failed: TemplateID is null." << endl;
			throw -1;
		}

		string _tmp_templateid_s = _tmp_templateid;
		filter_head_tail(_tmp_templateid_s);
		p_modify_req->GetTemplateInfo().template_id = str_2_num(_tmp_templateid_s);

		//src
		elem = docHandle.FirstChild().FirstChild("ModifyTemplate").FirstChild("Src").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: ModifyTemplate:Src. " << endl;
			throw -1;
		}

		const char * _tmp_src = elem->GetText();
		if( !_tmp_src)
		{
			ldbg2 << "Parse Msg failed: ModifyTemplate:Src is null." << endl;
			throw -1;
		}

		p_modify_req->GetTemplateInfo().src = _tmp_src;
		filter_head_tail(p_modify_req->GetTemplateInfo().src);

		//tgt
		elem = docHandle.FirstChild().FirstChild("ModifyTemplate").FirstChild("Tgt").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: ModifyTemplate:Tgt. " << endl;
			throw -1;
		}

		const char * _tmp_tgt = elem->GetText();
		if( !_tmp_tgt)
		{
			ldbg2 << "Parse Msg failed: ModifyTemplate:Tgt is null." << endl;
			throw -1;
		}

		p_modify_req->GetTemplateInfo().tgt = _tmp_tgt;
		filter_head_tail(p_modify_req->GetTemplateInfo().tgt);

		//isactive
		elem = docHandle.FirstChild().FirstChild("ModifyTemplate").FirstChild("IsActive").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse tMsg failed: IsActive. " << endl;
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
			p_modify_req->GetTemplateInfo().is_active = 1;
		else
			p_modify_req->GetTemplateInfo().is_active = 0;

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	//lout<< "--> ModifyTemplateProcess::parse_packet() end." << endl;

	return res;
}

bool ModifyTemplateProcess::package_packet(TemplateModifyRes * p_modify_res)
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
		TiXmlElement * submitListElem = new TiXmlElement("ModifyTemplateRes");   
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
		lerr << "ModifyTemplate Error: " << xmldata << endl;
	}

	return true;
}

