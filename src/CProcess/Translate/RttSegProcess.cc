#include "RttSegProcess.h"


RttSegProcess::RttSegProcess(EventEngine * p_owner, 
								 const CallID & cid, 
								 const TextID & tid,
								 NetPacket & inpacket, 
								 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket),
														 m_rtt_tid(tid),
														 m_need_align(false),
														 m_need_detail(false)//, m_fake(fake)
{
}

RttSegProcess::~RttSegProcess(void)
{
}

ProcessType RttSegProcess::GetType() const
{
	return PROCESS_TYPE_RTT_SEG;
}



ProcessRes RttSegProcess::Begin()
{
	//lout<< "--> RttSegProcess::Begin()." << endl;


	//����inpacket
	if(!parse_packet())
	{
		lerr << "RttSegProcess::Begin() parse input packet failed." << endl;
		return on_error(ERR_USR_PARSE_REQ);
	}

	//��ʱ�����Ƴ���
	//if(m_trans_src.size() > Configurator::_s_max_rtt_src_size)
	//{
	//	//TODO
	//	//ldbg1 << "Input sent is too long , it's size = " << m_trans_src.size() << " src = [" << m_trans_src<< "]" << endl;
	//	package_packet("NULL", "��ʾ��������ľ��ӳ����������ơ���Your input sentence is too long�� please submit it as file.��");
	//	return PROCESS_FINISH;;
	//}
	
	//����TransText
	TransText * p_trans_text = new TransText(TRANS_TYPE_RTT, m_rtt_tid); 

	#ifdef ENABLE_TIME_LOG
	p_trans_text->time_log[T_TOTAL].Start();
	#endif //ENABLE_TIME_LOG
	
		p_trans_text->SetUsrID(m_usr_id);
	p_trans_text->SetTextType(TEXT_TYPE_TXT);
	p_trans_text->SetDomain(m_domain_name, m_src_language, m_tgt_language);
	p_trans_text->ResetSrcData(m_trans_src); //����ԭ��
	
	//�����ļ�����
	bool need_language;
	if(LANGUAGE_UNKNOW != m_src_language)
		need_language = false;
	else
		need_language = true;
	LTransTextProcessor::GetInstance()->LocalProcess(p_trans_text, TRANS_TYPE_RTT, need_language, m_cid, mp_owner);

	//����״̬
	m_rtt_seg_proc_state = RTT_SEG_PROC_REMOTE_PROC;


	#ifdef ENABLE_TIME_LOG
	p_trans_text->time_log[T_TOTAL][T_TEXT_FORMAT].Start();
	#endif //ENABLE_TIME_LOG

	return PROCESS_KEEP;
}


ProcessRes RttSegProcess::on_txt_proc_end(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "ERROR: input event data is null." << endl;
		return PROCESS_KEEP;
	}

	TextProcRes * p_proc_res = dynamic_cast<TextProcRes*>(p_edata);
	if(!p_proc_res)
	{
		lerr << "ERROR: convert to TextProcRes failed." << endl;
		return PROCESS_KEEP;
	}

	//�б�״̬
	if(RTT_SEG_PROC_REMOTE_PROC != m_rtt_seg_proc_state)
	{
		lerr << "State is not match . curr state = " << m_rtt_seg_proc_state << endl;
		return PROCESS_KEEP;
	}

	//�жϽ��
	int result = p_proc_res->GetResult();
	if(result != SUCCESS)
	{
		lerr << "ERROR: result is mean failed. res = " << result << endl;
		return on_error(p_proc_res->GetResult());
	}

	#ifdef ENABLE_TIME_LOG
	p_proc_res->GetTransText()->time_log[T_TOTAL][T_TEXT_FORMAT].Stop();
	p_proc_res->GetTransText()->time_log[T_TOTAL][T_PER_PROCESS].Start();
	#endif //ENABLE_TIME_LOG

	//����Ԥ����.
	PreProcessor::PostRTTProcess(p_proc_res->GetTransText(), m_cid, mp_owner);

	m_rtt_seg_proc_state = RTT_SEG_PROC_PREPROC;

	return PROCESS_KEEP;
}


