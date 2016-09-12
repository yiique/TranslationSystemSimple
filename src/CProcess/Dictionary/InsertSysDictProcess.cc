#include "InsertSysDictProcess.h"


InsertSysDictProcess::InsertSysDictProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

InsertSysDictProcess::~InsertSysDictProcess(void)
{
}


ProcessRes InsertSysDictProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	//发送预处理
	string s;
	m_input_packet.GetMsgType(s);

	ToSysDictReq * p_dict_sys_insert = new ToSysDictReq(m_cid);

	if( !parse_packet(p_dict_sys_insert))
	{
		delete p_dict_sys_insert;
		ldbg2 << "Parse input packet failed, bulid error packet." << endl;
		return on_error(-1);
	}

	Event e(p_dict_sys_insert, mp_owner);
	DictProcessor::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes InsertSysDictProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}

	ToSysDictRes * p_data = dynamic_cast<ToSysDictRes*>(p_edata);

	if(!p_data)
	{
		ldbg2 << "Convert to ToSysDictRes failed." << endl;
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

bool InsertSysDictProcess::parse_packet(ToSysDictReq * p_dict_sys_insert)
{

	//lout<< "--> CreateDictProcess::parse_packet() start." << endl;
	if(!p_dict_sys_insert)
	{
		ldbg2 << "Input p_dictinfo is null." << endl;
		return false;
	}

	//获得报文数据
	string content;
	vector<size_t> src_word_id;
	DictID tgt_dict_id;
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
		//节点SrcUsrID
		elem = docHandle.FirstChild().FirstChild("SrcWordID").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse ToSysDictReq failed: SrcDictID. " << endl;
			throw -1;
		}

		TiXmlAttribute* pAttr = NULL;

		for(pAttr = elem->FirstAttribute(); pAttr; pAttr = pAttr->Next())
		{
			string tmp = pAttr->Value();
			src_word_id.push_back(atoi(tmp.c_str()));
		}

		//for(int i=0; i<src_word_id.size(); i++)
		//{
		//	cout << src_word_id[i] << "-----------------" << endl;
		//}
		/*const char * _src_usrid = elem->GetText();
		if( !_src_usrid)
		{
            ldbg2 << "Parse ToSysDictReq failed: src_usrid is null." << endl;
			throw -1;
		}

		src_dict_id = atoi(_src_usrid);
		*/
		//节点TgtUsrId
		elem = docHandle.FirstChild().FirstChild("TgtDictID").ToElement();
		if( !elem ) 
		{
			ldbg2 << "Parse ToSysDictReq failed: TgtDictID. " << endl;
			throw -1;
		}

		const char * _tgt_usrid = elem->GetText();
		if( !_tgt_usrid)
		{
            ldbg2 << "Parse ToSysDictReq failed: tgt_usrid is null." << endl;
			throw -1;
		}
		//filter_head_tail(_tgt_usrid);
		tgt_dict_id = atoi(_tgt_usrid);

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	p_dict_sys_insert->SetSrcWordID(src_word_id);
	p_dict_sys_insert->SetTgtDictID(tgt_dict_id);

	return res;
}

bool InsertSysDictProcess::package_packet(ToSysDictRes * p_dict_sys_insert_res)
{
	if(!p_dict_sys_insert_res)
	{
		ldbg2 << "ERROR:  InsertSysDictProcess::package_packet() input p_dict_sys_insert_res is null." << endl;
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


		TiXmlElement * submitListElem = new TiXmlElement("ToSysDictRes");   		
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_dict_sys_insert_res->GetRes());


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

	if(SUCCESS != p_dict_sys_insert_res->GetRes())
	{
		lerr << "ToSysDictRes Error: " << xmldata << endl;
	}

	return true;
}

