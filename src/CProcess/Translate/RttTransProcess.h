#ifndef RTT_TRANS_PROCESS_H
#define RTT_TRANS_PROCESS_H

//MAIN
#include "CProcess/CProcess.h"
#include "TransText/TransText.h"
#include "TransController/TransControlEvent.h"
#include "TransController/TransController.h"
#include "Common/MsgType.h"
#include "TinyXml/tinyxml.h"
#include "LanguageTools/CLanguage.h"
#include "Common/f_utility.h"
#include "PreProcessor/PreProcessor.h"
#include "TransText/LTransTextProcessor.h"
#include "Configurator/Configurator.h"
#include "PostProcessor/PostProcessor.h"


typedef enum
{
	RTT_PROC_REMOTE_PROC, //0
	RTT_PROC_PREPROC,   //1
	RTT_PROC_TOQUEUE,    //2
	RTT_PROC_TRANS,      //3
	RTT_PROC_POSTPROC,  //4
	RTT_PROC_END        //5
} RttProcState;

class RttTransProcess
	: public CProcess
{
public:
	RttTransProcess(EventEngine * p_owner, 
					const CallID & cid,
					const TextID & tid,   //用于即时翻译的文件ID
					NetPacket & inpacket, 
					NetPacket & outpacket);

	~RttTransProcess(void);

	//重写父类Begin方法
	ProcessRes Begin();

	//重写基类GetType方法
	ProcessType GetType() const;

	//重写父类Work方法
	ProcessRes Work(EventData * p_edata);

	const TextID & GetRttTextID() const
	{
		return m_rtt_tid;
	}

private:
	bool parse_packet();
	bool package_packet(TransText * p_text);
	
	ProcessRes on_txt_proc_end(EventData * p_edata);
	ProcessRes on_preprocess_end(EventData * p_edata);
	ProcessRes on_trans_end(EventData * p_edata);
	ProcessRes on_postprocess_end(EventData * p_edata);

	RttProcState m_rtt_proc_state;

	const TextID m_rtt_tid; //用于即时翻译的文件ID 

	UsrID m_usr_id;
	string m_domain_name;
	string m_src_language;
	string m_tgt_language;
	string m_trans_src;

	bool m_need_align;
	bool m_need_detail;
	bool m_need_sentpair;

};


#endif //RTT_TRANS_PROCESS_H

