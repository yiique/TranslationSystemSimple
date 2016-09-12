/**
* Copyright (c) 2009, 智能信息重点实验室,中国科学院计算技术研究所
* All rights reserved.
*
* 文件名：BasicType.h
* @brief  基础类型定义
* @project ToWin在线翻译平台
*
* @comment 
* @version 1.0
* @author 傅春霖
* @date 2010-03-24
* @
*/


#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <map>
using namespace std;
/*
	各种基本定义部分
	关键词：语种定义，领域定义，翻译定义，类型定义，任务定义，TransServer基本数据
*/
// for different lanuages
#define LANGUAGE_CHINESE "chinese"
#define LANGUAGE_ENGLISH "english"
#define LANGUAGE_UYGHUR "uyghur"
#define LANGUAGE_KOREAN "korean"
#define LANGUAGE_TIBET  "tibet"
#define LANGUAGE_THAI  "thai"
#define LANGUAGE_MONGOLIAN  "mongolian"
#define LANGUAGE_NEWMONGOLIAN "newmongolian"
#define LANGUAGE_VIETNAM "vietnam"
#define LANGUAGE_RUSSIAN "russian" // Definition of russian
#define LANGUAGE_JAPANESE "japanese"
#define LANGUAGE_UNKNOW "unknow"
// for domains
#define DOMAIN_UNKNOW "domain_unknow"
#define DOMAIN_NEWS "news"
#define DOMAIN_DOCUMENT "document"
#define DOMAIN_HXHG "hxhg"
#define DOMAIN_CTZY "ctzy"
#define DOMAIN_SPOKEN "spoken"
#define DOMAIN_MENU "menu"
#define DOMAIN_ALL "all"

#define DOWNLOAD_TYPE_NORMAL "normal"
#define DOWNLOAD_TYPE_TEXT "text"
#define DOWNLOAD_TYPE_TMX "tmx"
#define DOWNLOAD_TYPE_DETAIL "detail"

//for split type
#define SPLIT_TYPE_CHAR 0
#define SPLIT_TYPE_WORD 1


typedef enum
{
	BASE_SUB_TYPE,
	UYGHUR_LATIN,
	UYGHUR_TRAD,

	OTHER
} SubLanguageType;


//TransState
#define TRANS_STATE_SUBMITTING "SUBMITTING"
#define TRANS_STATE_RUNNING    "RUNNING"
#define TRANS_STATE_SUSPEND    "SUSPEND"
#define TRANS_STATE_CANCEL     "CANCEL"
#define TRANS_STATE_PERMISSION "NOMONEY"

//调式定义
#ifndef DEV_DEBUG
#define DEV_DEBUG
#endif





//===========================//
//  任务类型定义
//
typedef enum 
{
	TASK_TYPE_BASE,       //基类任务
	TASK_TYPE_TRANS,      //翻译任务
	TASK_TYPE_DOC,        //DOC类型翻译任务
	TASK_TYPE_HTML,       //HTML类型翻译任务
	TASK_TYPE_TXT         //TXT类型翻译任务
} TaskType;


//===========================//
//  任务ID定义
typedef int TaskID;


//===========================//
//  用户ID定义
typedef string UsrID;

//===========================//
//  领域定义(包含方向)
typedef pair<string, string> LangDirectionType;         //翻译语种方向   first(源语种)   second(目标语种)
typedef pair<string, LangDirectionType> DomainType;     //翻译领域       first(领域名称) second(语种方向)



//===========================//
//  翻译服务器地址定义
typedef pair<string, int> ServerAddrType;

//===========================//
//  翻译类型定义
const string TRANS_TYPE_RTT_STR = "TRANS_TYPE_RTT";
const string TRANS_TYPE_OLT_STR = "TRANS_TYPE_OLT";
const string TRANS_TYPE_PROC_STR = "TRANS_TYPE_PROC";

typedef enum 
{
	TRANS_TYPE_RTT,
	TRANS_TYPE_OLT,
	TRANS_TYPE_PROC
} TransType;

typedef struct
{
	ServerAddrType _server_addr;   //翻译服务器地址
	int _max_trans_num;   //最大同时翻译数
	string _cpu_use;      //cpu使用率
	string _mem_total;    //内存总数
	string _mem_use;      //内存使用数
	string _ser_uuid;
} TransServer;

//TextID
typedef string TextID;
typedef unsigned int  CellID;
typedef unsigned long  SentUniqID;

#endif //TYPE_H

