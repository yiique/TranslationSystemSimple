#include "TransSentProcess.h"

TransSentProcess::TransSentProcess(EventEngine * p_owner, 
								 const CallID & cid, 
								 NetPacket & inpacket, 
								 NetPacket & outpacket): CProcess(p_owner, cid, inpacket, outpacket)
{
}

TransSentProcess::~TransSentProcess(void)
{
}


ProcessRes TransSentProcess::Begin()
{


	//解析inpacket
	if(!parse_packet())
	{
		lerr << " parse input packet failed." << endl;
		return on_error(ERR_USR_PARSE_REQ);
	}


	//发送给TransController
	TransController::GetOltInstance()->GetTransSent(m_domain, m_usr_id, m_text_id, m_offset, m_cid, mp_owner);

	return PROCESS_KEEP;
}

ProcessRes TransSentProcess::Work(EventData * p_edata)
{
	//lout<< " start." << endl;

	if(!p_edata)
	{
		lerr << " input event data is null." << endl;
		return PROCESS_KEEP;
	}

	EDType type = p_edata->GetType();

	int result = SUCCESS;

	GetTransSentRes * p_res =  dynamic_cast<GetTransSentRes*>(p_edata);
	assert(p_res);
	
	//打包
	package_packet(p_res);

	return PROCESS_FINISH;
}



bool TransSentProcess::parse_packet()
{
	//解析报文
	/*
	<Msg>
		<UsrID>Usr_ID</ UsrID>
    	<Domain>news</Domain>
		<Language src="chinese" tgt="english" />
		<TextID>xxx</TextID>
	</Msg>
	*/

	//获得报文数据
	string content;

	if(!m_input_packet.GetBody(content))
	{
		lerr << "ERROR: TransControlProcess::parse_packet() get packet's body failed." << endl;
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

		//节点UsrID
		elem = docHandle.FirstChild("Msg").FirstChild("UsrID").ToElement();
		if( !elem ) 
		{
			lerr << "Parse Msg failed: UsrID. " << endl;
			throw -1;
		}
		const char * _tmp_usr_id = elem->GetText();
		if( !_tmp_usr_id)
		{
            lerr << "Parse Msg failed: UsrID is null." << endl;
			throw -1;
		}
		//
		m_usr_id = _tmp_usr_id;
		del_head_tail_blank(m_usr_id);


		//节点Domain
		elem = docHandle.FirstChild("Msg").FirstChild("Domain").ToElement();
		if( elem ) 
		{
			const char * _tmp_domain = elem->GetText();
			if( _tmp_domain)
			{
				m_domain.first = _tmp_domain;
				del_head_tail_blank(m_domain.first);
			}else
            {
                lerr << "Parse Msg failed: Domain is null." << endl;
				throw -1;
            }

		}else
		{
			lerr << "Parse Msg failed: Domain. " << endl;
			throw -1;
		}
	

		//节点Language 
		elem = docHandle.FirstChild("Msg").FirstChild("Language").ToElement();

		if(elem)
		{
			const char * _tmp_src_language = elem->Attribute("src");
			const char * _tmp_tgt_language = elem->Attribute("tgt");

			if( _tmp_src_language && _tmp_tgt_language)
			{
				m_domain.second.first = _tmp_src_language;
				del_head_tail_blank(m_domain.second.first);

				m_domain.second.second = _tmp_tgt_language;
				del_head_tail_blank(m_domain.second.second);
			}else
            {
                
			    lerr << "Parse Msg failed: Language is null." << endl;
				throw -1;
            }
		}else
		{
			lerr << "Parse Msg failed: Language. " << endl;
			throw -1;
		}

		//节点TextID
		elem = docHandle.FirstChild("Msg").FirstChild("TextID").ToElement();
		if( !elem ) 
		{
			lerr << "Parse Msg failed: TextID. " << endl;
			throw -1;
		}

		const char * _tmp_str_tid = elem->GetText();
		if( !_tmp_str_tid)
		{
        	lerr << "Parse Msg failed: _tmp_str_id is null. " << endl;
			throw -1;
		}

		m_text_id = _tmp_str_tid;
		del_head_tail_blank(m_text_id);

		//Offset
		elem = docHandle.FirstChild("Msg").FirstChild("Offset").ToElement();
		if( !elem ) 
		{
			lerr << "Parse Msg failed: Offset. " << endl;
			throw -1;
		}

		const char * _tmp_offset = elem->GetText();
		if( !_tmp_offset)
		{
        	lerr << "Parse Msg failed: Offset is null. " << endl;
			throw -1;
		}

		m_offset = str_2_num(_tmp_offset);

		
	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();

	return res;

}

bool TransSentProcess::package_packet(GetTransSentRes * p_res)
{
	assert(p_res);


	//XML生成
	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 

	/*
	<Msg>
		<ResCode>0</ResCode>
		<Content>
			<TransSentTotal>......</TransSentTotal>
			<TransSentExist>......</TransSentExist>
	 		<TransSentResult>......</TransSentResult>
		</Content>
	</Msg>
	*/

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

		//生成Content节点
		TiXmlElement * contElem = new TiXmlElement("Content");   
		msgElem->LinkEndChild(contElem);

		//结果
		stringstream result;
		for(size_t i=0; i<p_res->sent_vec.size(); ++i)
		{
			const pair<long, pair<string, string> > & sent = p_res->sent_vec[i];

			result << sent.first << endl;
			result << sent.second.first << endl;
			result << sent.second.second << endl;
		}

		TiXmlElement * resultElem = new TiXmlElement("TransSentResult");   
		contElem->LinkEndChild(resultElem);
		TiXmlText * resultText = new TiXmlText(result.str());
		resultElem->LinkEndChild(resultText);

		TiXmlElement * totalElem = new TiXmlElement("TransSentTotal");   
		contElem->LinkEndChild(totalElem);
		TiXmlText * totalText = new TiXmlText(num_2_str(p_res->total_sent));
		totalElem->LinkEndChild(totalText);

		TiXmlElement * existElem = new TiXmlElement("TransSentExist");   
		contElem->LinkEndChild(existElem);

		string s_exist;
		if(p_res->is_exist)
			s_exist = "TRUE";
		else
			s_exist = "FALSE";
		
		TiXmlText * existText = new TiXmlText(s_exist);
		existElem->LinkEndChild(existText);

		//输出到xmldata
		TiXmlPrinter print;
		xmlDocs->Accept(&print);
		xmldata = print.CStr();

    }
    catch (...)
    {
        xmldata = "<Msg><ResCode>";
		xmldata += num_2_str(ERR_USR_XML_PARSER);
		xmldata += "</ResCode><Content /></Msg>";
    }

	//删除xml解析器
	delete xmlDocs; //其基类析构函数会保证中间节点被delete

	//加入到sendpacket
	this->m_output_packet.Write(HEAD_RESPONSE, MSG_TYPE_OLT_TRANS, xmldata);

	return PROCESS_FINISH;
}

