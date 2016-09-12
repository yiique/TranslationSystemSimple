
#ifndef NET_PACKET_H
#define NET_PACKET_H

//MAIN
#include "Common/f_utility.h"
#include "Log/LogStream.h"
#include "NetType.h"

//STL
#include <string>
#include <sstream>
using namespace std;

typedef enum{	WRITE_FINISH,
				WRITE_ERR_MAXHEAD, 
				WRITE_ERR_INPUT, 
				WRITE_ERR_MSGTYPE, 
				WRITE_ERR_CONTENTLEN, 
				WRITE_NORMAL, 
				PARSE_HEAD_SUC} 
PACKET_STATE_T;


typedef enum{HEAD_REQUEST, HEAD_RESPONSE} PACKET_HEAD_T;

#define MAX_HEAD_SIZE 1024 //报文头最大长度
#define DEF_MAX_BODY_SIZE 1024000 //报文体最大长度 //TODO


/* Typedef    报文头识别状态机
*  Since      v0.5 */
typedef struct headstatemachine
{
	headstatemachine(char b, headstatemachine *next): input_b(b),  next_state(next) {};

	char input_b;
	headstatemachine *next_state;
} HeadStateMachine;

class NetPacket
{
public:
	NetPacket(sock_t sock, const size_t max_body_size = DEF_MAX_BODY_SIZE);
	~NetPacket(void);
	
	//流式写入  用于从网络流中接收NetPacket
	PACKET_STATE_T Write(const char * buf, size_t len);

	//一次性写入 用于生产响应报文
	PACKET_STATE_T Write(PACKET_HEAD_T head_type,
					 	 const string &msg_type,
					 	 const string &body,
						 const string & head_res_code = HEAD_RESPONSE_200);

	//读取 
	const char * GetData()
	{
		return m_data.c_str();
	};

	size_t GetSize()
	{
		return m_data.size();
	};

	sock_t GetSock()
	{
		return m_sock;
	};

	void SetSock(sock_t sockfd)
	{
		m_sock = sockfd;
	};

	bool IsGood()
	{
		return m_is_good;
	};

	bool GetBody(string & body)
	{
		if(!m_is_good)
			return false;

		//lout<< "NetPacket::GetBody() m_body_pos = " << m_body_pos << endl << " m_data.size() = " <<  m_data.size() << endl;
		//lout<< "NetPacket::GetBody() m_data=" << m_data << endl;

		//printf("body = %s", m_data.c_str());
		body = m_data.substr(m_body_pos, m_data.size()-m_body_pos);

		//lout<< "NetPacket::GetBody() body.size =" << body.size() << endl;

		return true;

	};

	bool GetMsgType(string & msgtype)
	{
		if(!m_is_good)
			return false;

		msgtype = m_msg_type;

		return true;
	}

	void Clear()
	{
		m_curr_hsm = &NetPacket::HSM_Start;
		m_write_offset = 0;
		m_body_size = 0;
		m_body_pos = 0;
		m_is_good = false;
		m_data.clear();

	};
private:

	static const string HEAD_REQUEST_TAG;
	static const string HEAD_RESPONSE_TAG;
	static const string HEAD_MSGTYPE_TAG;
	static const string HEAD_LENGTH_TAG;
	static const string HEAD_SEP_TAG;
	
	static const string HEAD_REQUEST_VER;
	static const string HEAD_RESPONSE_200;

	/* Member    消息头识别状态机，用于从流中判别分隔标记: “\r\n\r\n”
	*  Since      v0.5 */
	static HeadStateMachine HSM_Start;
	static HeadStateMachine HSM_Fir_R;
	static HeadStateMachine HSM_Fir_N;
	static HeadStateMachine HSM_Sec_R;
	static HeadStateMachine HSM_End_N;
	

	int m_write_offset;

	PACKET_STATE_T m_state;

	sock_t m_sock;

	string m_data;
	PACKET_HEAD_T m_head_type;
	string m_msg_type;
	size_t m_body_size;
	size_t m_body_pos;
	
	HeadStateMachine * m_curr_hsm;

	//解析报文头
	PACKET_STATE_T parse_head();

	bool m_is_good;

	//最大body长度限制
	const size_t m_max_body_size;
};




#endif //NET_PACKET_H

