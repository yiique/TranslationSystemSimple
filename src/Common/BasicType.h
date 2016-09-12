/**
* Copyright (c) 2009, ������Ϣ�ص�ʵ����,�й���ѧԺ���㼼���о���
* All rights reserved.
*
* �ļ�����BasicType.h
* @brief  �������Ͷ���
* @project ToWin���߷���ƽ̨
*
* @comment 
* @version 1.0
* @author ������
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
	���ֻ������岿��
	�ؼ��ʣ����ֶ��壬�����壬���붨�壬���Ͷ��壬�����壬TransServer��������
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

//��ʽ����
#ifndef DEV_DEBUG
#define DEV_DEBUG
#endif





//===========================//
//  �������Ͷ���
//
typedef enum 
{
	TASK_TYPE_BASE,       //��������
	TASK_TYPE_TRANS,      //��������
	TASK_TYPE_DOC,        //DOC���ͷ�������
	TASK_TYPE_HTML,       //HTML���ͷ�������
	TASK_TYPE_TXT         //TXT���ͷ�������
} TaskType;


//===========================//
//  ����ID����
typedef int TaskID;


//===========================//
//  �û�ID����
typedef string UsrID;

//===========================//
//  ������(��������)
typedef pair<string, string> LangDirectionType;         //�������ַ���   first(Դ����)   second(Ŀ������)
typedef pair<string, LangDirectionType> DomainType;     //��������       first(��������) second(���ַ���)



//===========================//
//  �����������ַ����
typedef pair<string, int> ServerAddrType;

//===========================//
//  �������Ͷ���
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
	ServerAddrType _server_addr;   //�����������ַ
	int _max_trans_num;   //���ͬʱ������
	string _cpu_use;      //cpuʹ����
	string _mem_total;    //�ڴ�����
	string _mem_use;      //�ڴ�ʹ����
	string _ser_uuid;
} TransServer;

//TextID
typedef string TextID;
typedef unsigned int  CellID;
typedef unsigned long  SentUniqID;

#endif //TYPE_H

