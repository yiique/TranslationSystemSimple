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

//Ψһʵ��
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
	}else if( MSG_TYPE_RTT_SEG == msg_type )  // ��ʱ�д�
	{
		//��ע��һ����ʱ�д�ID
		const TextID tid = registe_rtt_trans_text(session.GetCallID());
		//return new RttSegProcess(this, session.GetCallID(), tid, session._recv_packet, session._send_packet, " segmetation");
		return new RttSegProcess(this, session.GetCallID(), tid, session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_RTT_TRANS == msg_type )  // ��ʱ����
	{
		//��ע��һ����ʱ����ID
		const TextID tid = registe_rtt_trans_text(session.GetCallID());
		//return new RttSegProcess(this, session.GetCallID(), tid, session._recv_packet, session._send_packet, " sentence translation or NE translation");
		return new RttTransProcess(this, session.GetCallID(), tid, session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_OLT_TRANS == msg_type) //�ļ�����
	{
		return new OltTransProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_CREATE == msg_type) //�����ʵ�
	{
		return new CreateDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_TEMPLATELIB_CREATE == msg_type) //����ģ���
    {
        return new CreateTemplateLibProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
    }else if( MSG_TYPE_TEMPLATELIB_DELETE == msg_type) //ɾ��ģ���
    {
        return new DeleteTemplateLibProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
    }else if( MSG_TYPE_TEMPLATELIB_MODIFY == msg_type) //�޸�ģ���
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
    }else if( MSG_TYPE_SYS_TEMPLATELIB_INSERT == msg_type) //���뵽ϵͳģ���
	{
		return new InsertSysTemplateLibProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_TEMPLATELIB_RECOVER == msg_type) //�ָ�ģ���
	{
		return new RecoverTemplateLibProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_TEMPLATE_RECOVER == msg_type) //�ָ�ģ��
	{
		return new RecoverTemplateProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_DELETE == msg_type) //ɾ���ʵ�
	{
		return new DeleteDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_SYS_DICT_INSERT == msg_type) //���뵽ϵͳ�ʵ�
	{
		return new InsertSysDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_RECOVER == msg_type) //�ָ��ʵ�
	{
		return new RecoverDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_WORDS_RECOVER == msg_type) //�ָ�����
	{
		return new RecoverWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_WORD_INSERT == msg_type) //�������
	{
		return new InsertWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_WORD_DELETE == msg_type) //ɾ������
	{
		return new DeleteWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_WORD_LOAD == msg_type) //����ʵ��ļ�
	{
		return new LoadWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_WORD_MODIFY == msg_type) //�޸Ĵ���
	{
		return new ModifyWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_DICT_MODIFY == msg_type) //�޸Ĵʵ�
	{
		return new ModifyDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_OLT_DOWNLOAD == msg_type) //���ط�����
	{
		return new OltDownloadProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_OLT_CONTROL == msg_type) //���߷������
	{
		return new TransControlProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_OLT_SENT == msg_type) //���߷����ȡ��ǰ�ѷ���ľ���
	{
		return new TransSentProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_INSERT == msg_type) //������ķִʴ���
	{
		return new InsertChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_DELETE == msg_type) //ɾ�����ķִʴ���
	{
		return new DeleteChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_MODIFY == msg_type) //�޸����ķִʴ���
	{
		return new ModifyChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_LOAD == msg_type) //�������ķִʴʵ�
	{
		return new LoadChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_CHSEG_WORD_RECOVERY == msg_type) //�ָ�ɾ�������ķִʴ���
	{
		return new RecoveryChSegWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_DICT_CREATE == msg_type) //��������ʵ�
	{
		return new AfterCreateDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_DICT_DELETE == msg_type) //ɾ������ʵ�
	{
		return new AfterDeleteDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_DICT_MODIFY == msg_type) //�޸ĺ���ʵ�
	{
		return new AfterModifyDictProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_WORD_INSERT == msg_type) //����������
	{
		return new AfterInsertWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_WORD_DELETE == msg_type) //ɾ���������
	{
		return new AfterDeleteWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_WORD_MODIFY == msg_type) //�޸ĺ������
	{
		return new AfterModifyWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_WORD_LOAD == msg_type) //�ϴ��������
	{
		return new AfterLoadWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}else if( MSG_TYPE_AFTER_DICT_WORDS_RECOVER == msg_type) //�ָ��������
	{
		return new RecoverAfterWordProcess(this, session.GetCallID(), session._recv_packet, session._send_packet);
	}
	else if( MSG_TYPE_AFTER_DICT_RECOVER == msg_type) //�ָ�����ʵ�
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

		//����m_rtt_sess_text_map
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

		//����m_rtt_sess_text_map
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

	//���ؼ�ʱ������ ����TextID�ҵ���Ӧ��session

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


		//��textid-sesscallid��Ӧ���в���
		map<TextID, CallID>::iterator iter = m_rtt_sess_text_map.find(p_trans_text->GetID());

		if(iter == m_rtt_sess_text_map.end())
		{
			lerr << "Can't find session_id in rtt session transtext map, delete transtext. text_id = " << p_trans_text->GetID() << endl;
			delete p_trans_text;
			throw -1;
		}

		//�ҳ�session
		session_map_t::iterator siter = m_session_map.find(iter->second);

		if(siter == m_session_map.end())
		{
			lerr << "Can't find session in session map. call_id = " << iter->second << endl;
			delete p_trans_text;
			throw -1;
		}

		//Ͷ�ݸ���Ӧ��session����
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