ProcessRes RttSegProcess::on_preprocess_end(EventData * p_edata)
{

	//lout<< "--> RttSegProcess::on_preprocess_end()." << endl;

	if(!p_edata)
	{
		lerr << "ERROR: RttSegProcess::on_preprocess_end() input event data is null." << endl;
		return PROCESS_KEEP;
	}

	PreProcRes * p_proc_res = dynamic_cast<PreProcRes*>(p_edata);

	if(!p_proc_res)
	{
		lerr << "ERROR: RttSegProcess::on_preprocess_end() convert to PreProcRes failed." << endl;
		return PROCESS_KEEP;
	}

	//�жϽ��
	int result = p_proc_res->GetResult();
	if( SUCCESS != result)
	{
		lerr << "RttSegProcess::on_preprocess_end() result is mean failed. res = " << result << endl;
		delete p_proc_res->GetTransText();
		//SessionManager ���� delete p_edata;
		return on_error(result);
	}

	if(!package_packet(p_proc_res->GetTransText()))
	{
		lerr << "ERROR: RttSegProcess::on_preprocess_end() package_packet failed." << endl;
		return PROCESS_KEEP;
	}

	#ifdef ENABLE_TIME_LOG
	p_proc_res->GetTransText()->time_log[T_TOTAL][T_PER_PROCESS].Stop();
	p_proc_res->GetTransText()->time_log[T_TOTAL].Stop();
	#endif //ENABLE_TIME_LOG

	m_rtt_seg_proc_state = RTT_SEG_PROC_END;
	delete p_proc_res->GetTransText(); //����text ����RttSegProcess::Begin()�����ɵ�
	return PROCESS_FINISH;
}


ProcessRes RttSegProcess::Work(EventData * p_edata)
{
	//lout<< "--> RttSegProcess::Work()." << endl;

	if(!p_edata)
	{
		lerr << "ERROR: RttSegProcess::Work() input event data is null." << endl;
		return PROCESS_KEEP;
	}

	const EDType type = p_edata->GetType();
	
	if(EDTYPE_TEXT_PROCESS_RES == type && RTT_SEG_PROC_REMOTE_PROC == m_rtt_seg_proc_state)
	{
		//Զ��Ԥ�������
		return on_txt_proc_end(p_edata);

	}else if(EDTYPE_PREPROC_RES == type && RTT_SEG_PROC_PREPROC == m_rtt_seg_proc_state)
	{
		//Ԥ�������
		return on_preprocess_end(p_edata);
	}
	
	//�������
	lerr << "ERROR: RttSegProcess::Work() recv event is not match the state. state = " << m_rtt_seg_proc_state << endl;
	lerr << "                                                                   type = " << type << endl;
	return PROCESS_KEEP;
	
}

