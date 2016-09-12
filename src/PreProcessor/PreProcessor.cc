#include "PreProcessor.h"
#include "DataBase/DBOperation.h"
#include "LanguageProcess.h"

//BOOST
#include <boost/bind.hpp>


PreProcessor PreProcessor::ms_rtt_instance;
PreProcessor PreProcessor::ms_olt_instance;


bool PreProcessor::StartGroup(const size_t rtt_num, const size_t olt_num)
{
	if(!ms_rtt_instance.Start(rtt_num))
	{
		lerr << "Start rtt preprocess failed." << endl;
		return false;
	}

	if(!ms_olt_instance.Start(olt_num))
	{
		lerr << "Start rtt preprocess failed." << endl;
		return false;
	}
	
	return true;
}

PreProcessor::PreProcessor(void)
{
}

PreProcessor::~PreProcessor(void)
{
}

void PreProcessor::on_event(Event & e)
{
	//lout<< "--> PreProcessor::on_event()." << endl;

	if(!e._p_edata)
	{
		lerr << "PreProcessor::on_event() e._data is null." << endl;
		return;
	}

	if(!e._p_caller)
	{
		lerr << "PreProcessor::on_event() e.caller is null." << endl;
		delete e._p_edata;
		return;
	}


	const EDType e_type = e._p_edata->GetType();
	EventData * p_edata_re = NULL;

	switch(e_type)
	{
	case EDTYPE_PREPROC_REQ:
		p_edata_re = on_pre_process(e._p_edata);
		break;
	default:
		lerr << "PreProcessor can't handle this event , type = " << e_type << endl;
	}
	

	if(p_edata_re)
	{
		//返回结果
		Event re(p_edata_re, this);
		e._p_caller->PostEvent(re);
	}else
	{
		lerr << "PreProcessor handle faield. set p_edata_re is NULL." << endl;
	}
	
	//销毁Req
	delete e._p_edata;


}

EventData * PreProcessor::on_pre_process(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//转换为PreProcReq
	PreProcReq * p_proc_req = dynamic_cast<PreProcReq*>(p_edata);

	if(!p_proc_req)
	{
		lerr << "Convert to PreProcReq failed." << endl;
		return NULL;
	}

	//生成res
	PreProcRes * p_proc_res = new PreProcRes(p_proc_req->GetCallID(), p_proc_req->GetTransText());
	//lout << "p_proc_res->GetCallID() = " << p_proc_res->GetCallID() << endl;

	if(!p_proc_res->GetTransText())
	{
		lerr << "PreProcessor::pre_process() input PreProcRes's TransText is null." << endl;
		return NULL;
	}

	//开始预处理
	TransText & trans_text = *(p_proc_res->GetTransText());

	//ldbg1 << "PreProcess Start. textid = " << trans_text.GetID() << endl;

	int proc_res = SUCCESS;

	if( false == pre_process(trans_text) )
	{
		lerr << "PreProcess Failed, text id = " << trans_text.GetID() << endl;
		proc_res = ERR_USR_PRE_PROC;
	}

	//更新数据库中的领域 语言信息
	if(TRANS_TYPE_OLT == trans_text.GetTransType())
	{
		lout << "src = " << trans_text.GetSrcLanguage() << endl;
		lout << "tgt = " << trans_text.GetTgtLanguage() << endl;
		DBOperation::UpdateTransInfo(trans_text.GetID(), trans_text.GetDomainName(), trans_text.GetSrcLanguage(), trans_text.GetTgtLanguage() , trans_text.CountCharacter());
	}

#ifdef USR_CHARGING
    //权限获取或更新
    proc_res = DBOperation::GetPermission(trans_text.GetUsrID(), trans_text.CountCharacter());
#endif //USR_CHARGING

	//处理完毕
	p_proc_res->SetResult(proc_res);

	//ldbg1 << "PreProcess End proc_res = " << proc_res << ". textid = " << trans_text.GetID() << endl;

	return p_proc_res;

}

