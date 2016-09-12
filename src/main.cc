
#include "SessionManager/UsrSessManager.h"
#include "SessionManager/SerSessManager.h"
#include "PreProcessor/PreProcessor.h"
#include "TransController/TransController.h"
#include "LanguageTools/CLanguage.h"
#include "Log/LogPrinter.h"
#include "Log/LogStream.h"
#include "Timer/CTimer.h"
#include "Configurator/Configurator.h"
#include "NetDriver/NetInterface.h"
#include "TransText/LTransTextProcessor.h"
#include "TransText/TransText.h"
#include "OltManager/OltTaskManager.h"
#include "DictLibrary/DictProcessor.h"
#include "DictLibrary/DictManager.h"
#include "TemplateLibrary/TemplateProcessor.h"
#include "DataBase/DBConnPool.h"
#include "SipRPC/SipRPC.h"

#ifdef ENABLE_SOFT_PROTECT
#include "SoftProtect/SoftProtectHandle.h"
#endif 

#include "PostProcessor/PostProcessor.h"
#include "GeneralOperation/GeneralOperation.h"

//LINUX
#include <iostream>
#include <sstream>
using namespace std;

#ifdef ENABLE_SOFT_PROTECT
bool check();					//check the permission
void reset();					//reset the program because of registeration error
#endif

