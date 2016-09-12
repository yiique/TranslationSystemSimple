#include "LoadChSegWordProcess.h"

LoadChSegWordProcess::LoadChSegWordProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

LoadChSegWordProcess::~LoadChSegWordProcess(void)
{
}


ProcessRes LoadChSegWordProcess::Begin()
{
	
	//解析inpacket

	//获得数据

	string s;
	m_input_packet.GetMsgType(s);

	LoadChSegWordReq * p_req = new LoadChSegWordReq(m_cid);
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


ProcessRes LoadChSegWordProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	LoadChSegWordRes * p_res = dynamic_cast<LoadChSegWordRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to LoadChSegWordReq failed." << endl;
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

bool LoadChSegWordProcess::parse_packet(LoadChSegWordReq * p_req)
{

	assert(p_req);

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
		<WordFileName>xxx</WordFileName>
		<IsActive>xxx</IsActive> //可选
	</Msg>
	*/
	try
	{
		TiXmlElement* elem;
	
		//WordFileName
		elem = docHandle.FirstChild().FirstChild("WordFileName").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: WordFileName:Src. " << endl;
			throw -1;
		}

		const char * _tmp_word_file_name = elem->GetText();
		if( !_tmp_word_file_name)
		{
            ldbg2 << "Parse Msg failed: WordFileName:Src is null." << endl;
			throw -1;
		}

		string _tmp_word_file_name_s = _tmp_word_file_name;
		filter_head_tail(_tmp_word_file_name_s);
		p_req->SetFileName(_tmp_word_file_name_s);

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
				p_req->SetActive(1);
			else
				p_req->SetActive(0);
		}catch(...)
		{
			//设置词条启用
			p_req->SetActive(1);

		}
		
		try{
			elem = docHandle.FirstChild().FirstChild("IsUTF8").ToElement();
			if( !elem ) 
			{
				ldbg2 << "Parse Msg failed: IsUTF8. " << endl;
				throw -1;
			}

			const char * _tmp_isutf8 = elem->GetText();
			if( !_tmp_isutf8)
			{
			    ldbg2 << "Parse Msg failed: _tmp_isutf8 is null." << endl;
				throw -1;
			}

            string _tmp_isutf8_s = _tmp_isutf8;
			filter_head_tail(_tmp_isutf8_s);

			if("yes" == _tmp_isutf8_s)
				p_req->SetIsUTF8(true);
			else
				p_req->SetIsUTF8(false);
		}catch(...)
		{
			p_req->SetIsUTF8(true);

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

bool LoadChSegWordProcess::package_packet(LoadChSegWordRes * p_res)
{
	assert(p_res);

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
		lerr << "LoadWord Error: " << xmldata << endl;
	}

	return true;
}

