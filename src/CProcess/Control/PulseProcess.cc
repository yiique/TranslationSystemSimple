#include "PulseProcess.h"

PulseProcess::PulseProcess(EventEngine * p_owner, 
						   const CallID & cid, 
						   NetPacket & inpacket, 
						   NetPacket & outpacket): CProcess(p_owner, cid, inpacket, outpacket)
{
}

PulseProcess::~PulseProcess(void)
{
}


ProcessRes PulseProcess::Begin()
{

	//解析inpacket数据到PluseRequest
	if( !parse_packet())
	{
		lerr << "ERROR: PulseProcess::Begin() parse input packet failed, bulid error packet." << endl;
		return on_error(ERR_ILLEGAL_MSG);
	}

	//判断应该发送给哪一个翻译服务器
	if(m_slave_trans_type == TRANS_TYPE_RTT)
	{
		TransController::GetRttInstance()->PostSlavePluse(m_slave_info, m_cid, mp_owner);

	}else if(m_slave_trans_type == TRANS_TYPE_OLT)
	{
		TransController::GetOltInstance()->PostSlavePluse(m_slave_info, m_cid, mp_owner);

	}else 
	{
		lerr << "Plus type error. input type is : " << m_slave_trans_type << endl;
	}

	return PROCESS_KEEP;
}

ProcessRes PulseProcess::Work(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "ERROR: PulseProcess::Work() input _event_data is null." << endl;
		return PROCESS_KEEP;
	}

	//转换为PulseResponse
	SlavePulseRes *p_res = dynamic_cast<SlavePulseRes*>(p_edata);

	if(!p_res)
	{
		lerr << "ERROR: PulseProcess::Work() convert to SlavePulseRes failed." << endl;
		return PROCESS_KEEP;
	}

	//将结果打包到m_output_packet
	if(!package_packet(p_res))
	{
		lerr << "ERROR: PulseProcess::Work() package_packet failed, create default error packet." << endl;
		return on_error(ERR_PULSE_NORMAL);
	}
	
	return PROCESS_FINISH;
}

