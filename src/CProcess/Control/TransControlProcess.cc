#include "TransControlProcess.h"

TransControlProcess::TransControlProcess(EventEngine * p_owner, 
								 const CallID & cid, 
								 NetPacket & inpacket, 
								 NetPacket & outpacket): CProcess(p_owner, cid, inpacket, outpacket)
{
}

TransControlProcess::~TransControlProcess(void)
{
}


ProcessRes TransControlProcess::Begin()
{


	//解析inpacket
	if(!parse_packet())
	{
		lerr << " parse input packet failed." << endl;
		return on_error(ERR_USR_PARSE_REQ);
	}


	//发送给TransController
	if(CONTROL_TYPE_STR_SUSPEND == m_control_type)
	{
		TransController::GetOltInstance()->PostTransSuspend(m_domain, m_usr_id, m_text_id, m_cid, mp_owner);

	}else if(CONTROL_TYPE_STR_RERUN == m_control_type)
	{
		TransController::GetOltInstance()->PostTransRerun(m_domain, m_usr_id, m_text_id, m_cid, mp_owner);

	}else if(CONTROL_TYPE_STR_CANCEL == m_control_type)
	{
		TransController::GetOltInstance()->PostTransCancel(m_domain, m_usr_id, m_text_id, m_cid, mp_owner);

	}else
	{
		lerr << "Illegal control type = " << m_control_type << endl;
		return on_error(ERR_USR_ILLEGAL_CONTROL_TYPE);

	}

	return PROCESS_KEEP;
}

ProcessRes TransControlProcess::Work(EventData * p_edata)
{
	//lout<< " start." << endl;

	if(!p_edata)
	{
		lerr << " input event data is null." << endl;
		return PROCESS_KEEP;
	}

	EDType type = p_edata->GetType();

	int result = SUCCESS;

	if(CONTROL_TYPE_STR_SUSPEND == m_control_type && EDTYPE_TRANS_CONTROL_SUSPEND_RES == type)
	{
		TransSuspendRes * p_res =  dynamic_cast<TransSuspendRes*>(p_edata);
		assert(p_res);

		result = p_res->GetResult();

	}else if(CONTROL_TYPE_STR_RERUN == m_control_type && EDTYPE_TRANS_CONTROL_RERUN_RES == type)
	{
		TransRerunRes * p_res =  dynamic_cast<TransRerunRes*>(p_edata);
		assert(p_res);

		result = p_res->GetResult();

	}else if(CONTROL_TYPE_STR_CANCEL == m_control_type && EDTYPE_TRANS_CONTROL_CANCEL_RES == type)
	{
		TransCancelRes * p_res =  dynamic_cast<TransCancelRes*>(p_edata);
		assert(p_res);

		result = p_res->GetResult();
	}else
	{
		lerr << "Recv event is not match, control type = " << m_control_type << " , event type = " << type << endl;
		return PROCESS_KEEP;
	}
	
	//打包
	package_packet(result);

	return PROCESS_FINISH;
}



bool TransControlProcess::parse_packet()
{
	//解析报文
	/*
	<Msg>
		<ControlType>xxx</ControlType>
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

		//节点ControlType
		elem = docHandle.FirstChild("Msg").FirstChild("ControlType").ToElement();
		if( !elem ) 
		{
			lerr << "Parse Msg failed: ControlType. " << endl;
			throw -1;
		}
		const char * _tmp_control_type = elem->GetText();
		if( !_tmp_control_type)
		{
            lerr << "Parse Msg failed: ControlType is null." << endl;
			throw -1;
		}
		//
		m_control_type = _tmp_control_type;
		del_head_tail_blank(m_control_type);

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

		
	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();

	return res;

}

bool TransControlProcess::package_packet(const int result)
{
	//XML生成
	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 

	/*
	<Msg>
		<ResCode>0</ResCode>
		<Content>
	 		<TransControlResult>0</TransControlResult>
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
		TiXmlElement * resultElem = new TiXmlElement("TransControlResult");   
		contElem->LinkEndChild(resultElem);
		TiXmlText * resultText = new TiXmlText(num_2_str(result));
		resultElem->LinkEndChild(resultText);

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

