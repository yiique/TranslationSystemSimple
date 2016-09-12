#include "DeleteChSegWordProcess.h"

DeleteChSegWordProcess::DeleteChSegWordProcess(EventEngine * p_owner, 
									 const CallID & cid, 
									 NetPacket & inpacket, 
									 NetPacket & outpacket): CProcess(p_owner, cid, inpacket,outpacket)
{
}

DeleteChSegWordProcess::~DeleteChSegWordProcess(void)
{
}


ProcessRes DeleteChSegWordProcess::Begin()
{
	
	//解析inpacket

	//获得数据
	string s;
	m_input_packet.GetMsgType(s);
	DeleteChSegWordReq * p_req = new DeleteChSegWordReq(m_cid);
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


ProcessRes DeleteChSegWordProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		ldbg2 << "Input _event_data is null." << endl;
		return PROCESS_KEEP;
	}
	
	DeleteChSegWordRes * p_res = dynamic_cast<DeleteChSegWordRes*>(p_edata);

	if(!p_res)
	{
		lerr << "Convert to DeleteChSegWordRes failed." << endl;
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

bool DeleteChSegWordProcess::parse_packet(DeleteChSegWordReq * p_req)
{

	if(!p_req)
	{
		ldbg2 << "Input p_req is null." << endl;
		return false;
	}

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
		<DeleteChSegWordID>xxx</DeleteChSegWordID>
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
			elem = docHandle.FirstChild().Child("DeleteChSegWordID", idx).ToElement();
			if( !elem) 
			{
				ldbg2 << "Parse Msg failed: DeleteChSegWord: ID. " << endl;
				throw -1;
			}

			const char * _tmp_id = elem->GetText();
			if( !_tmp_id)
			{
			    ldbg2 << "Parse Msg failed: DeleteChSegWord: ID is null." << endl;
				throw -1;
			}

			p_req->AddWordID(str_2_num(_tmp_id));

			++idx;
		}

	}catch (...)
	{
		
	}

	//清理XML资源
	xmldoc.Clear();

	if(p_req->GetWordIdVec().size() == 0)
	{
		lwrn << "Delete Seg Word list is null." << endl;
		return false;
	}

	return res;
}

bool DeleteChSegWordProcess::package_packet(DeleteChSegWordRes * p_res)
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

    }catch (...)
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
		lerr << "DeleteChSegWord Error: " << xmldata << endl;
	}

	return true;
}

