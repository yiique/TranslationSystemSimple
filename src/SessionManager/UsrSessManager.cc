#include "UsrSessManager.h"

#include "CProcess/Translate/RttTransProcess.h"
#include "CProcess/Translate/OltTransProcess.h"
#include "CProcess/Translate/OltDownloadProcess.h"
#include "CProcess/Translate/RttSegProcess.h"

#include "CProcess/Dictionary/CreateDictProcess.h"
#include "CProcess/Dictionary/DeleteDictProcess.h"
#include "CProcess/Dictionary/InsertWordProcess.h"
#include "CProcess/Dictionary/DeleteWordProcess.h"
#include "CProcess/Dictionary/ModifyWordProcess.h"
#include "CProcess/Dictionary/ModifyDictProcess.h"
#include "CProcess/Dictionary/LoadWordProcess.h"
#include "CProcess/Dictionary/InsertSysDictProcess.h"

#include "CProcess/Template/InsertTemplateProcess.h"
#include "CProcess/Template/DeleteTemplateProcess.h"
#include "CProcess/Template/ModifyTemplateProcess.h"
#include "CProcess/Template/LoadTemplateProcess.h"
#include "CProcess/Template/CreateTemplateLibProcess.h"
#include "CProcess/Template/DeleteTemplateLibProcess.h"
#include "CProcess/Template/ModifyTemplateLibProcess.h"

#include "CProcess/Control/TransControlProcess.h"
#include "CProcess/Control/TransSentProcess.h"

#include "CProcess/PreProcess/InsertChSegWordProcess.h"
#include "CProcess/PreProcess/DeleteChSegWordProcess.h"
#include "CProcess/PreProcess/ModifyChSegWordProcess.h"
#include "CProcess/PreProcess/LoadChSegWordProcess.h"
#include "CProcess/PreProcess/RecoveryChSegWordProcess.h"


#include "CProcess/Dictionary/InsertSysDictProcess.h"
#include "CProcess/Template/InsertSysTemplateLibProcess.h"
#include "CProcess/Dictionary/RecoverDictProcess.h"
#include "CProcess/Dictionary/RecoverWordProcess.h"
#include "CProcess/Template/RecoverTemplateLibProcess.h"
#include "CProcess/Template/RecoverTemplateProcess.h"


#include "CProcess/PostProcess/AfterTreatment/AfterCreateDictProcess.h"
#include "CProcess/PostProcess/AfterTreatment/AfterDeleteDictProcess.h"
#include "CProcess/PostProcess/AfterTreatment/AfterModifyDictProcess.h"
#include "CProcess/PostProcess/AfterTreatment/AfterInsertWordProcess.h"
#include "CProcess/PostProcess/AfterTreatment/AfterDeleteWordProcess.h"
#include "CProcess/PostProcess/AfterTreatment/AfterModifyWordProcess.h"
#include "CProcess/PostProcess/AfterTreatment/AfterLoadWordProcess.h"
#include "CProcess/PostProcess/AfterTreatment/RecoverAfterDictProcess.h"
#include "CProcess/PostProcess/AfterTreatment/RecoverAfterWordProcess.h"

//唯一实例
UsrSessManager theUsrSessionManager(NetInterface::GetUsrNet());

UsrSessManager * UsrSessManager::GetInstance()
{
	return &theUsrSessionManager;
}

UsrSessManager::UsrSessManager(AsyncNetDriver * p_net_driver): SessionManager(p_net_driver)
{
}

UsrSessManager::~UsrSessManager(void)
{
}

bool UsrSessManager::Start()
{
	if( !SessionManager::Start() )
		return false;

	lout << " UsrSessManager this_ptr = " << (unsigned long ) this <<  endl;
	return true;
}

