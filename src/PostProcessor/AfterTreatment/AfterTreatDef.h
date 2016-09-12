#ifndef AFTERTREAT_DEF_H
#define AFTERTREAT_DEF_H

#include "Common/BasicType.h"
#include "Common/ErrorCode.h"

typedef int AfterDictID;
typedef int AfterWordID;

#define SYSMANAGER "superadmin"

class AfterWordInfo
{
public:
	AfterWordInfo(const int _is_deleted = 0, const int _is_active = 1) : is_deleted(_is_deleted), is_active(_is_active) {}
	AfterWordID word_id;

	string src;
	string tgt;
	string src_key;

	int is_deleted; //是否已删除  1表示已经删除 持久层用
	int is_active; //是否启用  1表示启用
};

class AfterDictInfo
{
public:
	AfterDictInfo(const int _is_deleted = 0, const int _is_active = 1) : is_deleted(_is_deleted), is_active(_is_active) {}

	AfterDictID dict_id;
	DomainType domain_info;

	string dict_name;
	UsrID usr_id;

	string description;

	int is_deleted; //是否已删除  1表示已经删除 持久层用
	int is_active; //是否启用  1表示启用
} ;


#endif 

