#include "AfterLoadWordProcess.h"

AfterLoadWordProcess::AfterLoadWordProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

AfterLoadWordProcess::~AfterLoadWordProcess(void)
{
}


ProcessRes AfterLoadWordProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);

	AfterWordLoadReq * p_req = new AfterWordLoadReq(m_cid);
	if( !parse_packet(p_req))
	{
		delete p_req;
		ldbg2 << "Parse input packet failed, build error packet." << endl;
		return on_error(-1);
	}

	
	Event e(p_req, mp_owner);
	GeneralOperation::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes AfterLoadWordProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	AfterWordLoadRes * p_res = dynamic_cast<AfterWordLoadRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to AfterWordLoadRes failed." << endl;
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

bool AfterLoadWordProcess::parse_packet(AfterWordLoadReq * p_load_req)
{

	if(!p_load_req)
	{
		ldbg2 << "Input p_load_req is null." << endl;
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
		<AfterWordFileName>xxx</AfterWordFileName>
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
		p_load_req->SetDictID(str_2_num(_tmp_dictid_s));


		//WordFileName
		elem = docHandle.FirstChild().FirstChild("AfterWordFileName").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: AfterWordFileName:Src. " << endl;
			throw -1;
		}

		const char * _tmp_word_file_name = elem->GetText();
		if( !_tmp_word_file_name)
		{
            ldbg2 << "Parse Msg failed: AfterWordFileName:Src is null." << endl;
			throw -1;
		}

		string _tmp_word_file_name_s = _tmp_word_file_name;
		filter_head_tail(_tmp_word_file_name_s);
		p_load_req->SetWordFileName(_tmp_word_file_name_s);

		//设置词条是否启用 可选  此为空的话 设置为启用
		try{
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
				p_load_req->SetActive(1);
			else
				p_load_req->SetActive(0);
		}catch(...)
		{
			//设置词条启用
			p_load_req->SetActive(1);

		}

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	//lout<< "--> LoadWordProcess::parse_packet() end." << endl;

	return res;
}

bool AfterLoadWordProcess::package_packet(AfterWordLoadRes * p_load_res)
{
	if(!p_load_res)
	{
		ldbg2 << "Input p_load_res is null." << endl;
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
		TiXmlElement * submitListElem = new TiXmlElement("AfterLoadWordRes");   
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_load_res->GetRes());
		
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

	if(SUCCESS != p_load_res->GetRes())
	{
		lerr << "AfterLoadWord Error: " << xmldata << endl;
	}

	return true;
}

