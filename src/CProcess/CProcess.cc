

#include "CProcess.h"


CProcess::CProcess(EventEngine * p_owner, 
					const CallID & cid, 
					NetPacket & inpacket, 
					NetPacket & outpacket): mp_owner(p_owner),
											m_cid(cid),
											m_input_packet(inpacket),
											m_output_packet(outpacket)
{
}

CProcess::~CProcess(void)
{
}

ProcessType CProcess::GetType() const
{
	return PROCESS_TYPE_NORMAL;
}

ProcessRes CProcess::Begin()
{
	//Ĭ������  echoԭ����
	m_output_packet.Clear();
	string body;
	string type;
	m_input_packet.GetBody(body);
	m_input_packet.GetMsgType(type);
	string reBody = "ECHO: ";
	reBody += body;
	m_output_packet.Write(HEAD_RESPONSE, type, reBody);

	return PROCESS_FINISH;
}

ProcessRes CProcess::Work(EventData * p_edata)
{
	//Ĭ������ ֱ�ӷ���PROCESS_FINISH��ʾ����
	return PROCESS_FINISH;

}

ProcessRes CProcess::on_error(int err_code)
{
	lerr << "Usr Process failed. err code = " << err_code << endl;
	//���ɻظ�����
	//XML����
	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 

	/*
	<Msg>
		<ResCode>err_code</ResCode>
		<Content/>
	</Msg>

	*/

	try
    {
		//msg �ڵ�
		TiXmlElement * msgElem = new TiXmlElement("Msg");   
		xmlDocs->LinkEndChild(msgElem);   

		//����ResCode�ڵ�
		TiXmlElement * codeElem = new TiXmlElement("ResCode");   
		TiXmlText * codeText = new TiXmlText(num_2_str(err_code)); 
		codeElem->LinkEndChild(codeText);
		msgElem->LinkEndChild(codeElem);

		//����Content�ڵ�
		TiXmlElement * contElem = new TiXmlElement("Content");   
		msgElem->LinkEndChild(contElem);


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
	string msg_type;
	if( !this->m_input_packet.GetMsgType(msg_type) )
	{
		msg_type = "unknow";
	}

	m_output_packet.Clear();
	this->m_output_packet.Write(HEAD_RESPONSE, msg_type , xmldata);


	return PROCESS_FINISH;
}


