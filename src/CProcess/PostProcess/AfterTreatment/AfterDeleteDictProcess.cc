#include "AfterDeleteDictProcess.h"

AfterDeleteDictProcess::AfterDeleteDictProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

AfterDeleteDictProcess::~AfterDeleteDictProcess(void)
{
}


ProcessRes AfterDeleteDictProcess::Begin()
{

	//�������

	string s;
	m_input_packet.GetMsgType(s);
	
	AfterDictDeleteReq * p_dict_delete = new AfterDictDeleteReq(m_cid);
	if( !parse_packet(p_dict_delete))
	{
		delete p_dict_delete;
		ldbg2 << "Parse input packet failed, bulid error packet." << endl;
		return on_error(-1);
	}

	Event e(p_dict_delete, mp_owner);
	GeneralOperation::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}



ProcessRes AfterDeleteDictProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	//lout<<"DeleteDictProcess:: Work() "<< _event_data->GetType() << endl;
	//ת��ΪPulseResponse
	AfterDictDeleteRes * p_res = dynamic_cast<AfterDictDeleteRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to AfterDictDeleteRes failed." << endl;
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

bool AfterDeleteDictProcess::parse_packet(AfterDictDeleteReq * p_delete_req)
{

	if(!p_delete_req)
	{
		ldbg2 << "Input p_dict_delete is null." << endl;
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
		<AfterDictID>xxx</AfterDictID>
	</Msg>
	*/
	try
	{
		TiXmlElement* elem;
		
		elem = docHandle.FirstChild().ToElement();
		TiXmlNode * pchild = elem->FirstChild("AfterDictID");
		while(pchild)
		{
			TiXmlElement * child_elem = pchild->ToElement();
			const char * _tmp_dictid = child_elem->GetText();
			if( !_tmp_dictid)
			{
				ldbg2 << "Parse AfterDictDelete failed: usrid is null." << endl;
				pchild = pchild->NextSibling();
				continue;
			}

			string _tmp_dictid_s = _tmp_dictid;
			filter_head_tail(_tmp_dictid_s);
			p_delete_req->AddDictID(str_2_num(_tmp_dictid));
			pchild = pchild->NextSibling();
		}
	}catch (...)
	{
		res = false;
	}

	//����XML��Դ
	xmldoc.Clear();
	//lout<< "--> DeleteDictProcess::parse_packet() end." << endl;

	return res;
}

bool AfterDeleteDictProcess::package_packet(AfterDictDeleteRes * p_delete_res)
{
	if(!p_delete_res)
	{
		ldbg2 << "Input p_delete_res is null." << endl;
		return false;
	}
	/*
	string content;
	content = "<Msg><Result>";
	content += num_2_str(pCmdRes->m_result);
	content += "</Result></Msg>";
	*/
	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 
	
	try
    {
		TiXmlElement * msgElem = new TiXmlElement("Msg");   
		xmlDocs->LinkEndChild(msgElem);   

		//����ResCode�ڵ�
		TiXmlElement * codeElem = new TiXmlElement("ResCode");
		TiXmlText * codeText = new TiXmlText("0"); 
		codeElem->LinkEndChild(codeText);
		msgElem->LinkEndChild(codeElem);
		TiXmlElement * contElem_content = new TiXmlElement("Content");   
		msgElem->LinkEndChild(contElem_content);

		TiXmlElement * submitListElem = new TiXmlElement("AfterDeleteDictRes");   		
		contElem_content->LinkEndChild(submitListElem);

		stringstream ss;
		map<AfterDictID, int>::const_iterator iter = p_delete_res->GetFailureList().begin();
		for(; iter != p_delete_res->GetFailureList().end(); ++iter)
			ss << "(" << iter->first << "," << iter->second << ");";

		submitListElem->SetAttribute("failureID", ss.str());

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

	return true;
}

