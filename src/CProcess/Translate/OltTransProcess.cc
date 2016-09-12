#include "OltTransProcess.h"

OltTransProcess::OltTransProcess(EventEngine * p_owner, 
								 const CallID & cid, 
								 NetPacket & inpacket, 
								 NetPacket & outpacket): CProcess(p_owner, cid, inpacket, outpacket)
{
}

OltTransProcess::~OltTransProcess(void)
{
}


ProcessRes OltTransProcess::Begin()
{


	//����inpacket
	if(!parse_packet())
	{
		lerr << " parse input packet failed." << endl;
		return on_error(ERR_USR_PARSE_REQ);
	}


	//���͸�OltTaskManager
	OltSubmitReq * p_req = new OltSubmitReq(m_cid,
											m_src_textid_vec, 
											m_domain_name,
											m_src_language, 
											m_tgt_language, 
											m_usr_id);

	Event e(p_req, mp_owner);
	OltTaskManager::GetInstance()->PostEvent(e);


	return PROCESS_KEEP;
}

ProcessRes OltTransProcess::Work(EventData * p_edata)
{
	//lout<< " start." << endl;

	if(!p_edata)
	{
		lerr << " input event data is null." << endl;
		return PROCESS_KEEP;
	}

	EDType type = p_edata->GetType();

	if(EDTYPE_OLT_SUBMIT_REQ != type)
	{
		//ֻ����OLT_SUBMIT_RES���ְ�
		lerr << "Recv event is not match type = " << type << endl;
		return PROCESS_KEEP;
	}
	
	//ת����OltSubmitRes
	OltSubmitRes * p_res = dynamic_cast<OltSubmitRes*>(p_edata);

	if(!p_res)
	{
		lerr << "dynamic_cast OltSubmitRes failed." << endl;
		return PROCESS_KEEP;
	}

	//���
	package_packet(p_res);

	return PROCESS_FINISH;
}



bool OltTransProcess::parse_packet()
{
	//��������
	/*
	<Msg>
		<UsrID>Usr_ID</ UsrID>
    	<Domain>news</Domain>
		<Language src=��chinese�� tgt=��english��>
		<TextList number="2">
			<Text seq="0">
				<TextID>32</TextID>
			</Text>
			<Text seq="1">
				<TextID>32</TextID>
			</Text>
		</TextList>
	</Msg>
	*/

	m_src_textid_vec.clear();

	//��ñ�������
	string content;

	if(!m_input_packet.GetBody(content))
	{
		lerr << "ERROR: OltTransProcess::parse_packet() get packet's body failed." << endl;
		return false;
	}

	//����XML
	TiXmlDocument xmldoc;
	xmldoc.Parse(content.c_str());
	TiXmlHandle docHandle( &xmldoc );

	bool res = true;

	try
	{
		TiXmlElement* elem;

		//�ڵ�UsrID
		elem = docHandle.FirstChild("Msg").FirstChild("UsrID").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: OltTransProcess::parse_packet() Parse Msg failed: UsrID. " << endl;
			throw -1;
		}
		const char * _tmp_usr_id = elem->GetText();
		if( !_tmp_usr_id)
		{
            lerr << "ERROR: OltTransProcess::parse_packet() Parse Msg failed: UsrID is null." << endl;
			throw -1;
		}
		//
		m_usr_id = _tmp_usr_id;
		del_head_tail_blank(m_usr_id);


		//�ڵ�Domain
		elem = docHandle.FirstChild("Msg").FirstChild("Domain").ToElement();
		if( elem ) 
		{
			const char * _tmp_domain = elem->GetText();
			if( _tmp_domain)
			{
				m_domain_name = _tmp_domain;
				del_head_tail_blank(m_domain_name);
			}else
            {
                m_domain_name = DOMAIN_UNKNOW;
            }

		}else
		{
			m_domain_name = DOMAIN_UNKNOW;
		}
	

		//�ڵ�Language 
		elem = docHandle.FirstChild("Msg").FirstChild("Language").ToElement();

		if(elem)
		{
			const char * _tmp_src_language = elem->Attribute("src");
			const char * _tmp_tgt_language = elem->Attribute("tgt");

			if( _tmp_src_language && _tmp_tgt_language)
			{
				m_src_language = _tmp_src_language;
				del_head_tail_blank(m_src_language);

				m_tgt_language = _tmp_tgt_language;
				del_head_tail_blank(m_tgt_language);
			}else
            {
                
			    m_src_language = LANGUAGE_UNKNOW;
			    m_tgt_language = LANGUAGE_UNKNOW;
            }
		}else
		{
			m_src_language = LANGUAGE_UNKNOW;
			m_tgt_language = LANGUAGE_UNKNOW;
		}

		//�ڵ�TextList
		elem = docHandle.FirstChild("Msg").FirstChild("TextList").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: OltTransProcess::parse_packet() Parse Msg failed: TransTextList. " << endl;
			throw -1;
		}

		//���TextList ������number
		int number;
		if(!elem->Attribute("number", &number) )
		{
			lerr << "ERROR: OltTransProcess::parse_packet() Parse Msg failed: TransTextList's number. " << endl;
			throw -1;
		}

		if(number < 0 )
		{
			lerr << "ERROR: OltTransProcess::parse_packet() Parse Msg failed: TransTextList's number val is less 0. " << endl;
			throw -1;
		}

		//ѭ����ȡtext
		for(int i=0; i<number; ++i)
		{
			elem = docHandle.FirstChild("Msg").FirstChild("TextList").Child("Text", i).FirstChild("TextID").ToElement();

			if( !elem ) 
			{
				lerr << "ERROR: OltTransProcess::parse_packet() Parse Msg failed: Text i=" << i << endl;
				throw -1;
			}

			//���idֵ
			const char * _tmp_str_id = elem->GetText();
			if( !_tmp_str_id)
			{
            	lerr << "ERROR: OltTransProcess::parse_packet() Parse Msg failed: _tmp_str_id is null. i=" << i << endl;
				throw -1;
			}
			
			TextID _tmp_id;
			if(!TransText::String2TextID(_tmp_str_id, _tmp_id))
			{
				lerr << "ERROR: OltTransProcess::parse_packet() Parse Msg failed: _tmp_str_id to TextID failed. i=" << i << endl;
				throw -1;
			}

			//���뵽vector
			m_src_textid_vec.push_back(_tmp_id);
		}	
		
	}catch (...)
	{
		res = false;
	}

	//����XML��Դ
	xmldoc.Clear();

	return res;

}

