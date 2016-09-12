#include "NetPacket.h"

const string NetPacket::HEAD_REQUEST_TAG("Request:");
const string NetPacket::HEAD_RESPONSE_TAG("Response:");
const string NetPacket::HEAD_MSGTYPE_TAG("Msg-Type:");
const string NetPacket::HEAD_LENGTH_TAG("Content-Length:");
const string NetPacket::HEAD_SEP_TAG("\r\n");

const string NetPacket::HEAD_REQUEST_VER("1.0");
const string NetPacket::HEAD_RESPONSE_200("200 OK");

HeadStateMachine NetPacket::HSM_End_N('0' , NULL);
HeadStateMachine NetPacket::HSM_Sec_R('\n', &NetPacket::HSM_End_N);
HeadStateMachine NetPacket::HSM_Fir_N('\r', &NetPacket::HSM_Sec_R);
HeadStateMachine NetPacket::HSM_Fir_R('\n', &NetPacket::HSM_Fir_N);
HeadStateMachine NetPacket::HSM_Start('\r',  &NetPacket::HSM_Fir_R);


NetPacket::NetPacket(sock_t sock, const size_t max_body_size): m_sock(sock), 
															   m_write_offset(0),
															   m_body_size(0),
															   m_body_pos(0),
															   m_curr_hsm(&NetPacket::HSM_Start),
															   m_is_good(false),
															   m_max_body_size(max_body_size)
{
}


NetPacket::~NetPacket(void)
{
}

PACKET_STATE_T NetPacket::Write(PACKET_HEAD_T head_type, 
				 				const string &msg_type,
				 				const string &body,
								const string & head_res_code)
{
	stringstream buf;

	m_is_good = false;

	string h_field;
	if( head_type == HEAD_REQUEST )
		h_field = HEAD_REQUEST_TAG + HEAD_REQUEST_VER;
	else
		h_field = HEAD_RESPONSE_TAG +" " + head_res_code;

	//���챨��ͷ
	buf << h_field  << HEAD_SEP_TAG;  //EX: Request: 1.0\r\n
	buf << HEAD_MSGTYPE_TAG << msg_type          << HEAD_SEP_TAG;  //EX: Msg-Type: xxx\r\n
	buf << HEAD_LENGTH_TAG  << body.size()   << HEAD_SEP_TAG;  //EX: Content-Length: xxx\r\n
	buf << HEAD_SEP_TAG;                                           //EX: \r\n

	//���챨����
	buf << body;

	//��m_data��ֵ
	m_data = buf.str();

	m_is_good = true;

	return WRITE_FINISH;


}


PACKET_STATE_T NetPacket::parse_head()
{
	//lout<< "head" << endl << m_data << endl;

	//����"Content-Length"
	string sbodylen;
	if( !str_tag_find(m_data, NetPacket::HEAD_LENGTH_TAG, NetPacket::HEAD_SEP_TAG, sbodylen) )
		return WRITE_ERR_CONTENTLEN;

	int bodysize;
	if( !cstr_2_num(sbodylen.c_str(), bodysize) )
		return WRITE_ERR_CONTENTLEN;

	//if( bodysize < 0 || bodysize > m_max_body_size)
	if( bodysize < 0)
		return WRITE_ERR_CONTENTLEN;

	m_body_size = (size_t) bodysize;

	//lout<< "NetPacket::parse_head() content_length = " << bodysize << endl;

	//����"Msg-Type"
    if( !str_tag_find(m_data, NetPacket::HEAD_MSGTYPE_TAG, NetPacket::HEAD_SEP_TAG, m_msg_type) )		    
    	return WRITE_ERR_MSGTYPE;
    
    //ȥ����β�ո�      
    del_head_tail_blank(m_msg_type);

	//body��λ
	//lout << "Before Init m_body_pos = " << m_body_pos << endl;
	m_body_pos = m_data.size();

	//lout << "Init m_body_pos = " << m_body_pos << endl;

	return PARSE_HEAD_SUC;
}


PACKET_STATE_T NetPacket::Write(const char * buf, size_t len)
{
	if(!buf || len == 0)
		return WRITE_ERR_INPUT;

	//lout<< "NetPacket::Write() this write = " << len << endl;

	size_t idx = 0;
	char byte;

	//����״̬����ȡ"\r\n\r\n"
	while( m_curr_hsm != &NetPacket::HSM_End_N )
	{
		//2011-1-16 fix ������Ϊ��ʱ�޷���ȷ�����Ĵ���
		if(idx >= len)
		{
			//lout<< "��ȡ�������գ�ͷ��ʱ" << endl;
			return WRITE_NORMAL;
		}

		if(buf[idx] == '\0'){
			++idx;
			continue;
		}

		//д��ͷ��
		byte = buf[idx];
		m_data += byte;

		//����״̬��
		if( byte == m_curr_hsm->input_b )
			m_curr_hsm = m_curr_hsm->next_state;
		else
			m_curr_hsm = &NetPacket::HSM_Start;

		if(m_curr_hsm == &NetPacket::HSM_End_N)
		{
			//��������ͷ//
			PACKET_STATE_T res = parse_head();
			if( res != PARSE_HEAD_SUC )
				return res;
		}

		//����ƫ����
		++idx;

		if(idx >= MAX_HEAD_SIZE)
		{
			//������󳤶�
			return WRITE_ERR_MAXHEAD;
		}

	}


	//��ʼ��ȡ������
	//2011-1-16 fix ������Ϊ��ʱ�޷���ȷ�����Ĵ���
	if(m_body_size == 0)
	{
		m_is_good = true;
		return WRITE_FINISH;
	}

	while( idx < len)  
	{
		if(buf[idx] == '\0'){
			++idx;
			continue;
		}

		m_data += buf[idx];

		//lout<< "NetPacket::Write() buf["<< idx << "] = " << (int)buf[idx] << endl;
		//lout<< "                   m_data=[" << m_data << "]" << endl << endl;

		++idx;


		if(m_data.size()-m_body_pos == m_body_size)
		{
			//�������
			m_is_good = true;
			return WRITE_FINISH;
		}

	}



	return WRITE_NORMAL;

}


