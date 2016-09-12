#ifndef USR_TEMPLATE_MANAGER_H
#define USR_TEMPLATE_MANAGER_H

#include "Common/RWLock.h"
#include "Common/BasicType.h"

#include "TemplateLib.h"
#include "Log/LogStream.h"

#include <string>
#include <map>
using namespace std;


class UsrTemplateLib
{
	friend class UsrTemplateLibManager;
public:
	UsrTemplateLib(const UsrID & usr_id) : m_usr_id(usr_id)
	{
	}

	int CreateTemplateLib(TemplateLibInfo & templatelib_info);
	int ModifyTemplateLib(const DomainType & old_domain_info, const TemplateLibInfo & templatelib_info);
	int DeleteTemplateLib(const DomainType & domain_info, const TemplateLibID templatelib_id);

	int InsertTemplate(const DomainType & domain_info, const TemplateLibID templatelib_id, TemplateInfo & template_info);
	int InsertTemplateGroup(const DomainType & domain_info, const TemplateLibID templatelib_id, vector<TemplateInfo> & template_info_vec);
	int DeleteTemplate(const DomainType & domain_info, const TemplateLibID templatelib_id, const TemplateID template_id);
	int ModifyTemplate(const DomainType & domain_info, const TemplateLibID templatelib_id, TemplateInfo & template_info);


	int MatchSentAppend(const DomainType & domain_info, const string & sent, vector<string> & result_vec);

	bool MountTemplateLib(TemplateLib * p_templatelib);

	void Print();

	int RecoverTemplate(const DomainType & domain_info, const TemplateLibID templatelib_id, const TemplateID template_id);
	int RecoverTemplateLib(TemplateLibInfo & templatelib_info);
private:
	string m_usr_id;

	map<DomainType, map<TemplateLibID, TemplateLib*> > m_templatelib_map;

	TemplateLib * find_templatelib(const DomainType & domain_info, const TemplateLibID templatelib_id);
};


class UsrTemplateLibManager
{
public:
	~UsrTemplateLibManager(void) {}

	static UsrTemplateLibManager * GetInstance();

	bool Initialize(void);

	int CreateTemplateLib(TemplateLibInfo & templatelib_info);
	int ModifyTemplateLib(const TemplateLibInfo & templatelib_info);
	int DeleteTemplateLib(const TemplateLibID templatelib_id);
	//
	int ToSysTemplateLib(const vector<int> & src_list, const TemplateLibID tgt_templatelib_id);

	int LoadTemplateFile(const TemplateLibID templatelib_id, const string & template_file_path, const int is_active = 1, const bool is_utf8 = true);
	int InsertTemplate(const TemplateLibID templatelib_id, TemplateInfo & template_info);
	int DeleteTemplate(const TemplateLibID templatelib_id, const TemplateID template_id);
	int ModifyTemplate(const TemplateLibID templatelib_id, TemplateInfo & template_info);

	int MatchSent(const UsrID & usrid, const DomainType & domain_info, const string & sent, vector<string> & result_vec);

	void PrintAllTemplateLib() ;
	int RecoverTemplateLib(const TemplateLibID & templatelib_id);
	int RecoverTemplate(const TemplateLibID & templatelib_id, const TemplateID & template_id);
private:
	UsrTemplateLibManager(void) {}

	static UsrTemplateLibManager ms_instance;

	RWLock m_rw_lock;

	map<UsrID, UsrTemplateLib*> m_usrtemplatelib_map;
	map<TemplateLibID, TemplateLibInfo> m_templatelib_info_map;

	TemplateLibInfo * find_templatelib_info(const TemplateLibID templatelib_id);
	UsrTemplateLib * find_usr_templatelib(const UsrID & usr_id);
	bool load_template_file(const string & file_path, vector<TemplateInfo> & template_info_vec, const bool is_utf8);
};

#endif //USR_TEMPLATE_MANAGER_H
