#include "InsertTemplateProcess.h"

InsertTemplateProcess::InsertTemplateProcess(EventEngine * p_owner, 
	const CallID & cid, 
	NetPacket & inpacket, 
	NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

InsertTemplateProcess::~InsertTemplateProcess(void)
{
}


ProcessRes InsertTemplateProcess::Begin()
{

	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);
	//lout<<"InsertTemplateProcess Begin():  MsgType"<<s<<endl;
	TemplateInsertReq * p_req=new TemplateInsertReq(m_cid);
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


ProcessRes InsertTemplateProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}

	TemplateInsertRes * p_res = dynamic_cast<TemplateInsertRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to TemplateInsertRes failed." << endl;
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

bool InsertTemplateProcess::parse_packet(TemplateInsertReq * p_insert_req)
{

	if(!p_insert_req)
	{
		ldbg2 << "Input p_insert_req is null." << endl;
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
	<InsertTemplate>
	<Src>xxx</Src>
	<Tgt>xxx</Tgt>
	<IsActive>1<IsActive>
	</InsertTemplate>
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
		p_insert_req->SetTemplateLibID(str_2_num(_tmp_templatelibid_s));


		//src
		elem = docHandle.FirstChild().FirstChild("InsertTemplate").FirstChild("Src").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: InsertTemplate:Src. " << endl;
			throw -1;
		}

		const char * _tmp_src = elem->GetText();
		if( !_tmp_src)
		{
			ldbg2 << "Parse Msg failed: InsertTemplate:Src is null." << endl;
			throw -1;
		}

		p_insert_req->GetTemplateInfo().src = _tmp_src;
		filter_head_tail(p_insert_req->GetTemplateInfo().src);

		//tgt
		elem = docHandle.FirstChild().FirstChild("InsertTemplate").FirstChild("Tgt").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: InsertTemplate:Tgt. " << endl;
			throw -1;
		}

		const char * _tmp_tgt = elem->GetText();
		if( !_tmp_tgt)
		{
			ldbg2 << "Parse Msg failed: InsertTemplate:Tgt is null." << endl;
			throw -1;
		}

		p_insert_req->GetTemplateInfo().tgt = _tmp_tgt;
		filter_head_tail(p_insert_req->GetTemplateInfo().tgt);

		//isactive
		//设置模版是否启用 可选  此为空的话 设置为启用
		try{
			elem = docHandle.FirstChild().FirstChild("InsertTemplate").FirstChild("IsActive").ToElement();
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
				p_insert_req->GetTemplateInfo().is_active = 1;
			else
				p_insert_req->GetTemplateInfo().is_active = 0;
		}catch(...)
		{
			//设置模版启用
			p_insert_req->GetTemplateInfo().is_active = 1;

		}

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	//lout<< "--> InsertTemplateProcess::parse_packet() end." << endl;

	return res;
}

bool InsertTemplateProcess::package_packet(TemplateInsertRes * p_insert_res)
{
	if(!p_insert_res)
	{
		ldbg2 << "Input p_insert_res is null." << endl;
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
		TiXmlElement * submitListElem = new TiXmlElement("InsertTemplateRes");   
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_insert_res->GetRes());

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

	if(SUCCESS != p_insert_res->GetRes())
	{
		lerr << "InsertTemplate Error: " << xmldata << endl;
	}

	return true;
}

