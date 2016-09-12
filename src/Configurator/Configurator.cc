#include "Configurator.h"

size_t Configurator::_s_max_rtt_src_size = 500;
string Configurator::_s_kr_rpc_ip = "127.0.0.1";
string Configurator::_s_kr_rpc_port = "30001";
string Configurator::_s_ti_rpc_ip = "127.0.0.1";
string Configurator::_s_ti_rpc_port = "30002";
string Configurator::uy_netrans_prob = "1";

string Configurator::_s_thai_seg_port;
string Configurator::_s_thai_seg_ip;

Configurator::Configurator(void) : _pro_dict_weight("50 50 50 50"), _sys_dict_weight("20 20 20 20"), _usr_dict_weight("100 100 100 100"), _enable_remote_dict(false)
{
}

Configurator::~Configurator(void)
{
}


bool Configurator::ReadConfig(const char * filepath)
{
	if( !filepath )
	{
		cerr << "ERROR: Configurator::ReadConfig() input filepath is null. " << endl;
		return false;
	}

	lout << "NOTIFY: Config file path = [ " << filepath << " ]" << endl;

	ifstream file(filepath);

	if(!file.good())
		return false;
	
	bool loop = true;
	while(loop)
	{
		string content;

		if( !read_content(file, content) ) break;
		

		if( "[LOCAL_IP]" == content )
		{
			if( !read_content(file, _local_ip) ) break;
			
		}else if("[LOCAL_USR_PORT]" == content)
		{
			string s_local_usr_port;

			if( !read_content(file, s_local_usr_port) ) break;

			_local_usr_port = str_2_num(s_local_usr_port);

		}else if("[LOCAL_SER_PORT]" == content)
		{
			string s_local_ser_port;

			if( !read_content(file, s_local_ser_port) ) break;

			_local_ser_port = str_2_num(s_local_ser_port);

		}else if("[DB_NAME]" == content)
		{
			if( !read_content(file, _db_name) ) break;

		}else if("[DB_IP]" == content)
		{
			if( !read_content(file, _db_ip) ) break;

		}else if("[DB_PORT]" == content)
		{
			string s_db_port;

			if( !read_content(file, s_db_port) ) break;

			_db_port = str_2_num(s_db_port);

		}else if("[DB_USR]" == content)
		{
			if( !read_content(file, _db_usr) ) break;

		}else if("[DB_PWD]" == content)
		{
			if( !read_content(file, _db_pwd) ) break;

		}/*else if("[DB_THREADS_NUM]" == content)
		{
			string s_db_threads_num;

			if( !read_content(file, s_db_threads_num) ) break;

			_db_threads_num = str_2_num(s_db_threads_num);

		}*/else if("[PRE_RTT_THREADS_NUM]" == content)
		{
			string s_pre_threads_num;

			if( !read_content(file, s_pre_threads_num) ) break;

			_pre_rtt_threads_num = str_2_num(s_pre_threads_num);

		}else if("[PRE_OLT_THREADS_NUM]" == content)
		{
			string s_pre_threads_num;

			if( !read_content(file, s_pre_threads_num) ) break;

			_pre_olt_threads_num = str_2_num(s_pre_threads_num);

		}else if("[POST_THREADS_NUM]" == content)
		{
			string s_post_threads_num;

			if( !read_content(file, s_post_threads_num) ) break;

			_post_threads_num = str_2_num(s_post_threads_num);

		}else if("[TEXT_THREADS_NUM]" == content)
		{
			string s_text_threads_num;

			if( !read_content(file, s_text_threads_num) ) break;

			_text_threads_num = str_2_num(s_text_threads_num);

		}else if("[GENERAL_THREADS_NUM]" == content)
		{
			string s_general_threads_num;

			if( !read_content(file, s_general_threads_num) ) break;

			_general_threads_num = str_2_num(s_general_threads_num);

		}else if("[TEXT_RESULT_PATH]" == content)
		{

			if( !read_content(file, _text_result_path) ) break;

		}else if("[SYS_DICT_WEIGHT]" == content)
		{
			
			if( !read_content(file, _sys_dict_weight) ) break;

		}else if("[PRO_DICT_WEIGHT]" == content)
		{
			
			if( !read_content(file, _pro_dict_weight) ) break;

		}else if("[USR_DICT_WEIGHT]" == content)
		{
			
			if( !read_content(file, _usr_dict_weight) ) break;

		}else if("[ENABLE_REMOTE_DICT]" == content)
		{
			string s_enable_remote_dict;

			if( !read_content(file, s_enable_remote_dict) ) break;

			if(s_enable_remote_dict == "YES")
				_enable_remote_dict = true;
			else
				_enable_remote_dict = false;

		}else if("[KR_RPC_IP]" == content)
		{
			if( !read_content(file, _s_kr_rpc_ip) ) break;

		}else if("[KR_RPC_PORT]" == content)
		{
			if( !read_content(file, _s_kr_rpc_port) ) break;

		}else if("[TI_RPC_IP]" == content)
		{
			if( !read_content(file, _s_ti_rpc_ip) ) break;

		}else if("[TI_RPC_PORT]" == content)
		{
			if( !read_content(file, _s_ti_rpc_port) ) break;

		}else if("[THAI_SEG_IP]" == content)
		{
			if( !read_content(file, _s_thai_seg_ip) ) break;

		}else if("[THAI_SEG_PORT]" == content)
		{
			if( !read_content(file, _s_thai_seg_port) ) break;

		}else if("[UY_NE_PROB]" == content)
		{
			if( !read_content(file, uy_netrans_prob) ) break;

		}else if("[DOMAIN]" == content)
        {
            string domain;
            vector<string> tgt;
            LangDirectionType langtype;
            DomainType domaintype;
            
            if( !read_content(file, domain) ) break;

            split_sequence_by_tag(domain.c_str(), tgt, '|');
            if(tgt.size() != 3)
            {
                lout << "domain information is wrong : " << domain << endl;
            }

            langtype.first = del_head_tail_blank(tgt[1]);
            langtype.second = del_head_tail_blank(tgt[2]);

            domaintype.first = del_head_tail_blank(tgt[0]);
            domaintype.second = langtype;

            _domain.insert(domaintype);
        }
	}

	file.close();

	//输出配置信息
	PrintConfigInfo();

	return true;
}

