#ifndef TEMPLATE_DEF_H
#define TEMPLATE_DEF_H

#include "Common/BasicType.h"
#include <string>
using namespace std;

#define SYSMANAGER "superadmin"
typedef int TemplateLibID;
typedef int TemplateID;

class TemplateInfo
{
public:
	TemplateInfo(const int _is_checked = 0, const int _is_deleted = 0, const int _is_active = 1): is_checked(_is_checked),is_active(_is_active),is_deleted(_is_deleted){}
	TemplateID template_id;

	string src;
	string tgt;

	int is_deleted; //是否已删除  1表示已经删除 持久层用
	int is_active; //是否启用  1表示启用
	int is_checked;
};

class TemplateLibInfo
{
public:
	TemplateLibInfo(const int _is_deleted = 0, const int _is_active = 1, const int _is_system = 0):is_deleted(_is_deleted),is_active(_is_active),is_system(_is_system){}
	TemplateLibID templatelib_id;
	DomainType domain_info;

	string templatelib_name;
	UsrID usr_id;

	int is_deleted; //是否已删除  1表示已经删除 持久层用
	int is_active; //是否启用  1表示启用
	int is_system;

	string description;

	bool operator == ( const TemplateLibInfo & rsh ) const
	{
		if(templatelib_id == rsh.templatelib_id
			&& domain_info == rsh.domain_info
			&& templatelib_name == rsh.templatelib_name
			&& usr_id == rsh.usr_id
			&& is_deleted == rsh.is_deleted
			&& is_active == rsh.is_active
			&& description == rsh.description)
			return true;

		return false;
	}

} ;


#endif //TEMPLATE_DEF_H
