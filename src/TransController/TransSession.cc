#include "TransSession.h"

#ifdef ENABLE_SOFT_PROTECT
#include "SoftProtect/PermitKey.h"
#endif

TransSession::TransSession(const CallID & cid,
						   const int sockfd, 
						   trans_task_share_ptr sp_task): 
											CSession(cid, sockfd),
											m_sp_task(sp_task)
{

	#ifdef OUTPUT_TRANS_TIME
	gettimeofday(&m_trans_beg_time,NULL);
	#endif
}

TransSession::~TransSession(void)
{
}

bool TransSession::ParseRecvPacekt()
{

	//��ñ�������
	string content;

	if(!_recv_packet.GetBody(content))
	{
		lerr << "ERROR: TransSession::ParseRecvPacekt() get packet's body failed." << endl;
		return false;
	}

	//lout << "Get trans result = [" <<content << "]" << endl;

	//����XML
	TiXmlDocument xmldoc;
	xmldoc.Parse(content.c_str());
	TiXmlHandle docHandle( &xmldoc );

	/*
		<Msg>
			<TranslateResult rescode="0" > //������֤ �ܿ��ܴ�ʱ�������Ѿ���Ϊ������������� 0Ϊ�ɹ���1Ϊ�ܾ�
				<Target>This is translate result . </Target>
				//����������Ϣ
				<Assist1></Assist1>
				<Assist2></Assist2>
			</TranslateResult>
		</Msg>
	*/
	
	bool res = true;

	try
	{
		TiXmlElement* elem;

		//�ڵ�TranslateResult
		elem = docHandle.FirstChild("Msg").FirstChild("TranslateResult").ToElement();
		if( !elem ) 
		{
			//lerr << "ERROR: TransSession::ParseRecvPacekt() Parse xml failed: no this elem :TranslateResult. " << endl;
			throw -1;
		}

		//��֤rescode
		const char * _tmp_rescode = elem->Attribute("rescode");
		if(!_tmp_rescode)
		{
			//�������ڴ����ԣ�����Ϊ����������
			m_ser_rescode = RES_CODE_SUC;
		}else
		{
			//���ڴ����ԣ�����ֵ֤
			m_ser_rescode = _tmp_rescode;

			if(m_ser_rescode != RES_CODE_SUC)
			{
				lerr << "WARNING: TRANS SERVER DENIED. _rescode = " << m_ser_rescode << endl;
				throw -1;
			}
		}

		//�ڵ�Target
		elem = docHandle.FirstChild("Msg").FirstChild("TranslateResult").FirstChild("Target").ToElement();
		if( !elem ) 
		{
			//lerr << "ERROR: TransSession::ParseRecvPacekt() Parse xml failed: no this elem :Target. " << endl;
			throw -1;
		}
		const char * _tmp_target = elem->GetText();
		if( !_tmp_target)
		{
            //ldbg2 << "WARNING: TransSession::ParseRecvPacekt() Parse xml failed: _tmp_target is null." << endl;
			_tmp_target = "";
		}
	
		//����trans_tgt
		m_sp_task->trans_tgt._decode_result = _tmp_target;
		m_sp_task->trans_tgt._tgt_str = _tmp_target;

		//ȥ����λ�ո�
		del_head_tail_blank(m_sp_task->trans_tgt._tgt_str);

        
		//�ڵ�Assist1
		elem = docHandle.FirstChild("Msg").FirstChild("TranslateResult").FirstChild("Assist1").ToElement();
		if( elem ) 
		{
			const char * _tmp_assist1 = elem->GetText();
			if( !_tmp_assist1)
			{
			    //ldbg2 << "WARNING: TransSession::ParseRecvPacekt() Parse xml failed: Assist1 is null." << endl;
				_tmp_assist1 = "";
			}
	
			//����Assist1
			m_sp_task->trans_tgt._assist_str = _tmp_assist1;
		}else
		{
			m_sp_task->trans_tgt._assist_str = "";
		}
		
		
		//�ڵ�Assist2
		elem = docHandle.FirstChild("Msg").FirstChild("TranslateResult").FirstChild("Assist2").ToElement();
		if( elem ) 
		{
			const char * _tmp_assist2 = elem->GetText();
			if( !_tmp_assist2)
			{
			    //ldbg2 << "WARNING: TransSession::ParseRecvPacekt() Parse xml failed: Assist2 is null." << endl;
				_tmp_assist2 = "";
			}
	
			//����trans_tgt
			m_sp_task->trans_tgt._assist2_str = _tmp_assist2;
		}else
		{
			m_sp_task->trans_tgt._assist2_str = "";
		}

		//�ڵ�oovwords
		elem = docHandle.FirstChild("Msg").FirstChild("TranslateResult").FirstChild("OOV").FirstChild("OOVWords").ToElement();

		if( elem ) 
		{
			const char * _tmp_oov_words = elem->GetText();
			if( _tmp_oov_words)
			{
			    split_string_by_tag(_tmp_oov_words, m_sp_task->trans_tgt._oov_vec, '\t');	
			}

		}

		//�ڵ�Alignment
		elem = docHandle.FirstChild("Msg").FirstChild("TranslateResult").FirstChild("Alignment").ToElement();

		if( elem ) 
		{
			const char * _tmp_align_src = elem->GetText();
			if( _tmp_align_src)
			{
				//lout << "_tmp_align_src : " << _tmp_align_src << endl;
				BaseAlign::UnSerialize(_tmp_align_src, m_sp_task->trans_tgt._decode_align_vec);
				//lout << "Alignment vec size : " << m_sp_task->trans_tgt._decode_align_vec.size() << endl;
			}else
			{
				//ldbg2 << "WARNING: TransSession::ParseRecvPacekt() Parse xml failed: Alignment is null." << endl;
			}
		}else
		{	
			//ldbg2 << "WARNING: TransSession::ParseRecvPacekt() Parse xml failed: no Alignment." << endl;
		}

		//�ڵ�OOVSent
		elem = docHandle.FirstChild("Msg").FirstChild("TranslateResult").FirstChild("OOV").FirstChild("OOVSent").ToElement();

		if( elem ) 
		{
			const char * _tmp_oov_sent = elem->GetText();
			if( _tmp_oov_sent)
			{
			    m_sp_task->trans_tgt._oov_sent = _tmp_oov_sent;
			}

		}


	}catch (...)
	{
		res = false;
	}

	//����XML��Դ
	xmldoc.Clear();
	
	return res;
}