int main(int argc, char *argv[])
{
#ifdef ENABLE_SOFT_PROTECT
	if(false == check())
		return -1;
#endif

	try
	{

	//先启动日志
    cout << "kao" << endl;
	if(!LogPrinter::GetInstance()->Start())
	{
		cout << "LogPrinter init failed." << endl;
		throw -1;
	}

	cout << "log success" << endl;
	
	//读取配置文件
	Configurator config;

	string config_path;

	if(argc > 1)
		config_path = argv[1];
	else
		config_path = "server_config.txt";

	if( !config.ReadConfig(config_path.c_str()) )
	{
		lerr << "Read Config Failed. config path = " << config_path << endl;
		throw -1;
	}

	lout << "Read Config Success." << endl;
	
	//初始化siprpc
	if(false == siprpc::SipRPC::Initialize())
	{
		lerr << "Init sip rpc failed." << endl;
		throw -1;
	}

	lout << "Init sip rpc Success." << endl;
	
	//初始化网络
	lout << "Initialize NetInterface..." << endl;
	if( !NetInterface::Initialize() )
	{
		lerr << "Initialize NetInterface Failed." << endl;
		throw -1;
	}

	lout << "Initialize NetInterface Success." << endl;
	
	//Init Data Base .
	lout << "DBConnPool Start..." << endl;
	if( !DBConnPool::GetInstance()->Initialize(config._db_name, config._db_ip, config._db_usr, config._db_pwd, config._db_port))
	{
		lerr<< "DBConnPool Start failed." << endl;
		throw -1;
	}
	lout << "DBConnPool Start Success." << endl;
	
	//初始化TransText 句子id
	long max_sent_id = 0;
	if(SUCCESS != DBOperation::GetMaxSentID(max_sent_id))
	{
		lerr<< "Get Max Sent id from database failed." << endl;
		throw -1;
	}

	TransText::ResetBaseSentID(max_sent_id);

	lout<< "Get Max Sent id from database Success. Max id is : " << max_sent_id << endl;
	
	//LanguageTools static resource 尽量靠前，其他模块会调用
	lout << "LanguageTools Init..." << endl;
	if( !CLanguage::InitStaticResource(config))
	{
		lerr<< "LanguageTools Init Failed." << endl;
		throw -1;
	}
	lout << "LTransTextProcessor Init Success." << endl;
	
	//DictManager
	lout << "DictManager Init..." << endl;
	if( !dictionary::DictManager::GetInstance()->Initialize())
	{
		lerr<< "DictManager Init failed." << endl;
		throw -1;
	}
	lout << "DictManager Init Success." << endl;

	//UsrDictManager::GetInstance()->PrintAllDict();

	//UsrTemplateLibManager
	lout << "TemplateLibManager Init..." << endl;
	if( !UsrTemplateLibManager::GetInstance()->Initialize())
	{
	    lerr << "UsrTemplateLibManager Init failed." << endl;
		throw -1;
	}
	lout << "UsrTemplateLibManager Init Success." << endl;
	
	//TemplateLibProcessor
	lout << "TemplateLibProcessor Start..." << endl;
	if( !TemplateLibProcessor::GetInstance()->Start(config._text_result_path))
	{
	    lerr << "TemplateLibProcessor Start failed." << endl;
		throw -1;
	}
	lout << "TemplateLibProcessor Start Success." << endl;
	
	//AfterTreatManager
	lout << "AfterTreatManager Start..." << endl;
	if( !AfterTreatManager::GetInstance()->Initialize(config._text_result_path))
	{
	    lerr << "AfterTreatManager Start failed." << endl;
		throw -1;
	}
	lout << "AfterTreatManager Start Success." << endl;

	//DictProcessor
	lout << "DictProcessor Start..." << endl;
	if( !DictProcessor::GetInstance()->Start(config._text_result_path))
	{
		lerr<< "DictProcessor Start failed." << endl;
		throw -1;
	}
	lout << "DictProcessor Start Success." << endl;

	//PreProcessor
	lout << "PreProcessor Start..." << endl;
	if( !PreProcessor::StartGroup(config._pre_rtt_threads_num, config._pre_olt_threads_num))
	{
		lerr<< "PreProcessor Start failed." << endl;
		throw -1;
	}
	lout << "PreProcessor Start Success." << endl;

	//PostProcessor
	lout << "PostProcessor Start..." << endl;
	if( !PostProcessor::GetInstance()->Start(config._post_threads_num))
	{
		lerr<< "PostProcessor Start failed." << endl;
		throw -1;
	}
	lout << "PostProcessor Start Success." << endl;
	
	//GeneralOperation
	lout << "GeneralOperation Start..." << endl;
	if( !GeneralOperation::GetInstance()->Start(config._general_threads_num))
	{
		lerr << "GeneralOperation Start failed." << endl;
		throw -1;
	}
	lout << "GeneralOperation Start Success." << endl;

	//OltTaskManager
	lout << "OltTaskManager Start..." << endl;
	if( !OltTaskManager::GetInstance()->Start())
	{
		lerr << "OltTaskManager Start failed." << endl;
		throw -1;
	}
	lout << "OltTaskManager Start Success." << endl;

	//TextHandler
	lout << "TextHandler Start..." << endl;
	if( !TextHandler::GetInstance()->Start(config._text_threads_num, config._text_result_path) )
	{
		lerr << "TextHandler Start failed." << endl;
		throw -1;
	}
	lout << "TextHandler Start Success." << endl;
	

	//transcontroller
	lout << "TransController RTT Start..." << endl;
	if( !TransController::GetRttInstance()->Start(config) )
	{
		lerr<< "Rtt TransController Start Failed." << endl;
		throw -1;
	}
	lout << "Rtt TransController Start Success." << endl;

	//transcontroller
	lout << "TransController OLT Start..." << endl;
	if( !TransController::GetOltInstance()->Start(config) )
	{
		lerr<< "Olt TransController Start Failed." << endl;
		throw -1;
	}
	lout << "Olt TransController Start Success." << endl;

	//LTransTextProcessor
	lout << "LTransTextProcessor Start..." << endl;
	if( !LTransTextProcessor::GetInstance()->Start() )
	{
		lerr<< "LTransTextProcessor Start Failed." << endl;
		throw -1;
	}
	lout << "LTransTextProcessor Start Success." << endl;
	

	//session 管理器
	cout << "ip:" << config._local_ip.c_str() << endl;
	cout << "port1:" << config._local_usr_port << endl;
	cout << "port2:" << config._local_ser_port << endl;
	UsrSessManager::GetInstance()->SetListen(config._local_ip.c_str(), config._local_usr_port);
	SerSessManager::GetInstance()->SetListen(config._local_ip.c_str(), config._local_ser_port);

	lout << "UsrSessionManager Start..." << endl;
	if( !UsrSessManager::GetInstance()->Start())
	{
		lerr<< "UsrSessionManager Start Failed." << endl;
		throw -1;
	}

	lout << "UsrSessionManager Start Success." << endl;

	lout << "SerSessionManager Start..." << endl;
	if( !SerSessManager::GetInstance()->Start())
	{
		lerr<< "SerSessionManager Start Failed." << endl;
		throw -1;
	}

	lout << "SerSessionManager Start Success." << endl;

	//CTimer
	CTimer::GetInstance()->RegTimerBeforeStart(TransController::GetRttInstance(), 30);
	CTimer::GetInstance()->RegTimerBeforeStart(TransController::GetOltInstance(), 30);

	if( !CTimer::GetInstance()->Start())
	{
		lerr<< "CTimer Start Failed." << endl;
		throw -1;
	}

	lout << "CTimer Start Success." << endl;
	
	
	lout << "All Start Success." << endl;

	while(true)
	{
		boost::this_thread::sleep(boost::posix_time::seconds(100000));
	}
	

	}catch(exception e)
	{
		cerr << "System exception error: " << e.what() << endl;
		
	}catch(int e)
	{
		cerr << "Master init failed." << endl;
		
	}

	int x;
	cin >> x;
	return 0;
}

#ifdef ENABLE_SOFT_PROTECT

bool check()
{
	softprotect::SoftProtectHandle::SetUpdateBufSize(10);

	int ret = softprotect::SoftProtectHandle::CheckHost();
	if(0 != ret)
	{
		cerr << "The Programe cannot running on this host without permission. ErrCode = " << ret << endl;
		reset();

		return false;
	}

	ret = softprotect::SoftProtectHandle::CheckUseCnt();

	if(0 != ret)
	{
		cerr << "The Programe's using count is full. ErrCode = " << ret << endl;
		reset();
		return false;
	}

	cout << "Soft Check Success." << endl;

	return true;

}

void reset()
{
	cout << "Is recreate the register info (yes/no) ? :";

	string cmd;
	cin >> cmd;

	if(cmd == "yes")
	{
		int ret = softprotect::SoftProtectHandle::Reset();

		if(0 != ret)
			cerr << "Recreate failed. ErrCode = " << ret << endl;
		else
			cout << "Recreate success. Please restart the programe." << endl;
	}else
	{
		cout << "Your input is no(" << cmd << "), The Programe will be showdown. " << endl;
	}

}

#endif //ENABLE_SOFT_PROTECT
