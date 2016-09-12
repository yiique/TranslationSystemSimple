#include "OltDownloadProcess.h"

OltDownloadProcess::OltDownloadProcess(EventEngine * p_owner, 
								 const CallID & cid, 
								 NetPacket & inpacket, 
								 NetPacket & outpacket): CProcess(p_owner, cid, inpacket, outpacket)
{
}

OltDownloadProcess::~OltDownloadProcess(void)
{
}


ProcessRes OltDownloadProcess::Begin()
{
	//����inpacket
	if(!parse_packet())
	{
		lerr << " parse input packet failed." << endl;
		return on_error(ERR_USR_PARSE_REQ);
	}

	//���͸�TextHandler
	TextHandler::GetInstance()->CreateResult(m_tid, m_cid, m_download_type, m_is_bilingual, mp_owner);

	return PROCESS_KEEP;
}

ProcessRes OltDownloadProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << " input event data is null." << endl;
		return PROCESS_KEEP;
	}

	EDType type = p_edata->GetType();

	if(EDTYPE_CREATE_RESULT_RES != type)
	{
		//ֻ����EDTYPE_CREATE_RESULT_REQ���ְ�
		lerr << "Recv event is not match type = " << type << endl;
		return PROCESS_KEEP;
	}
	
	//ת����TextCreateRes
	TextCreateRes * p_res = dynamic_cast<TextCreateRes*>(p_edata);

	if(!p_res)
	{
		lerr << "dynamic_cast TextCreateRes failed." << endl;
		return PROCESS_KEEP;
	}

	//���
	if(p_res->GetResult() == SUCCESS)
	{
		package_packet(p_res);
	}else
	{
		return on_error(p_res->GetResult());
	}

	return PROCESS_FINISH;
}



bool OltDownloadProcess::parse_packet()
{
	//��������
	/*
	<Msg>
		<TextID type="detail" bilingual="true">testid</TextID>
	</Msg>
	*/

	//��ñ�������
	string content;

	if(!m_input_packet.GetBody(content))
	{
		lerr << "ERROR: OltDownloadProcess::parse_packet() get packet's body failed." << endl;
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

		//�ڵ�TextID
		elem = docHandle.FirstChild("Msg").FirstChild("TextID").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: OltDownloadProcess::parse_packet() Parse Msg failed: TextID. " << endl;
			throw -1;
		}

		//��ȡ����
		const char * _tmp_type = elem->Attribute("type");
		
		if(_tmp_type)
		{
			m_download_type = _tmp_type;
			filter_head_tail(m_download_type);

			if(DOWNLOAD_TYPE_NORMAL != m_download_type
				&& DOWNLOAD_TYPE_TMX != m_download_type
				&& DOWNLOAD_TYPE_DETAIL != m_download_type
				&& DOWNLOAD_TYPE_TEXT != m_download_type)
			{
				lerr << "Not suport this download type = " << m_download_type << ", set it to normal." << endl;
				m_download_type = DOWNLOAD_TYPE_NORMAL;
			}
		}else
		{
			m_download_type = DOWNLOAD_TYPE_NORMAL;
		}

		const char * _tmp_bili = elem->Attribute("bilingual");
		
		if(_tmp_bili)
		{
			if(string(_tmp_bili) == "true")
				m_is_bilingual = true;
			else
				m_is_bilingual = false;
		}else
		{
			m_is_bilingual = false;
		}

		//��ȡtextid
		const char * _tmp_id = elem->GetText();
		if( !_tmp_id)
		{
            lerr << "ERROR: OltDownloadProcess::parse_packet() Parse Msg failed: TextID is null." << endl;
			throw -1;
		}
		
		m_tid = _tmp_id;
		del_head_tail_blank(m_tid);
		
	}catch (...)
	{
		res = false;
	}

	//����XML��Դ
	xmldoc.Clear();

	return res;

}

bool OltDownloadProcess::package_packet(TextCreateRes * p_create_res)
{
	if(!p_create_res)
	{
		lerr << "ERROR: OltDownloadProcess::package_packet() input TextCreateRes is null." << endl;
		return false;
	}

	//XML����
	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 

	/*
	<Msg>
		<ResCode>0</ResCode>
		<Content>
	 		<ResultFileName>dddd</ResultFileName>
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

		//ResultFileName
		TiXmlElement * fileElem = new TiXmlElement("ResultFileName");   
		TiXmlText * fileText = new TiXmlText(p_create_res->GetResultFileName()); 
		fileElem->LinkEndChild(fileText);
		contElem->LinkEndChild(fileElem);

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

	lout << xmldata << endl;

	//���뵽sendpacket
	this->m_output_packet.Write(HEAD_RESPONSE, MSG_TYPE_OLT_DOWNLOAD, xmldata);

	return PROCESS_FINISH;
}