bool TransSession::BuildSendPacket()
{


	//���ĸ�ʽ��
	/*
		<Msg>
			<Translate>
				<SerUUID>xxxx<SerUUID>//������֤
				<Domain>news</Domain>  //������֤ �ܿ��ܴ�ʱ�������Ѿ���Ϊ�������������
				<Language src=��chinese�� tgt=��english�� /> //������֤
				<Source>���� �� ���� �� ���� ��</Source>
				//�ʵ��ģ��
				<TemplateAndDict>xxxxx</TemplateAndDict>
			</Translate>
		</Msg>
	*/

	//XML����
	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 

	try
    {
		//msg �ڵ�
		TiXmlElement * msgElem = new TiXmlElement("Msg");   
		xmlDocs->LinkEndChild(msgElem);   

		//����Translate
		TiXmlElement * transElem = new TiXmlElement("Translate");   
		msgElem->LinkEndChild(transElem);

		//����SerUUID
		TiXmlElement * uuidElem = new TiXmlElement("SerUUID");   
		TiXmlText * uuidText = new TiXmlText(m_sp_task->sp_slave->GetInfo().slave_uuid); 
		uuidElem->LinkEndChild(uuidText);
		transElem->LinkEndChild(uuidElem);

		//����Domain
		TiXmlElement * domainElem = new TiXmlElement("Domain");   
		TiXmlText * domainText = new TiXmlText(m_sp_task->sp_slave->GetInfo().domain.first); 
		domainElem->LinkEndChild(domainText);
		transElem->LinkEndChild(domainElem);

		//����Language
		TiXmlElement * langElem = new TiXmlElement("Language");
		transElem->LinkEndChild(langElem);
		langElem->SetAttribute("src", m_sp_task->sp_slave->GetInfo().domain.second.first);
		langElem->SetAttribute("tgt", m_sp_task->sp_slave->GetInfo().domain.second.second);

		//����SubLanguage
		TiXmlElement * sublanguageElem = new TiXmlElement("SubLanguage");   
		TiXmlText * sublanguageText = new TiXmlText(num_2_str(m_sp_task->sub_src_language)); 
		transElem->LinkEndChild(sublanguageElem);
		sublanguageElem->SetAttribute("src", num_2_str(m_sp_task->sub_src_language));

		//����Source
		TiXmlElement * sourceElem = new TiXmlElement("Source");   
		TiXmlText * sourceText = new TiXmlText(m_sp_task->trans_src._src_str); 
		sourceElem->LinkEndChild(sourceText);
		transElem->LinkEndChild(sourceElem);

		//����DictRule
		string dict_text;
		for(size_t i=0; i<m_sp_task->trans_src._dict_result_vec.size(); ++i)
		{
			dict_text += m_sp_task->trans_src._dict_result_vec[i] + "\n";
		}

		TiXmlElement * dictElem = new TiXmlElement("DictRule");   
		TiXmlText * dictText = new TiXmlText(dict_text); 
		dictElem->LinkEndChild(dictText);
		transElem->LinkEndChild(dictElem);

		
#ifdef ENABLE_SOFT_PROTECT
		//PermitKey
		string src_key = m_sp_task->trans_src._src_str;
		string permit_key;
		PermitKey::Generate(filter_head_tail(src_key), permit_key);
		TiXmlElement * permitElem = new TiXmlElement("PermitKey");   
		TiXmlText * permitText = new TiXmlText(permit_key); 
		permitElem->LinkEndChild(permitText);
		transElem->LinkEndChild(permitElem);
#endif
	


		//�����xmldata
		TiXmlPrinter print;
		xmlDocs->Accept(&print);
		xmldata = print.CStr();

    }
    catch (...)
    {
		//ɾ��xml������
		delete xmlDocs;

        return false;
    }

	//ɾ��xml������
	delete xmlDocs; //��������������ᱣ֤�м�ڵ㱻delete

	//���뵽sendpacket
	this->_send_packet.Write(HEAD_REQUEST, MSG_TYPE_TRANSLATE, xmldata);

	//ldbg1 << "--> TransSession::BuildSendPacket() send packet get data = " << _send_packet.GetData() << endl;

	return true;
}


