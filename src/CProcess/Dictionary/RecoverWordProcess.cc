#include "RecoverWordProcess.h"

RecoverWordProcess::RecoverWordProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

RecoverWordProcess::~RecoverWordProcess(void)
{
}


ProcessRes RecoverWordProcess::Begin()
{
	
	//����inpacket

	//�������

	string s;
	m_input_packet.GetMsgType(s);

	WordRecoverReq * p_req = new WordRecoverReq(m_cid);
	if( !parse_packet(p_req))
	{
		delete p_req;
		ldbg2 << "Parse input packet failed, build error packet." << endl;
		return on_error(-1);
	}

	
	Event e(p_req, mp_owner);
	DictProcessor::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes RecoverWordProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	WordRecoverRes * p_res = dynamic_cast<WordRecoverRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to WordRecoverRes failed." << endl;
		return PROCESS_KEEP;
	}

	//����������m_output_packet
	if( !package_packet(p_res) )
	{
		ldbg2 << "Package_packet failed, delete default error packet." << endl;
		//TODO delete_error_repacket();
	}
	
	//���ݵ�ǰ״̬��֤�Ƿ�Ϊ
	return PROCESS_FINISH;//DEBUG
}

bool RecoverWordProcess::parse_packet(WordRecoverReq * p_recover_req)
{

	if(!p_recover_req)
	{
		ldbg2 << "Input p_recover_req is null." << endl;
		return false;
	}

	//��ñ�������
	string content;
	if(!m_input_packet.GetBody(content))
	{
		ldbg2 << "Get packet's body failed." << endl;
		return false;
	}

	//����XML
	TiXmlDocument xmldoc;
	xmldoc.Parse(content.c_str());
	TiXmlHandle docHandle( &xmldoc );

	bool res = true;
	/*
	<Msg>
		<DictID> dict_id </DictID>
		<DeleteWordID>xxx</DeleteWordID>
	</Msg>
	*/
	try
	{
		TiXmlElement* elem;
		
		//�ڵ�DictID
	
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
		p_recover_req->SetDictID(str_2_num(_tmp_dictid_s));


		//src
		elem = docHandle.FirstChild().FirstChild("RecoverWordID").ToElement();
		if( !elem) 
		{
			ldbg2 << "Parse Msg failed: RecoverWordID:Src. " << endl;
			throw -1;
		}

		const char * _tmp_del_word_id = elem->GetText();
		if( !_tmp_del_word_id)
		{
            ldbg2 << "Parse Msg failed: RecoverWordID:Src is null." << endl;
			throw -1;
		}

		string _tmp_del_word_id_s = _tmp_del_word_id;
		filter_head_tail(_tmp_del_word_id_s);
		p_recover_req->SetWordID(str_2_num(_tmp_del_word_id_s));


	}catch (...)
	{
		res = false;
	}

	//����XML��Դ
	xmldoc.Clear();

	return res;
}

bool RecoverWordProcess::package_packet(WordRecoverRes * p_recover_res)
{
	if(!p_recover_res)
	{
		ldbg2 << "Input p_delete_res is null." << endl;
		return false;
	}

	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 
	
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
		TiXmlElement * contElem_content = new TiXmlElement("Content");   
		msgElem->LinkEndChild(contElem_content);
		TiXmlElement * submitListElem = new TiXmlElement("RecoverWordRes");   
		contElem_content->LinkEndChild(submitListElem);

		submitListElem->SetAttribute("result", p_recover_res->GetRes());
		
		//�����xmldata
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

	if(SUCCESS != p_recover_res->GetRes())
	{
		lerr << "RecoverWord Error: " << xmldata << endl;
	}

	return true;
}

