#ifndef DICT_DEF_H
#define DICT_DEF_H

//MAIN
#include "Common/BasicType.h"

#include <string>
#include <utility>
using namespace std;

#define SYSMANAGER "superadmin"

typedef size_t WordID;
typedef size_t DictID;
typedef int DictType;


#define DICT_TYPE_USR 0
#define DICT_TYPE_SYS 1
#define DICT_TYPE_PRO 2


class WordInfo
{
public:
	WordInfo(const int _is_deleted = 0, const int _is_active = 1, const int _is_checked = 1) : is_deleted(_is_deleted), is_active(_is_active), is_checked(_is_checked) {}

	WordID word_id;

	string src;
	string tgt;
	string src_key;

	int is_deleted; //是否已删除  1表示已经删除 持久层用
	int is_active; //是否启用  1表示启用

	int is_checked;  //是否已审核  1表示已经审核

	size_t dict_id;  //初始化的时候用
};

class DictInfo
{
public:

	DictInfo(const int _is_deleted = 0, const int _is_active = 1, const int _type = 0) : is_deleted(_is_deleted), is_active(_is_active), type(_type) {}

	DictID dict_id;
	DomainType domain_info;

	string dict_name;
	UsrID usr_id;

	DictType type; //词典类型 0 用户词典； 1 系统词典； 2 专业词典； （数据库字段对应is_system）

	int is_deleted; //是否已删除  1表示已经删除 持久层用
	int is_active; //是否启用  1表示启用

	string description;

};


#endif //DICT_DEF_H