bool PreProcessor::pre_process(TransText & trans_text)
{
	//Step 1.识别语种
	if(LANGUAGE_UNKNOW == trans_text.GetSrcLanguage())
	{
		//为unknow的情况只对英汉或汉英作识别
		string src_language = CLanguage::IdentifyChEn(trans_text.GetSrcData());

		trans_text.SetSrcLanguage(src_language);

	}
	else if(LANGUAGE_UYGHUR == trans_text.GetSrcLanguage())
	{
		//识别是拉丁还是传统维文
		if(CLanguage::IsUyghurLatin(trans_text.GetSrcData()))
		{
			lout << "Get uy language : latin" << endl;
			trans_text.SetSubSrcLanguage(UYGHUR_LATIN);
		}else
		{
			lout << "Get uy language : trad" << endl;
			trans_text.SetSubSrcLanguage(UYGHUR_TRAD);
		}
			
	}
	
	
	//Step 2.文本预处理
	bool res = true;
	try{
#ifdef ALIGMENT_OUTPUT

	if(DOMAIN_MENU == trans_text.GetDomainName() && LANGUAGE_CHINESE == trans_text.GetSrcLanguage())
	{
		trans_text.SetTgtLanguage(LANGUAGE_ENGLISH);

		res = trans_text.PreProcess( boost::bind(&LanguageProcess::ChPreTextProcess, _1, _2),
									 boost::bind(&LanguageProcess::ChSpliteSent, _1, _2, _3),
									 boost::bind(&LanguageProcess::ChMenuPreSentProcess, _1) );

	}else if(LANGUAGE_CHINESE == trans_text.GetSrcLanguage())
	{
		trans_text.SetTgtLanguage(LANGUAGE_ENGLISH);

		res = trans_text.PreProcess( boost::bind(&LanguageProcess::ChPreTextProcess, _1, _2),
									 boost::bind(&LanguageProcess::ChSpliteSent, _1, _2, _3),
									 boost::bind(&LanguageProcess::ChPreSentProcess, _1) );


	}else if(LANGUAGE_ENGLISH == trans_text.GetSrcLanguage())
	{
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);

		res = trans_text.PreProcess( boost::bind(&LanguageProcess::EnPreTextProcess, _1, _2),
									 boost::bind(&LanguageProcess::EnSpliteSent, _1, _2, _3),
									 boost::bind(&LanguageProcess::EnPreSentProcess, _1) );

	}else if(LANGUAGE_UYGHUR == trans_text.GetSrcLanguage())
	{
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);

		if(trans_text.SubSrcLanguage() == UYGHUR_TRAD)
		{

			res = trans_text.PreProcess( boost::bind(&LanguageProcess::UyTradPreTextProcess, _1, _2),
										 boost::bind(&LanguageProcess::UySpliteSent, _1, _2, _3),
										 boost::bind(&LanguageProcess::UyPreSentProcess, _1) );
		}else
		{
			res = trans_text.PreProcess( boost::bind(&LanguageProcess::UyLatinPreTextProcess, _1, _2),
										 boost::bind(&LanguageProcess::UySpliteSent, _1, _2, _3),
										 boost::bind(&LanguageProcess::UyPreSentProcess, _1) );
		}

	}else if(LANGUAGE_TIBET == trans_text.GetSrcLanguage())
	{
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);

		res = trans_text.PreProcess( boost::bind(&LanguageProcess::TibetPreTextProcess, _1, _2),
									 boost::bind(&LanguageProcess::TibetSpliteSent, _1, _2, _3),
									 boost::bind(&LanguageProcess::TibetPreSentProcess, _1) );

	}else
	{
		lerr << "Not suport this language = " << trans_text.GetSrcLanguage() << endl;
		res = false;
	}