bool RttSegProcess::parse_packet()
{
	//��ñ�������
	string content;

	if(!m_input_packet.GetBody(content))
	{
		lerr << "ERROR: RttSegProcess::parse_packet() get packet's body failed." << endl;
		return false;
	}

	//����XML
	TiXmlDocument xmldoc;
	xmldoc.Parse(content.c_str());
	TiXmlHandle docHandle( &xmldoc );

	//���ĸ�ʽ��
	/*
		<Msg>
		    <UsrID>xxx</UsrID>
    		<Domain>news</Domain>
			<Language src=��chinese�� tgt=��english�� />  
			<TransSrc>�������ı�����Ҫ����xml�淶��</TransSrc>
		</Msg>
	*/
	bool res = true;

	try
	{
		TiXmlElement* elem;

		//�ڵ�UsrID
		elem = docHandle.FirstChild("Msg").FirstChild("UsrID").ToElement();
		if( elem ) 
		{	
			const char * _tmp_usr_id = elem->GetText();

			if( _tmp_usr_id)
			{
				m_usr_id = _tmp_usr_id;
				del_head_tail_blank(m_usr_id);

			}else
			{
			    lwrn << "Parse Msg failed: _tmp_usr_id is null. Set UsrID to guest." << endl;
				m_usr_id = "guest";
			}

		}else
		{
			lwrn << "Parse Msg failed: UsrID. Set UsrID to guest." << endl;
			m_usr_id = "guest";
		}

	
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
		
		//�ڵ�TransSrc
		elem = docHandle.FirstChild("Msg").FirstChild("TransSrc").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: RttSegProcess::parse_packet() Parse Msg failed: TransSrc. " << endl;
			throw -1;
		}

		const char * _tmp_trans_src = elem->GetText();
		if( !_tmp_trans_src)
		{
            lerr << "WARNING: RttSegProcess::parse_packet() Parse Msg failed: TransSrc is null." << endl;
			_tmp_trans_src = "";
		}
		
		m_trans_src = _tmp_trans_src; //����ȥǰ��ո�del_head_tail_blank

		find_and_replace(m_trans_src, "\\\"", "\"");
		find_and_replace(m_trans_src, "\\'", "'");
		find_and_replace(m_trans_src, "\\\\", "\\");

		const char * _tmp_need_align = elem->Attribute("align");
		if(_tmp_need_align && string("1") == _tmp_need_align)
		{
			m_need_align = true;
		}else
		{
			m_need_align = false;
		}
	
		const char * _tmp_need_detail = elem->Attribute("detail");
		if(_tmp_need_detail)
		{
			if(string("1") == _tmp_need_detail)
			{
				m_need_detail = true;
				m_need_sentpair = false;
			}else if(string("0") == _tmp_need_detail)
			{
				m_need_detail = false;
				m_need_sentpair = false;
			}else
			{
				m_need_detail = false;
				m_need_sentpair = true;
			}
		}else
		{
			m_need_detail = false;
			m_need_sentpair = false;
		}

	}catch (...)
	{
		res = false;
	}

	//����XML��Դ
	xmldoc.Clear();

	return res;

}

bool RttSegProcess::package_packet(TransText * p_text)
{

	//���ɻظ�����
	//XML����
	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 

	/*
	<Msg>
		<ResCode>0</ResCode>
		<Content>
			<TransRes>���Ƿ�������</TransRes>
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

		//����Language�ڵ�
		TiXmlElement * langElem = new TiXmlElement("Language");   
		contElem->LinkEndChild(langElem);
		langElem->SetAttribute("src", p_text->GetSrcLanguage());
		langElem->SetAttribute("tgt", p_text->GetTgtLanguage());

		//����TransRes
		TiXmlElement * transElem = new TiXmlElement("TransRes");   
		contElem->LinkEndChild(transElem);

		//����Alignment
		if(m_need_align)
		{
			string align_str;
			string format_str;
			p_text->CreateAlignment(format_str, align_str);

			TiXmlElement * srcElem = new TiXmlElement("SrcData");   
			TiXmlText * srcText = new TiXmlText(p_text->GetSrcData()); 
			srcElem->LinkEndChild(srcText);
			msgElem->LinkEndChild(srcElem);

			TiXmlElement * alignElem = new TiXmlElement("Alignment");   
			TiXmlText * alignText = new TiXmlText(align_str); 
			alignElem->LinkEndChild(alignText);
			msgElem->LinkEndChild(alignElem);

			TiXmlElement * formatElem = new TiXmlElement("Format");   
			TiXmlText * fromatText = new TiXmlText(format_str); 
			formatElem->LinkEndChild(fromatText);
			msgElem->LinkEndChild(formatElem);
		}

		string trans_tgt;
		p_text->getPreResult(trans_tgt);
		//trans_tgt += m_fake;
			
		lout << "TransResult[" <<  p_text->GetID() << "] : [" << trans_tgt << "]" << endl; 

		TiXmlText * transText = new TiXmlText(trans_tgt);
		transElem->LinkEndChild(transText);

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
	#ifdef ENABLE_TIME_LOG
	string time_log_str = p_text->time_log.Serialize();
	xmldata += "\n" + time_log_str;
	lout << "TimeLog[" << p_text->GetID() << "] : \n" << time_log_str << endl;
	#endif //ENABLE_TIME_LOG

	//ɾ��xml������
	delete xmlDocs; //��������������ᱣ֤�м�ڵ㱻delete

	//���뵽sendpacket
	this->m_output_packet.Write(HEAD_RESPONSE, MSG_TYPE_RTT_TRANS, xmldata);


	return true;
}

