#include "ModifyChSegWordProcess.h"

ModifyChSegWordProcess::ModifyChSegWordProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

ModifyChSegWordProcess::~ModifyChSegWordProcess(void)
{
}


ProcessRes ModifyChSegWordProcess::Begin()
{
	
	//解析inpacket

	//获得数据
	string s;
	m_input_packet.GetMsgType(s);
	ModifyChSegWordReq * p_req = new ModifyChSegWordReq(m_cid);
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


ProcessRes ModifyChSegWordProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	ModifyChSegWordRes * p_res = dynamic_cast<ModifyChSegWordRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to ModifyChSegWordRes failed." << endl;
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

bool ModifyChSegWordProcess::parse_packet(ModifyChSegWordReq * p_req)
{

	if(!p_req)
	{
		ldbg2 << "Input p_req is null." << endl;
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
		<ModifyChSegWord>
			<Id>XXX</Id>
			<Src>xxx</Src>
			<IsActive>1<IsActive>
		</ModifyChSegWord>
	</Msg>
	*/
	try
	{
		TiXmlElement* elem;

		int id;
		string word;
		int is_active;

		//id
		elem = docHandle.FirstChild().FirstChild("ModifyChSegWord").FirstChild("Id").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: ModifyChSegWord: Id. " << endl;
			throw -1;
		}

		const char * _tmp_id = elem->GetText();
		if( !_tmp_id)
		{
		    ldbg2 << "Parse Msg failed: ModifyChSegWord: Id is null." << endl;
			throw -1;
		}

		id = str_2_num(_tmp_id);

		//src
		elem = docHandle.FirstChild().FirstChild("ModifyChSegWord").FirstChild("Src").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: ModifyChSegWord:Src. " << endl;
			throw -1;
		}

		const char * _tmp_src = elem->GetText();
		if( !_tmp_src)
		{
		    ldbg2 << "Parse Msg failed: ModifyChSegWord:Src is null." << endl;
			throw -1;
		}

		word = _tmp_src;
		filter_head_tail(word);

		//isactive
		elem = docHandle.FirstChild().FirstChild("ModifyChSegWord").FirstChild("IsActive").ToElement();
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
			is_active = 1;
		else
			is_active = 0;

		seg_word_share_ptr sp_word = seg_word_share_ptr(new ChSegWord(word, is_active));
		sp_word->word_id = id;

		p_req->SetWord(sp_word);

		

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();

	
	return res;
}

bool ModifyChSegWordProcess::package_packet(ModifyChSegWordRes * p_res)
{
	if(!p_res)
	{
		ldbg2 << "Input p_res is null." << endl;
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
		TiXmlText * codeText = new TiXmlText(num_2_str(p_res->GetResult())); 
		codeElem->LinkEndChild(codeText);
		msgElem->LinkEndChild(codeElem);
		
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

	if(SUCCESS != p_res->GetResult())
	{
		lerr << "ModifyChSegWord Error: " << xmldata << endl;
	}

	return true;
}

