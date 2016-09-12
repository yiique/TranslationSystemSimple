#include "ModifyWordProcess.h"

ModifyWordProcess::ModifyWordProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

ModifyWordProcess::~ModifyWordProcess(void)
{
}


ProcessRes ModifyWordProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);
	//lout<<"ModifyWordProcess Begin():  MsgType"<<s<<endl;
	WordModifyReq * p_req=new WordModifyReq(m_cid);
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


ProcessRes ModifyWordProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	WordModifyRes * p_res = dynamic_cast<WordModifyRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to WordModifyRes failed." << endl;
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

bool ModifyWordProcess::parse_packet(WordModifyReq * p_modify_req)
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
		<DictID> dict_id </DictID>
		<ModifyWord>
			<WordID>XXX</WordID>
			<Src>xxx</Src>
			<Tgt>xxx</Tgt>
			<IsActive>1<IsActive>
		</ModifyWord>
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
		p_modify_req->SetDictID(str_2_num(_tmp_dictid_s));

		//WordID
		elem = docHandle.FirstChild().FirstChild("ModifyWord").FirstChild("WordID").ToElement();
		if( !elem )
		{
			ldbg2 << "Parse Msg failed: WordID. " << endl;
			throw -1;
		}
		const char * _tmp_wordid = elem->GetText();
		if( !_tmp_wordid)
		{
            ldbg2 << "Parse Msg failed: WordID is null." << endl;
			throw -1;
		}

		string _tmp_wordid_s = _tmp_wordid;
		filter_head_tail(_tmp_wordid_s);
		p_modify_req->GetWordInfo().word_id = str_2_num(_tmp_wordid_s);

		//src
		elem = docHandle.FirstChild().FirstChild("ModifyWord").FirstChild("Src").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: ModifyWord:Src. " << endl;
			throw -1;
		}

		const char * _tmp_src = elem->GetText();
		if( !_tmp_src)
		{
            ldbg2 << "Parse Msg failed: ModifyWord:Src is null." << endl;
			throw -1;
		}

		p_modify_req->GetWordInfo().src = _tmp_src;
		filter_head_tail(p_modify_req->GetWordInfo().src);

		//tgt
		elem = docHandle.FirstChild().FirstChild("ModifyWord").FirstChild("Tgt").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: ModifyWord:Tgt. " << endl;
			throw -1;
		}

		const char * _tmp_tgt = elem->GetText();
		if( !_tmp_tgt)
		{
            ldbg2 << "Parse Msg failed: ModifyWord:Tgt is null." << endl;
			throw -1;
		}

		p_modify_req->GetWordInfo().tgt = _tmp_tgt;
		filter_head_tail(p_modify_req->GetWordInfo().tgt);

		//isactive
		elem = docHandle.FirstChild().FirstChild("ModifyWord").FirstChild("IsActive").ToElement();
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
			p_modify_req->GetWordInfo().is_active = 1;
		else
			p_modify_req->GetWordInfo().is_active = 0;



		//ischecked
		//elem = docHandle.FirstChild().FirstChild("ModifyWord").FirstChild("IsChecked").ToElement();
		//if( !elem ) 
		//{
		//	ldbg2 << "Parse tMsg failed: IsChecked. " << endl;
		//	throw -1;
		//}

		//const char * _tmp_ischecked = elem->GetText();
		//if( !_tmp_ischecked)
		//{
		//    ldbg2 << "Parse Msg failed: _tmp_ischecked is null." << endl;
		//	throw -1;
		//}

		////设置审核
		//string _tmp_ischecked_s = _tmp_ischecked;
		//filter_head_tail(_tmp_ischecked_s);

		//if("1" == _tmp_ischecked_s)
		//	p_modify_req->GetWordInfo().is_checked = 1;
		//else
		//	p_modify_req->GetWordInfo().is_checked = 0;

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();
	//lout<< "--> ModifyWordProcess::parse_packet() end." << endl;

	return res;
}

bool ModifyWordProcess::package_packet(WordModifyRes * p_modify_res)
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
		TiXmlElement * submitListElem = new TiXmlElement("ModifyWordRes");   
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
		lerr << "ModifyWord Error: " << xmldata << endl;
	}

	return true;
}