bool Configurator::read_content( ifstream & file, string & content )
{
	string line;
	stringstream ss;

	if( getline(file, content) )
	{
		ss << line;
		ss >> content;
		
		//去掉首位空格和换行符（因为dos和linux不一样）
		filter_head_tail(content);

		return true;
	}else
	{
		return false;	
	}
}


void Configurator::PrintConfigInfo()
{
	lout << "|--------------------CONFIG INFO--------------------|" << endl;
	lout << "> _local_ip			 = " <<  _local_ip << endl;
	lout << "> _local_usr_port		 = " <<  _local_usr_port << endl;
	lout << "> _local_ser_port		 = " <<  _local_ser_port << endl;
	lout << "> _db_name				 = " <<  _db_name << endl;
	lout << "> _db_ip				 = " <<  _db_ip << endl;
	lout << "> _db_port				 = " <<  _db_port << endl;
	lout << "> _db_usr				 = " <<  _db_usr << endl;
	lout << "> _db_pwd				 = " <<  _db_pwd << endl;
	lout << "> _pre_rtt_threads_num  = " << _pre_rtt_threads_num << endl;
	lout << "> _pre_olt_threads_num  = " << _pre_olt_threads_num << endl;
	lout << "> _post_threads_num     = " << _post_threads_num << endl;
	lout << "> _text_threads_num	 = " << _text_threads_num << endl;
	lout << "> _general_threads_num  = " << _general_threads_num << endl;
	lout << "> _text_result_path	 = " << _text_result_path << endl;
	lout << "> _usr_dict_weight	     = " << _usr_dict_weight << endl;
	lout << "> _pro_dict_weight	     = " << _pro_dict_weight << endl;
	lout << "> _sys_dict_weight	     = " << _sys_dict_weight << endl;
	lout << "> _enable_remote_dict   = " << _enable_remote_dict << endl;
	lout << "> _s_kr_rpc_ip          = " << _s_kr_rpc_ip << endl;
	lout << "> _s_kr_rpc_port        = " << _s_kr_rpc_port << endl;
	lout << "> _s_ti_rpc_ip          = " << _s_kr_rpc_ip << endl;
	lout << "> _s_ti_rpc_port        = " << _s_kr_rpc_port << endl;
	lout << "> uy_ne_prob        = " << uy_netrans_prob << endl;
	
	set<DomainType>::iterator iter = _domain.begin();
	size_t idx = 0;
	lout << "> _domain num           = " << _domain.size() << endl;
	for(; iter != _domain.end(); ++iter)
	{
		lout << "> ____ " << idx++ << ". " << *iter << endl;
	}

	lout << "|----------------------------------------------------|" << endl << endl;
}