#else //ALIGMENT_OUTPUT
	if(DOMAIN_MENU == trans_text.GetDomainName() && LANGUAGE_CHINESE == trans_text.GetSrcLanguage())
	{ // Menu Chinese -> English preprocess
		trans_text.SetTgtLanguage(LANGUAGE_ENGLISH);

		res = trans_text.PreProcess( boost::bind(&LanguageProcess::ChPreTextProcess, _1),
									 boost::bind(&LanguageProcess::ChSpliteSent, _1, _2),
									 boost::bind(&LanguageProcess::ChMenuPreSentProcess, _1) );

	}else if(LANGUAGE_CHINESE == trans_text.GetSrcLanguage())
	{ // Chinese preprocess

		res = trans_text.PreProcess( boost::bind(&LanguageProcess::ChPreTextProcess, _1),
									 boost::bind(&LanguageProcess::ChSpliteSent, _1, _2),
									 boost::bind(&LanguageProcess::ChPreSentProcess, _1) );


	}else if(LANGUAGE_ENGLISH == trans_text.GetSrcLanguage())
	{ // English preprocess
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
		//trans_text.setFakeString("english to chinese");
		
		res = trans_text.PreProcess( boost::bind(&LanguageProcess::EnPreTextProcess, _1),
									 boost::bind(&LanguageProcess::EnSpliteSent, _1, _2),
									 boost::bind(&LanguageProcess::EnPreSentProcess, _1) );
									 

	}else if(LANGUAGE_UYGHUR == trans_text.GetSrcLanguage())
	{ // Uyghur preprocess
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
		//trans_text.setFakeString("uy to chinese");
		// for different type of Uyghur old(traditional)|latin
		if(trans_text.SubSrcLanguage() == UYGHUR_TRAD)
		{

			res = trans_text.PreProcess( boost::bind(&LanguageProcess::UyTradPreTextProcess, _1),
										 boost::bind(&LanguageProcess::UySpliteSent, _1, _2),
										 boost::bind(&LanguageProcess::UyPreSentProcess, _1) );
		}else
		{
			lout << "Debug: Uy Latin preprocess" <<  endl; // LK Debug
			res = trans_text.PreProcess( boost::bind(&LanguageProcess::UyLatinPreTextProcess, _1),
										 boost::bind(&LanguageProcess::UySpliteSent, _1, _2),
										 boost::bind(&LanguageProcess::UyPreSentProcess, _1) );
		}

	}else if(LANGUAGE_TIBET == trans_text.GetSrcLanguage())
	{ // Tibetan preprocess
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
		//trans_text.setFakeString("tibet to chinese");
		res = trans_text.PreProcess( boost::bind(&LanguageProcess::TibetPreTextProcess, _1),
									 boost::bind(&LanguageProcess::TibetSpliteSent, _1, _2),
									 boost::bind(&LanguageProcess::TibetPreSentProcess, _1) );
									 

	}else if(LANGUAGE_KOREAN == trans_text.GetSrcLanguage())
	{ // Korean preprocess
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
		//trans_text.setFakeString("korean to chinese");
		res = trans_text.PreProcess( boost::bind(&LanguageProcess::KoreanPreTextProcess, _1),
									 boost::bind(&LanguageProcess::KoreanSpliteSent, _1, _2),
									 boost::bind(&LanguageProcess::KoreanPreSentProcess, _1) );
									 

	}else if(LANGUAGE_MONGOLIAN == trans_text.GetSrcLanguage())
	{ // Mongolian preprocess 
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
		//trans_text.setFakeString("old mogolian to chinese");
		res = trans_text.PreProcess( boost::bind(&LanguageProcess::MongolianPreTextProcess, _1),
									 boost::bind(&LanguageProcess::MongolianSpliteSent, _1, _2),
									 boost::bind(&LanguageProcess::MongolianPreSentProcess, _1) );
									 

	}else if(LANGUAGE_NEWMONGOLIAN == trans_text.GetSrcLanguage())
	{ // New Cyril Mongolian (Outer Monglian)
		
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
		//trans_text.setFakeString("new mogolian to chinese");
		res = trans_text.PreProcess( boost::bind(&LanguageProcess::NEWMongolianPreTextProcess, _1),
									 boost::bind(&LanguageProcess::NEWMongolianSpliteSent, _1, _2),
									 boost::bind(&LanguageProcess::NEWMongolianPreSentProcess, _1) );
									 

	}else if(LANGUAGE_VIETNAM == trans_text.GetSrcLanguage())
	{ // Vietnam 
		// We only have Vietnam to Chinese translation so the target language should be Chinese
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
		//trans_text.setFakeString("vienam to chinese");
		res = trans_text.PreProcess( boost::bind(&LanguageProcess::VietnamPreTextProcess, _1),
									 boost::bind(&LanguageProcess::VietnamSpliteSent, _1, _2),
									 boost::bind(&LanguageProcess::VietnamPreSentProcess, _1) );
									 

	}else if(LANGUAGE_THAI == trans_text.GetSrcLanguage())
	{ // Thai, of course
		// We only have Thai to Chinese translation so the target language should be Chinese
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
		//trans_text.setFakeString("thai to chinese");
		res = trans_text.PreProcess( boost::bind(&LanguageProcess::ThaiPreTextProcess, _1),
									 boost::bind(&LanguageProcess::ThaiSpliteSent, _1, _2),
									 boost::bind(&LanguageProcess::ThaiPreSentProcess, _1) );
									 
									 

	}else if(LANGUAGE_RUSSIAN == trans_text.GetSrcLanguage())
	{ // the Russian
		// We only have Russian to Chinese translation so the target language should be Chinese
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE); 
		//trans_text.setFakeString("russian to chinese");
		res = trans_text.PreProcess( boost::bind(&LanguageProcess::RussianPreTextProcess, _1), // the Russian pre process
									 boost::bind(&LanguageProcess::UySpliteSent, _1, _2), // use uy sentence split method for Russian
									 boost::bind(&LanguageProcess::UyPreSentProcess, _1) ); // use uy pre sentence processs for Russian
	}else if(LANGUAGE_JAPANESE == trans_text.GetSrcLanguage())
	{
		trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
		//trans_text.setFakeString("japanese to chinese");
		res = trans_text.PreProcess( boost::bind(&LanguageProcess::JapanesePreTextProcess, _1), // the Russian pre process
									 boost::bind(&LanguageProcess::JapaneseSpliteSent, _1, _2), // use uy sentence split method for Russian
									 boost::bind(&LanguageProcess::JapanesePreSentProcess, _1) ); // use uy pre sentence processs for Russian
	}
	else
	{
		int split_status;
		if(SUCCESS == DBOperation::GetSplitStatus(trans_text.GetSrcLanguage(), split_status)){
			lout << "Language split_status " << split_status << endl;
			trans_text.SetTgtLanguage(LANGUAGE_CHINESE);
			//trans_text.setFakeString("other to chinese");
			if(SPLIT_TYPE_CHAR == split_status){//字切分
				res = trans_text.PreProcess( boost::bind(&LanguageProcess::OthPreTextProcess, _1),
					boost::bind(&LanguageProcess::OthCharSpliteSent, _1, _2),
					boost::bind(&LanguageProcess::OthCharPreSentProcess, _1) );
			}
			else if(SPLIT_TYPE_WORD == split_status){//词切分
				res = trans_text.PreProcess( boost::bind(&LanguageProcess::OthPreTextProcess, _1),
					boost::bind(&LanguageProcess::OthWordSpliteSent, _1, _2),
					boost::bind(&LanguageProcess::UyPreSentProcess, _1) );
			}
		}
		else{
			lerr << "Not suport this language = " << trans_text.GetSrcLanguage() << endl;
			res = false;
		}
	}

#endif //ALIGMENT_OUTPUT
	}
	catch(exception e){
		lerr << "Catch Error In PreProcessor :" << e.what() << endl;
		res = false;
	}
	return res;
}

void PreProcessor::PostRTTProcess(TransText * p_trans_text, const CallID cid, EventEngine * p_caller)
{
	assert(p_trans_text && p_caller);

	PreProcReq *p_proc_req = new PreProcReq(cid, p_trans_text);
	Event e(p_proc_req, p_caller);

	ms_rtt_instance.PostEvent(e);
}

void PreProcessor::PostOLTProcess(TransText * p_trans_text, const CallID cid, EventEngine * p_caller)
{
	assert(p_trans_text && p_caller);

	PreProcReq *p_proc_req = new PreProcReq(cid, p_trans_text);
	Event e(p_proc_req, p_caller);

	ms_olt_instance.PostEvent(e);
}