bool PulseProcess::parse_packet()
{

	//获得报文数据
	string content;

	if(!m_input_packet.GetBody(content))
	{
		lerr << "ERROR: PulseProcess::parse_packet() get packet's body failed." << endl;
		return false;
	}

	//解析XML
	TiXmlDocument xmldoc;
	xmldoc.Parse(content.c_str());
	TiXmlHandle docHandle( &xmldoc );

	//报文格式：
	/*
		<Msg>
			<TransServer>
				<TransType>RTT<TransType> //or: OLT
				<SerUUID>XXXXXXXXX</SerUUID>
				<Domain>news</Domain>
				<LanguageSrc>english</LanguageSrc>
				<LnaguageTgt>chinese</LnaguageTgt>
				<IP>10.28.0.12</IP>
				<PORT>50001</PORT>
				<CPUCore>2</CPUCore>
				<CPUUse>76%</CPUUse>
				<TotalMem>2048MB</TotalMem>
				<UseMem>1800MB</UseMem>
			</TransServer>
		</Msg>
	*/
	bool res = true;

	try
	{
		TiXmlElement* elem;

		//节点TransType
		elem = docHandle.FirstChild("Msg").FirstChild("TransServer").FirstChild("TransType").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: TransType. " << endl;
			throw -1;
		}
		const char * _tmp_trans_type = elem->GetText();
		if( !_tmp_trans_type)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: TransType is null." << endl;
			throw -1;
		}
		string _tmp_trans_type_s = _tmp_trans_type;
		del_head_tail_blank(_tmp_trans_type_s);

		if(_tmp_trans_type_s == TRANS_TYPE_RTT_STR)
		{
			m_slave_trans_type = TRANS_TYPE_RTT;
		}else if(_tmp_trans_type_s == TRANS_TYPE_OLT_STR)
		{
			m_slave_trans_type = TRANS_TYPE_OLT;
		}else
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: TransType val is illegal. Recv TransType = " << _tmp_trans_type_s << endl;
			throw -1;
		}

		//节点SerUUID
		elem = docHandle.FirstChild().FirstChild().FirstChild("SerUUID").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: SerUUID. " << endl;
			throw -1;
		}

		const char * _tmp_ser_uuid = elem->GetText();
		if( !_tmp_ser_uuid)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: SerUUID is null." << endl;
			throw -1;
		}

		m_slave_info.slave_uuid = _tmp_ser_uuid;
		del_head_tail_blank(m_slave_info.slave_uuid);

		//节点Domain
		elem = docHandle.FirstChild().FirstChild().FirstChild("Domain").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: domain. " << endl;
			throw -1;
		}

		const char * _tmp_domain_name = elem->GetText();
		if( !_tmp_domain_name)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: domain is null." << endl;
			throw -1;
		}
		
		//节点LanguageSrc
		elem = docHandle.FirstChild().FirstChild().FirstChild("LanguageSrc").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: LanguageSrc. " << endl;
			throw -1;
		}

		const char * _tmp_language_src = elem->GetText();
		if( !_tmp_language_src)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: LanguageSrc is null." << endl;
			throw -1;
		}

		//节点LanguageTgt
		elem = docHandle.FirstChild().FirstChild().FirstChild("LanguageTgt").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: LanguageTgt. " << endl;
			throw -1;
		}

		const char * _tmp_language_tgt = elem->GetText();
		if( !_tmp_language_tgt)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: LanguageTgt is null." << endl;
			throw -1;
		}

	//--生成DomainType
		m_slave_info.domain.first = _tmp_domain_name;
		m_slave_info.domain.second.first = _tmp_language_src;
		m_slave_info.domain.second.second = _tmp_language_tgt;


		//节点IP
		elem = docHandle.FirstChild().FirstChild().FirstChild("IP").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: IP. " << endl;
			throw -1;
		}

		const char * _tmp_ip = elem->GetText();
		if( !_tmp_ip)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: IP is null." << endl;
			throw -1;
		}
		
		//节点PORT
		elem = docHandle.FirstChild().FirstChild().FirstChild("PORT").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: PORT. " << endl;
			throw -1;
		}
		
		const char * _tmp_sport = elem->GetText();
		if( !_tmp_sport)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: PORT is null." << endl;
			throw -1;
		}
		
		int _tmp_port;
		if(!cstr_2_num(_tmp_sport, _tmp_port) )
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: PORT str to int failed." << endl;
			throw -1;
		}

		if(_tmp_port <=0 || _tmp_port > 65535)
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: PORT val is illegal." << endl;
			throw -1;
		}


	//--生成ServerAddr
		m_slave_info.ip = _tmp_ip;
		m_slave_info.port = _tmp_port;
		del_head_tail_blank(m_slave_info.ip);
		

		//节点CPUCore
		elem = docHandle.FirstChild().FirstChild().FirstChild("CPUCore").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: CPUCore. " << endl;
			throw -1;
		}
		
		const char * _tmp_scpucore = elem->GetText();
		if( !_tmp_scpucore)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: CPUCore is null." << endl;
			throw -1;
		}

		m_slave_info.concurrent_num = str_2_num(_tmp_scpucore);

		/*

		int max_trans_num;
		if(!cstr_2_num(_tmp_scpucore, max_trans_num) )
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: CPUCore str to int failed." << endl;
			throw -1;
		}

		if(max_trans_num <=0)
		{
			p_pulse_req->SetMaxTransNum(0);
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: CPUCore val is illegal." << endl;
			throw -1;
		}else
		{
			p_pulse_req->SetMaxTransNum(max_trans_num);
		}

		//节点CPUUse
		elem = docHandle.FirstChild().FirstChild().FirstChild("CPUUse").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: CPUUse. " << endl;
			throw -1;
		}

		const char * _tmp_cpu_use = elem->GetText();
		if( !_tmp_cpu_use)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: CPUUse is null." << endl;
			throw -1;
		}

		string _tmp_cpu_use_s = _tmp_cpu_use;
		del_head_tail_blank(_tmp_cpu_use_s);
		p_pulse_req->SetCpuUse(_tmp_cpu_use);

		//节点TotalMem
		elem = docHandle.FirstChild().FirstChild().FirstChild("TotalMem").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: TotalMem. " << endl;
			throw -1;
		}

		const char * _tmp_mem_total = elem->GetText();
		if( !_tmp_mem_total)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: TotalMem is null." << endl;
			throw -1;
		}

		string _tmp_mem_total_s = _tmp_mem_total;
		del_head_tail_blank(_tmp_mem_total_s);
		p_pulse_req->SetMemTotal(_tmp_mem_total);

		//节点UseMem
		elem = docHandle.FirstChild().FirstChild().FirstChild("UseMem").ToElement();
		if( !elem ) 
		{
			lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: UseMem. " << endl;
			throw -1;
		}

		const char * _tmp_mem_use = elem->GetText();
		if( !_tmp_mem_use)
		{
            lerr << "ERROR: PulseProcess::parse_packet() Parse PulseMsg failed: UseMem is null." << endl;
			throw -1;
		}

		string _tmp_mem_use_s = _tmp_mem_use;
		del_head_tail_blank(_tmp_mem_use_s);
		p_pulse_req->SetMemUse(_tmp_mem_use);
		*/

	}catch (...)
	{
		res = false;
	}

	//清理XML资源
	xmldoc.Clear();

	return res;
	
}

bool PulseProcess::package_packet(SlavePulseRes * p_pulse_res)
{
	if(!p_pulse_res)
	{
		lerr << "ERROR: PulseProcess::package_packet() input PulseResponse is null." << endl;
		return false;
	}

	string content;
	content = "<Msg><Result>";
	content += num_2_str(p_pulse_res->GetResult());
	content += "</Result></Msg>";

	string type;
	m_input_packet.GetMsgType(type);

	m_output_packet.Write(HEAD_RESPONSE, type, content);

	return true;
}

