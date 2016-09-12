#include "InsertWordProcess.h"

InsertWordProcess::InsertWordProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

InsertWordProcess::~InsertWordProcess(void)
{
}


ProcessRes InsertWordProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);
	//lout<<"InsertWordProcess Begin():  MsgType"<<s<<endl;
	WordInsertReq * p_req=new WordInsertReq(m_cid);
	if( !parse_packet(p_req))
	{
		delete p_req;
		ldbg2 << "Parse input packet failed, build error packet." << endl;
		return on_error(-1);
	}

	//lout << "Post insert dict event to PreProcessor..." << endl;
	Event e(p_req,mp_owner);
	DictProcessor::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes InsertWordProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	WordInsertRes * p_res = dynamic_cast<WordInsertRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to WordInsertRes failed." << endl;
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

bool InsertWordProcess::parse_packet(WordInsertReq * p_insert_req)
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
		<DictID> dict_id </DictID>
		<InsertWord>
			<Src>xxx</Src>
			<Tgt>xxx</Tgt>
			<IsActive>1<IsActive>
		</InsertWord>
	</Msg>
	*/
	try
	{
		TiXmlElement* elem;
		
		//节点DictID
	
		elem =docHandle.FirstChild().FirstChild("DictID").ToElement();
		if( !elem )
		{
			ldbg2 << "Parse Msg failed: DictID. " << endl;
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
		p_insert_req->SetDictID(str_2_num(_tmp_dictid_s));


		//src
		elem = docHandle.FirstChild().FirstChild("InsertWord").FirstChild("Src").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: InsertWord:Src. " << endl;
			throw -1;
		}

		const char * _tmp_src = elem->GetText();
		if( !_tmp_src)
		{
            ldbg2 << "Parse Msg failed: InsertWord:Src is null." << endl;
			throw -1;
		}

		p_insert_req->GetWordInfo().src = _tmp_src;
		filter_head_tail(p_insert_req->GetWordInfo().src);

		//tgt
		elem = docHandle.FirstChild().FirstChild("InsertWord").FirstChild("Tgt").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: InsertWord:Tgt. " << endl;
			throw -1;
		}

		const char * _tmp_tgt = elem->GetText();
		if( !_tmp_tgt)
		{
            ldbg2 << "Parse Msg failed: InsertWord:Tgt is null." << endl;
			throw -1;
		}

		p_insert_req->GetWordInfo().tgt = _tmp_tgt;
		filter_head_tail(p_insert_req->GetWordInfo().tgt);

		//isactive
		//设置词条是否启用 可选  此为空的话 设置为启用
		try{
			elem = docHandle.FirstChild().FirstChild("InsertWord").FirstChild("IsActive").ToElement();
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

			//设置词条启用
			string _tmp_isactive_s = _tmp_isactive;
			filter_head_tail(_tmp_isactive_s);

			if("1" == _tmp_isactive_s)
				p_insert_req->GetWordInfo().is_active = 1;
			else
				p_insert_req->GetWordInfo().is_active = 0;
		}catch(...)
		{
			//设置词条启用
			p_insert_req->GetWordInfo().is_active = 1;

		}

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	//lout<< "--> InsertWordProcess::parse_packet() end." << endl;

	return res;
}

bool InsertWordProcess::package_packet(WordInsertRes * p_insert_res)
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
		TiXmlElement * submitListElem = new TiXmlElement("InsertWordRes");   
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
		lerr << "InsertWord Error: " << xmldata << endl;
	}

	return true;
}