bool OltTransProcess::package_packet(OltSubmitRes * p_olt_submit_res)
{
	if(!p_olt_submit_res)
	{
		lerr << "ERROR: OltTransProcess::package_packet() input OltSubmitRes is null." << endl;
		return false;
	}

	//XML����
	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 

	/*
	<Msg>
		<ResCode>0</ResCode>
		<Content>
	 		<SubmitResList number="2">
        		<SubmitRes seq="0" res="0" />
        		<SubmitRes seq="1" res="0" />
    		</SubmitResList>
		</Content>
	</Msg>
	*/

	try
    {
		//msg �ڵ�
		TiXmlElement * msgElem = new TiXmlElement("Msg");   
		xmlDocs->LinkEndChild(msgElem);   

		//����ResCode�ڵ�
		TiXmlElement * codeElem = new TiXmlElement("ResCode");   
		TiXmlText * codeText = new TiXmlText("0"); 
		codeElem->LinkEndChild(codeText);
		msgElem->LinkEndChild(codeElem);

		//����Content�ڵ�
		TiXmlElement * contElem = new TiXmlElement("Content");   
		msgElem->LinkEndChild(contElem);

		//ѭ�����ÿ��Text�Ľ��
		TiXmlElement * submitListElem = new TiXmlElement("SubmitResList");   
		contElem->LinkEndChild(submitListElem);
		const vector<int> & result_vec = p_olt_submit_res->GetResultVec();
		submitListElem->SetAttribute("number", result_vec.size());

		for(size_t i=0; i<result_vec.size(); ++i)
		{
			TiXmlElement * submitElem = new TiXmlElement("SubmitRes");   
			submitListElem->LinkEndChild(submitElem);

			submitElem->SetAttribute("seq", i);
			submitElem->SetAttribute("res", result_vec[i]);
		}

		//�����xmldata
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

	//ɾ��xml������
	delete xmlDocs; //��������������ᱣ֤�м�ڵ㱻delete

	//���뵽sendpacket
	this->m_output_packet.Write(HEAD_RESPONSE, MSG_TYPE_OLT_TRANS, xmldata);

	return PROCESS_FINISH;
}

