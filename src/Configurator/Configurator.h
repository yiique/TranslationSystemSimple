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

	string  _local_ip;    //本地IP地址
	int     _local_usr_port;  //本地侦听端口(用户交互)
	int     _local_ser_port;  //本地侦听端口(服务器交互)

	string  _db_name;    //数据库名
	string  _db_ip;      //数据库IP地址
	int     _db_port;    //数据库端口地址
	string  _db_usr;     //数据库用户名
	string  _db_pwd;     //数据库密码

	//int     _db_threads_num; //数据库处理线程数
	int     _pre_rtt_threads_num; //即时翻译预处理线程数
	int     _pre_olt_threads_num; //文件翻译预处理线程数
	int     _post_threads_num; //后处理线程数
	int     _text_threads_num;  //文本处理线程数
	int     _general_threads_num; //通用操作线程数
	string     _text_result_path; //文本结果储存路径

    set<DomainType> _domain; //翻译领域

	string _usr_dict_weight;
	string _pro_dict_weight;
	string _sys_dict_weight;

	bool _enable_remote_dict;  //是否启用远程词典
	

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


