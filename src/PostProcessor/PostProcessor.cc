#include "PostProcessor.h"
#include "DataBase/DBOperation.h"

//BOOST
#include <boost/bind.hpp>

//MAIN
#include "PostProcessor/AfterTreatment/AfterTreatManager.h"


PostProcessor PostProcessor::ms_instance;

PostProcessor * PostProcessor::GetInstance()
{
	return &ms_instance;
}


PostProcessor::PostProcessor(void)
{
}

PostProcessor::~PostProcessor(void)
{
};


bool PostProcessor::Start(const size_t num)
{

	if( !EventEngine::Start(num) )
		return false;

	lout << " PreProcessor this_ptr = " << (unsigned long) this << " thread num = " << num << endl;
	return true;
}

void PostProcessor::on_event(Event & e)
{
	//lout<< "--> PostProcessor::on_event()." << endl;

	if(!e._p_edata)
	{
		lerr << "PostProcessor::on_event() e._data is null." << endl;
		return;
	}

	if(!e._p_caller)
	{
		lerr << "PostProcessor::on_event() e.caller is null." << endl;
		delete e._p_edata;
		return;
	}


	const EDType e_type = e._p_edata->GetType();
	EventData * p_edata_re = NULL;

	switch(e_type)
	{
	case EDTYPE_POSTPROC_REQ:
		p_edata_re = on_post_process(e._p_edata);
		break;
	default:
		lerr << "PostProcessor Can't handle this event , type = " << e_type << endl;
	}
	

	if(p_edata_re)
	{
		//返回结果
		Event re(p_edata_re, this);
		e._p_caller->PostEvent(re);
	}else
	{
		lerr << "PostProcessor handle faield. set p_edata_re is NULL." << endl;
	}
	
	//销毁Req
	delete e._p_edata;


}

EventData * PostProcessor::on_post_process(EventData * p_edata)
{

	lout << "on_post_process: " << endl;

	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//转换为PostProcReq
	PostProcReq * p_proc_req = dynamic_cast<PostProcReq*>(p_edata);

	if(!p_proc_req)
	{
		lerr << "Convert to PostProcReq failed." << endl;
		return NULL;
	}

	//生成res
	PostProcRes * p_proc_res = new PostProcRes(p_proc_req->GetCallID(), p_proc_req->GetTransText());
	//lout << "p_proc_res->GetCallID() = " << p_proc_res->GetCallID() << endl;

	if(!p_proc_res->GetTransText())
	{
		lerr << "PostProcessor::post_process() input PostProcRes's TransText is null." << endl;
		return NULL;
	}

	//开始预处理
	TransText & trans_text = *(p_proc_res->GetTransText());


	int proc_res = SUCCESS;

	if( false == post_process(trans_text) )
	{
		lerr << "PostProcRes Failed, text id = " << trans_text.GetID() << endl;
		proc_res = ERR_USR_POST_PROC;
	}

	//处理完毕
	p_proc_res->SetResult(proc_res);

	ldbg1 << "PostProcessor End proc_res = " << proc_res << ". textid = " << trans_text.GetID() << endl;

	return p_proc_res;

}

bool PostProcessor::post_process(TransText & trans_text)
{
	//提交OOV
	DBOperation::SubmitOOV(trans_text);

	return trans_text.PostProcess(boost::bind(&PostProcessor::post_process_handler, this,_1));
}

bool PostProcessor::post_process_handler(SentProcCell & sent_cell)
{

	//lout << "_decode_result = [" << sent_cell.trans_tgt._decode_result << "]" << endl;

	sent_cell.trans_tgt._assist_tgt = sent_cell.trans_tgt._decode_result;

	sent_cell.trans_tgt._tgt_str = AfterTreatManager::GetInstance()->Replace(sent_cell.trans_tgt._decode_result, sent_cell.domain, sent_cell.usr_id);

	//lout << "_tgt_str = [" << sent_cell.trans_tgt._tgt_str << "]" << endl;
	return true;
}
