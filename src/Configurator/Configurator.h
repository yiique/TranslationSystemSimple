#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

//MAIN
#include "Common/f_utility.h"
#include "Common/BasicType.h"
#include "Log/LogStream.h"

//STL
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class Configurator
{
public:
	Configurator(void);
	~Configurator(void);

	bool ReadConfig(const char * filepath);

	void PrintConfigInfo();

public:

	string  _local_ip;    //����IP��ַ
	int     _local_usr_port;  //���������˿�(�û�����)
	int     _local_ser_port;  //���������˿�(����������)

	string  _db_name;    //���ݿ���
	string  _db_ip;      //���ݿ�IP��ַ
	int     _db_port;    //���ݿ�˿ڵ�ַ
	string  _db_usr;     //���ݿ��û���
	string  _db_pwd;     //���ݿ�����

	//int     _db_threads_num; //���ݿ⴦���߳���
	int     _pre_rtt_threads_num; //��ʱ����Ԥ�����߳���
	int     _pre_olt_threads_num; //�ļ�����Ԥ�����߳���
	int     _post_threads_num; //�����߳���
	int     _text_threads_num;  //�ı������߳���
	int     _general_threads_num; //ͨ�ò����߳���
	string     _text_result_path; //�ı��������·��

    set<DomainType> _domain; //��������

	string _usr_dict_weight;
	string _pro_dict_weight;
	string _sys_dict_weight;

	bool _enable_remote_dict;  //�Ƿ�����Զ�̴ʵ�
	

	static size_t _s_max_rtt_src_size;

	static string _s_kr_rpc_ip;
	static string _s_kr_rpc_port;
	static string _s_ti_rpc_ip;
	static string _s_ti_rpc_port;
	static string _s_thai_seg_port;
	static string _s_thai_seg_ip;

	static string uy_netrans_prob; //uy netrans 

private:

	bool read_content( ifstream & file, string & content );

};

#endif //CONFIGURATOR_H