CProcess * UsrSessManager::create_process(CSession & session)
{
	
	if(!session._recv_packet.IsGood())
	{
		lerr << "ERROR: session's recv packet is not good." << endl;
		return NULL;
	}


	string msg_type;
	if(!session._recv_packet.GetMsgType(msg_type))
	{
		lerr << "ERROR: get recv packet's msg type failed." << endl;
		return NULL;
	}

	//ldbg1 << " recv_packet = " << session._recv_packet.GetData() << endl;
	
	cout << "cqw msg: " << msg_type << endl;
	if( MSG_TYPE_ECHO == msg_type )
	{
		return new CProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_RTT_SEG == msg_type )  // 即时切词
	{
		//先注册一个即时切词ID
		const TextID tid = registe_rtt_trans_text(session.GetCallID());
		//return new RttSegProcess(this, session.GetCallID(), tid, session._recv_packet, session._send_packet, " segmetation");
		return new RttSegProcess(this, session.GetCallID(), tid, session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_RTT_TRANS == msg_type )  // 即时翻译
	{
		//先注册一个即时翻译ID
		const TextID tid = registe_rtt_trans_text(session.GetCallID());
		//return new RttSegProcess(this, session.GetCallID(), tid, session._recv_packet, session._send_packet, " sentence translation or NE translation");
		return new RttTransProcess(this, session.GetCallID(), tid, session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_OLT_TRANS == msg_type) //文件翻译
	{
		return new OltTransProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_CREATE == msg_type) //创建词典
	{
		return new CreateDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_TEMPLATELIB_CREATE == msg_type) //创建模版库
    {
        return new CreateTemplateLibProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
    }else if( MSG_TYPE_TEMPLATELIB_DELETE == msg_type) //删除模版库
    {
        return new DeleteTemplateLibProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
    }else if( MSG_TYPE_TEMPLATELIB_MODIFY == msg_type) //修改模版库
    {
        return new ModifyTemplateLibProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
    }else if ( MSG_TYPE_TEMPLATE_INSERT == msg_type)
    {
        return new InsertTemplateProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
    }else if ( MSG_TYPE_TEMPLATE_DELETE == msg_type)
    {
        return new DeleteTemplateProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
    }else if ( MSG_TYPE_TEMPLATE_MODIFY == msg_type)
    {
        return new ModifyTemplateProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
    }else if (MSG_TYPE_TEMPLATE_LOAD == msg_type)
    {
        return new LoadTemplateProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
    }else if( MSG_TYPE_SYS_TEMPLATELIB_INSERT == msg_type) //导入到系统模版库
	{
		return new InsertSysTemplateLibProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_TEMPLATELIB_RECOVER == msg_type) //恢复模版库
	{
		return new RecoverTemplateLibProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_TEMPLATE_RECOVER == msg_type) //恢复模版
	{
		return new RecoverTemplateProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_DELETE == msg_type) //删除词典
	{
		return new DeleteDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_SYS_DICT_INSERT == msg_type) //导入到系统词典
	{
		return new InsertSysDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_RECOVER == msg_type) //恢复词典
	{
		return new RecoverDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_WORDS_RECOVER == msg_type) //恢复词条
	{
		return new RecoverWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_WORD_INSERT == msg_type) //插入词条
	{
		return new InsertWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_WORD_DELETE == msg_type) //删除词条
	{
		return new DeleteWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_WORD_LOAD == msg_type) //导入词典文件
	{
		return new LoadWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_WORD_MODIFY == msg_type) //修改词条
	{
		return new ModifyWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_MODIFY == msg_type) //修改词典
	{
		return new ModifyDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_OLT_DOWNLOAD == msg_type) //下载翻译结果
	{
		return new OltDownloadProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_OLT_CONTROL == msg_type) //离线翻译控制
	{
		return new TransControlProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_OLT_SENT == msg_type) //离线翻译获取当前已翻译的句子
	{
		return new TransSentProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_INSERT == msg_type) //添加中文分词词条
	{
		return new InsertChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_DELETE == msg_type) //删除中文分词词条
	{
		return new DeleteChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_MODIFY == msg_type) //修改中文分词词条
	{
		return new ModifyChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_LOAD == msg_type) //导入中文分词词典
	{
		return new LoadChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_RECOVERY == msg_type) //恢复删除的中文分词词条
	{
		return new RecoveryChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_DICT_CREATE == msg_type) //创建后处理词典
	{
		return new AfterCreateDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_DICT_DELETE == msg_type) //删除后处理词典
	{
		return new AfterDeleteDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_DICT_MODIFY == msg_type) //修改后处理词典
	{
		return new AfterModifyDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_WORD_INSERT == msg_type) //插入后处理词条
	{
		return new AfterInsertWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_WORD_DELETE == msg_type) //删除后处理词条
	{
		return new AfterDeleteWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_WORD_MODIFY == msg_type) //修改后处理词条
	{
		return new AfterModifyWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_WORD_LOAD == msg_type) //上传后处理词条
	{
		return new AfterLoadWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_DICT_WORDS_RECOVER == msg_type) //恢复后处理词条
	{
		return new RecoverAfterWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}
	else if( MSG_TYPE_AFTER_DICT_RECOVER == msg_type) //恢复后处理词典
	{
		return new RecoverAfterDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}
	else
	{
		lerr << "ERROR: Can't find this msg type : " << msg_type <<endl;
		return NULL;
	}
	return NULL;
}

TextID UsrSessManager::registe_rtt_trans_text(const CallID & sess_cid)
{
	const TextID tid = TransText::GenerateTextUUID();

	m_rtt_sess_text_map.insert(make_pair(tid, sess_cid)).second;

	return tid;
}

void UsrSessManager::end_process(CSession & session)
{

	if(session._p_process && PROCESS_TYPE_RTT == session._p_process->GetType())
	{

		//清理m_rtt_sess_text_map
		RttTransProcess * p_rtt_proc = dynamic_cast<RttTransProcess*>(session._p_process);

		if(NULL != p_rtt_proc)
		{
			m_rtt_sess_text_map.erase(p_rtt_proc->GetRttTextID());

		}else
		{
			lerr << "Can't cast process to RttTransProcess." << endl;
		}
	}
	if(session._p_process && PROCESS_TYPE_RTT_SEG == session._p_process->GetType()){

		//清理m_rtt_sess_text_map
		RttSegProcess * p_rtt_seg_proc = dynamic_cast<RttSegProcess*>(session._p_process);

		if(NULL != p_rtt_seg_proc)
		{
			m_rtt_sess_text_map.erase(p_rtt_seg_proc->GetRttTextID());

		}else
		{
			lerr << "Can't cast process to RttSegProcess." << endl;
		}
	}

	SessionManager::end_process(session);

}

void UsrSessManager::on_no_session_event(Event & e)
{

	//拦截即时翻译结果 根据TextID找到对应的session

	try
	{
		if(!e._p_edata)
		{
			lerr << "Input event's data is null." << endl;
			throw -1;
		}

		TransText * p_trans_text = NULL;

		if(e._p_edata->GetType() == EDTYPE_TRANS_RESULT)
		{
			TransResult * p_trans_result = dynamic_cast<TransResult*>(e._p_edata);

			if(!p_trans_result)
			{
				lerr << "Convert event data to TransResult failed." << endl;
				throw -1;
			}

			p_trans_text = p_trans_result->GetTransText();

		}else
		{
		
			throw -1;
		}
		

		if(!p_trans_text)
		{
			lerr << "TransText is null." << endl;
			throw -1;
		}


		//从textid-sesscallid对应表中查找
		map<TextID, CallID>::iterator iter = m_rtt_sess_text_map.find(p_trans_text->GetID());

		if(iter == m_rtt_sess_text_map.end())
		{
			lerr << "Can't find session_id in rtt session transtext map, delete transtext. text_id = " << p_trans_text->GetID() << endl;
			delete p_trans_text;
			throw -1;
		}

		//找出session
		session_map_t::iterator siter = m_session_map.find(iter->second);

		if(siter == m_session_map.end())
		{
			lerr << "Can't find session in session map. call_id = " << iter->second << endl;
			delete p_trans_text;
			throw -1;
		}

		//投递给对应的session处理
		SessionManager::on_proc_event(siter->second, e._p_edata); 

	}catch(...)
	{
		SessionManager::on_no_session_event(e);
	}
}



int UsrSessManager::on_listen_newsock(CSession & session, EventData * p_edata)
{
	int clifd = SessionManager::on_listen_newsock(session, p_edata);
	if(-1 == clifd)
		return -1;

	return clifd; 
}
