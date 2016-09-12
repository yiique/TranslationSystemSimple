#include "AfterDeleteWordProcess.h"

AfterDeleteWordProcess::AfterDeleteWordProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

AfterDeleteWordProcess::~AfterDeleteWordProcess(void)
{
}


ProcessRes AfterDeleteWordProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);

	AfterWordDeleteReq * p_req = new AfterWordDeleteReq(m_cid);
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


ProcessRes AfterDeleteWordProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	AfterWordDeleteRes * p_res = dynamic_cast<AfterWordDeleteRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to AfterWordDeleteRes failed." << endl;
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

bool AfterDeleteWordProcess::parse_packet(AfterWordDeleteReq * p_delete_req)
{

	if(!p_delete_req)
	{
		ldbg2 << "Input p_delete_req is null." << endl;
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
		<AfterDeleteWordID>xxx</AfterDeleteWordID>
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
		p_delete_req->SetDictID(str_2_num(_tmp_dictid_s));


		//src
		elem = docHandle.FirstChild().FirstChild("AfterDeleteWordID").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: AfterDeleteWordID:Src. " << endl;
			throw -1;
		}

		const char * _tmp_del_word_id = elem->GetText();
		if( !_tmp_del_word_id)
		{
            ldbg2 << "Parse Msg failed: AfterDeleteWordID:Src is null." << endl;
			throw -1;
		}

		string _tmp_del_word_id_s = _tmp_del_word_id;
		filter_head_tail(_tmp_del_word_id_s);
		p_delete_req->SetWordID(str_2_num(_tmp_del_word_id_s));


	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	//lout<< "--> DeleteWordProcess::parse_packet() end." << endl;

	return res;
}

bool AfterDeleteWordProcess::package_packet(AfterWordDeleteRes * p_delete_res)
{
	if(!p_delete_res)
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
		TiXmlElement * submitListElem = new TiXmlElement("AfterDeleteWordRes");   
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_delete_res->GetRes());
		
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

	if(SUCCESS != p_delete_res->GetRes())
	{
		lerr << "AfterDeleteWord Error: " << xmldata << endl;
	}

	return true;
}

