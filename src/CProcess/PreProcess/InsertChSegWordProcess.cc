#include "InsertChSegWordProcess.h"

InsertChSegWordProcess::InsertChSegWordProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

InsertChSegWordProcess::~InsertChSegWordProcess(void)
{
}


ProcessRes InsertChSegWordProcess::Begin()
{
	
	//����inpacket

	//�������
	string s;
	m_input_packet.GetMsgType(s);
	InsertChSegWordReq * p_req = new InsertChSegWordReq(m_cid);
	if( !parse_packet(p_req))
	{
		delete p_req;
		ldbg2 << "Parse input packet failed, build error packet." << endl;
		return on_error(-1);
	}

	lout << "begin." << endl;

	Event e(p_req, mp_owner);
	GeneralOperation::GetInstance()->PostEvent(e); 
	return PROCESS_KEEP;
}


ProcessRes InsertChSegWordProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	InsertChSegWordRes * p_res = dynamic_cast<InsertChSegWordRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to InsertChSegWordRes failed." << endl;
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

bool InsertChSegWordProcess::parse_packet(InsertChSegWordReq * p_req)
{

	if(!p_req)
	{
		ldbg2 << "Input p_req is null." << endl;
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
		<InsertChSegWord>
			<Src>xxx</Src>
			<IsActive>1<IsActive>
		</InsertChSegWord>
		...
	</Msg>
	*/
	try
	{
		TiXmlElement* elem;

		size_t idx = 0;
		while(true)
		{
			
			string word;
			int is_active;

			//src
			elem = docHandle.FirstChild().Child("InsertChSegWord", idx).FirstChild("Src").ToElement();

			if( !elem) 
			{
				ldbg2 << "Parse Msg failed: InsertChSegWord:Src. " << endl;
				throw -1;
			}

			const char * _tmp_src = elem->GetText();
			if( !_tmp_src)
			{
			    ldbg2 << "Parse Msg failed: InsertChSegWord:Src is null." << endl;
				throw -1;
			}

			word = _tmp_src;
			filter_head_tail(word);

			//isactive
			//���ô����Ƿ����� ��ѡ  ��Ϊ�յĻ� ����Ϊ����
			try
			{
				elem = docHandle.FirstChild().Child("InsertChSegWord", idx).FirstChild("IsActive").ToElement();
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

				//���ô�������
				string _tmp_isactive_s = _tmp_isactive;
				filter_head_tail(_tmp_isactive_s);

				if("1" == _tmp_isactive_s)
					is_active = 1;
				else
					is_active = 0;

			}catch(...)
			{
				//���ô�������
				is_active = 1;
			}


			p_req->AddWord(seg_word_share_ptr(new ChSegWord(word, is_active)));

			++idx;

		}

	}catch (...)
	{
		
	}

	//����XML��Դ
	xmldoc.Clear();

	if(p_req->GetWordVec().size() == 0)
	{
		lwrn << "Insert Seg Word list is null." << endl;
		return false;
	}

	return res;
}

bool InsertChSegWordProcess::package_packet(InsertChSegWordRes * p_res)
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
		//msg �ڵ�
		TiXmlElement * msgElem = new TiXmlElement("Msg");   
		xmlDocs->LinkEndChild(msgElem);   

		//����ResCode�ڵ�
		TiXmlElement * codeElem = new TiXmlElement("ResCode");
		TiXmlText * codeText = new TiXmlText(num_2_str(p_res->GetResult())); 
		codeElem->LinkEndChild(codeText);
		msgElem->LinkEndChild(codeElem);
		
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

	if(SUCCESS != p_res->GetResult())
	{
		lerr << "InsertChSegWord Error: " << xmldata << endl;
	}

	return true;
}

