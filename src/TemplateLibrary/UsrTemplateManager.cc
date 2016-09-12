#include "UsrTemplateManager.h"
#include "Common/f_utility.h"
#include "Common/ICUConverter.h"
#include "LanguageTools/CLanguage.h"
#include "Common/ErrorCode.h"
#include "DataBase/DBOperation.h"
#include <fstream>

UsrTemplateLibManager UsrTemplateLibManager::ms_instance;

UsrTemplateLibManager * UsrTemplateLibManager::GetInstance()
{
	return &ms_instance;
}

bool UsrTemplateLibManager::Initialize(void)
{
	WriteLockGuard guard(m_rw_lock);

	if(false == TemplateLib::Initialize())
	{
		lerr << "TemplateLib init failed." << endl;
		return false;
	}

	vector<TemplateLib*> templatelib_vec;
	if(false == TemplateLib::LoadAll(templatelib_vec))
		return false;

	for(size_t i=0; i<templatelib_vec.size(); ++i)
	{

		TemplateLib * p_templatelib = templatelib_vec[i];
		map<UsrID, UsrTemplateLib*>::iterator iter = m_usrtemplatelib_map.find(p_templatelib->GetInfo().usr_id);

		if(m_usrtemplatelib_map.end() == iter)
		{
			iter = m_usrtemplatelib_map.insert(make_pair(p_templatelib->GetInfo().usr_id, new UsrTemplateLib(p_templatelib->GetInfo().usr_id))).first;
		}

		if(false == iter->second->MountTemplateLib(p_templatelib))
			TemplateLib::Delete(p_templatelib);
		else
			m_templatelib_info_map.insert(make_pair(p_templatelib->GetInfo().templatelib_id, p_templatelib->GetInfo()));
	}

	return true;
}

void UsrTemplateLib::Print()
{
	stringstream ss;

	size_t idx = 0;
	for(map<DomainType, map<TemplateLibID, TemplateLib*> >::iterator iter =  m_templatelib_map.begin();
		iter != m_templatelib_map.end(); ++iter)
	{
		ss << "           [" << idx++ << "]  domain : [" << iter->first.first << " : " << iter->first.second.first << " : " << iter->first.second.second << "]" << endl;

		for(map<TemplateLibID, TemplateLib*>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
		{
			ss << "                 templatelib_id = " << iter2->first << endl;
			iter2->second->Print();
		}
	}

	lout << "      Print Usr: " << m_usr_id << "'s Map: " << endl << ss.str() << endl;

}

void UsrTemplateLibManager::PrintAllTemplateLib() 
{
	WriteLockGuard guard(m_rw_lock);

	stringstream ss;


	size_t idx=0;
	for(map<TemplateLibID, TemplateLibInfo>::iterator iter = m_templatelib_info_map.begin(); iter != m_templatelib_info_map.end(); ++iter)
	{
		ss << "   [" << idx++ << "]  templatelib_id = " << iter->first << endl; 
	}

	lout << "Print TemplateLibID Map: " << endl << ss.str() << endl;


	idx = 0;

	lout << "Print UsrID Map: " << endl;
	for(map<UsrID, UsrTemplateLib*>::iterator iter = m_usrtemplatelib_map.begin(); iter != m_usrtemplatelib_map.end(); ++iter)
	{
		lout << "   [" << idx++ << "]  usr_id = " << iter->first << endl; 
		iter->second->Print();
	}


}

bool UsrTemplateLib::MountTemplateLib(TemplateLib * p_templatelib)
{
	if(NULL == p_templatelib)
	{
		return false;
	}

	map<TemplateLibID, TemplateLib*> & domain_templatelib_map = m_templatelib_map[p_templatelib->GetInfo().domain_info];

	map<TemplateLibID, TemplateLib*>::iterator iter = domain_templatelib_map.find(p_templatelib->GetInfo().templatelib_id);

	if(domain_templatelib_map.end() == iter)
	{
		domain_templatelib_map.insert(make_pair(p_templatelib->GetInfo().templatelib_id, p_templatelib));
	}else
	{
		lerr << "Same id in UsrTemplateLib: template lib id = " << p_templatelib->GetInfo().templatelib_id << endl;
		return false;
	}

	return true;

}

int UsrTemplateLibManager::CreateTemplateLib(TemplateLibInfo & templatelib_info)
{
	WriteLockGuard guard(m_rw_lock);

	map<UsrID, UsrTemplateLib*>::iterator miter = m_usrtemplatelib_map.find(templatelib_info.usr_id);

	if(m_usrtemplatelib_map.end() == miter)
	{
		miter = m_usrtemplatelib_map.insert(make_pair(templatelib_info.usr_id, new UsrTemplateLib(templatelib_info.usr_id))).first;
	}

	int result = miter->second->CreateTemplateLib(templatelib_info);

	m_templatelib_info_map.insert(make_pair(templatelib_info.templatelib_id, templatelib_info));

	return result;

}

int UsrTemplateLibManager::ModifyTemplateLib(const TemplateLibInfo & templatelib_info)
{
	WriteLockGuard guard(m_rw_lock);

	map<TemplateLibID, TemplateLibInfo>::iterator info_iter = m_templatelib_info_map.find(templatelib_info.templatelib_id);

	if(m_templatelib_info_map.end() == info_iter)
	{
		return ERR_TEMPLATELIB_NOT_FIND_IN_INFOMAP;
	}

	map<UsrID, UsrTemplateLib*>::iterator usr_iter = m_usrtemplatelib_map.find(info_iter->second.usr_id);

	if(m_usrtemplatelib_map.end() == usr_iter)
	{
		return ERR_TEMPLATELIB_NOT_FIND_IN_USRMAP;
	}

	int ret = usr_iter->second->ModifyTemplateLib(info_iter->second.domain_info, templatelib_info);
	//lout << "ret = " << ret << endl;

	if(SUCCESS == ret )
	{
		//lout << "rest info_iter" << endl;
		info_iter->second.description = templatelib_info.description;
		info_iter->second.templatelib_name = templatelib_info.templatelib_name;
		info_iter->second.domain_info = templatelib_info.domain_info;
		info_iter->second.is_active = templatelib_info.is_active;
		info_iter->second.is_deleted = 0;
		//
		info_iter->second.is_system = templatelib_info.is_system;
	}

	return ret;
}

int UsrTemplateLibManager::DeleteTemplateLib(const TemplateLibID templatelib_id)
{
	WriteLockGuard guard(m_rw_lock);

	map<TemplateLibID, TemplateLibInfo>::iterator info_iter = m_templatelib_info_map.find(templatelib_id);

	if(m_templatelib_info_map.end() == info_iter)
	{
		return ERR_TEMPLATELIB_NOT_FIND_IN_INFOMAP;
	}

	map<UsrID, UsrTemplateLib*>::iterator usr_iter = m_usrtemplatelib_map.find(info_iter->second.usr_id);

	if(m_usrtemplatelib_map.end() == usr_iter)
	{
		return ERR_TEMPLATELIB_NOT_FIND_IN_USRMAP;
	}

	int result = usr_iter->second->DeleteTemplateLib(info_iter->second.domain_info, templatelib_id);


	//lout << "DeleteTemplateLib  res = " <<  result << " templatelib_id = " << templatelib_id << endl;

	if(SUCCESS == result)
		m_templatelib_info_map.erase(info_iter);


	return result;

}


int UsrTemplateLibManager::LoadTemplateFile(const TemplateLibID templatelib_id, const string & template_file_path, const int is_active, bool is_utf8)
{
	WriteLockGuard guard(m_rw_lock);

	TemplateLibInfo * p_templatelib_info = find_templatelib_info(templatelib_id);

	if(NULL == p_templatelib_info)
		return ERR_TEMPLATELIB_NOT_FIND_IN_INFOMAP;


	UsrTemplateLib * p_usr_templatelib = find_usr_templatelib(p_templatelib_info->usr_id);
	
	vector<TemplateInfo> template_info_vec;
	if(NULL == p_usr_templatelib)
	{
		p_usr_templatelib = find_usr_templatelib(SYSMANAGER);

		if(NULL == p_usr_templatelib)
		{
			return ERR_TEMPLATELIB_NOT_FIND_IN_USRMAP;
		}
		if(false == load_template_file(template_file_path, template_info_vec, is_utf8))
			return ERR_TEMPLATELIB_LOAD_TEMPLATE_FILE_FAILED;

		for(size_t i=0; i<template_info_vec.size(); ++i)
		{
			template_info_vec[i].is_active = is_active;
			template_info_vec[i].is_checked = 0;
		}

		return p_usr_templatelib->InsertTemplateGroup(p_templatelib_info->domain_info, templatelib_id, template_info_vec);
	}

	//读取文件
	if(false == load_template_file(template_file_path, template_info_vec, is_utf8))
		return ERR_TEMPLATELIB_LOAD_TEMPLATE_FILE_FAILED;

	if(p_templatelib_info->usr_id == SYSMANAGER)
	{
		for(size_t i=0; i<template_info_vec.size(); ++i)
		{
			template_info_vec[i].is_active = is_active;
			template_info_vec[i].is_checked = 1;
		}
	}
	else
	{
		for(size_t i=0; i<template_info_vec.size(); ++i)
		{
			template_info_vec[i].is_active = is_active;
		}
	}

	return p_usr_templatelib->InsertTemplateGroup(p_templatelib_info->domain_info, templatelib_id, template_info_vec);
}

int UsrTemplateLibManager::InsertTemplate(const TemplateLibID templatelib_id, TemplateInfo & template_info)
{
	WriteLockGuard guard(m_rw_lock);

	//A2B
	CLanguage::Utf8A2B(template_info.src);
	CLanguage::Utf8A2B(template_info.tgt);

	TemplateLibInfo * p_templatelib_info = find_templatelib_info(templatelib_id);

	if(NULL == p_templatelib_info)
		return ERR_TEMPLATELIB_NOT_FIND_IN_INFOMAP;


	UsrTemplateLib * p_usr_templatelib = find_usr_templatelib(p_templatelib_info->usr_id);

	if(NULL == p_usr_templatelib)
	{
		p_usr_templatelib = find_usr_templatelib(SYSMANAGER);
		if(NULL == p_usr_templatelib)
		{
			return ERR_TEMPLATELIB_NOT_FIND_IN_USRMAP;
		}
		template_info.is_checked = 0;

		return p_usr_templatelib->InsertTemplate(p_templatelib_info->domain_info, templatelib_id, template_info);
	}

	template_info.is_checked = 1;

	return p_usr_templatelib->InsertTemplate(p_templatelib_info->domain_info, templatelib_id, template_info);

}

bool UsrTemplateLibManager::load_template_file(const string & file_path, vector<TemplateInfo> & template_info_vec, const bool is_utf8)
{
	template_info_vec.clear();

	ifstream file(file_path.c_str());

	if(false == file.good())
		return false;

	string line;
	size_t idx = 0;
	while(getline(file, line))
	{	
		idx++;
		vector<string> tmp_vec;
		split_string_by_tag(line.c_str(), tmp_vec, '\t');

		if(tmp_vec.size() < 2)
		{
			lwrn << "line = [" << line << "]" << endl;
			lwrn << "tmp_vec.size() = " << tmp_vec.size() << endl;
			lwrn << "tmp_vec[0] = [" << tmp_vec[0] << "]" << endl;
			lwrn << "Illegal line : " << idx << endl;
			continue;
		}

		TemplateInfo template_info;
		template_info.src = tmp_vec.at(0);
		template_info.tgt = tmp_vec.at(1);

        if(false == is_utf8)
        {
            ICUConverter::Convert("GB18030", "UTF-8", template_info.src);
            ICUConverter::Convert("GB18030", "UTF-8", template_info.tgt);
        }

		template_info_vec.push_back(template_info);
	}

	return true;
}

TemplateLibInfo * UsrTemplateLibManager::find_templatelib_info(const TemplateLibID templatelib_id)
{
	map<TemplateLibID, TemplateLibInfo>::iterator iter = m_templatelib_info_map.find(templatelib_id);

	if(m_templatelib_info_map.end() == iter)
		return NULL;

	return &(iter->second);
}

UsrTemplateLib * UsrTemplateLibManager::find_usr_templatelib(const UsrID & usr_id)
{
	map<UsrID, UsrTemplateLib*>::iterator iter = m_usrtemplatelib_map.find(usr_id);

	if(m_usrtemplatelib_map.end() == iter)
		return NULL;

	return iter->second;
}

int UsrTemplateLibManager::ModifyTemplate(const TemplateLibID templatelib_id, TemplateInfo & template_info)
{
	WriteLockGuard guard(m_rw_lock);

	TemplateLibInfo * p_templatelib_info = find_templatelib_info(templatelib_id);

	if(NULL == p_templatelib_info)
		return ERR_TEMPLATELIB_NOT_FIND_IN_INFOMAP;

	UsrTemplateLib * p_usr_templatelib = find_usr_templatelib(p_templatelib_info->usr_id);

	if(NULL == p_usr_templatelib)
		return ERR_TEMPLATELIB_NOT_FIND_IN_USRMAP;

	return p_usr_templatelib->ModifyTemplate(p_templatelib_info->domain_info, templatelib_id, template_info);
}

int UsrTemplateLibManager::DeleteTemplate(const TemplateLibID templatelib_id, const TemplateID template_id)
{


	//lout << "Delete template's lib id = " << templatelib_id << "   template_id = " << template_id << endl;

	WriteLockGuard guard(m_rw_lock);

	TemplateLibInfo * p_templatelib_info = find_templatelib_info(templatelib_id);

	if(NULL == p_templatelib_info)
		return ERR_TEMPLATELIB_NOT_FIND_IN_INFOMAP;

	UsrTemplateLib * p_usr_templatelib = find_usr_templatelib(p_templatelib_info->usr_id);

	if(NULL == p_usr_templatelib)
		return ERR_TEMPLATELIB_NOT_FIND_IN_USRMAP;

	//lout << "Find template lib id = " << p_templatelib_info->templatelib_id << endl;

	return p_usr_templatelib->DeleteTemplate(p_templatelib_info->domain_info, templatelib_id, template_id);
}

int UsrTemplateLibManager::MatchSent(const UsrID & usrid, const DomainType & domain_info, const string & sent, vector<string> & result_vec)
{

	WriteLockGuard guard(m_rw_lock); //无法确保iwslt模板是否线程安全，故用写锁


	DomainType all_domain = domain_info;

	all_domain.first = DOMAIN_ALL;

	//查询用户模板库
	map<UsrID, UsrTemplateLib*>::iterator miter = m_usrtemplatelib_map.find(usrid);

	if( miter != m_usrtemplatelib_map.end())
	{
		miter->second->MatchSentAppend(domain_info, sent, result_vec);
		
		if(all_domain != domain_info)
			miter->second->MatchSentAppend(all_domain, sent, result_vec);
	}

	//查询系统模板库
	map<UsrID, UsrTemplateLib*>::iterator sys_miter = m_usrtemplatelib_map.find(SYSMANAGER);

	if( sys_miter != m_usrtemplatelib_map.end())
	{
		sys_miter->second->MatchSentAppend(domain_info, sent, result_vec);

		if(all_domain != domain_info)
			sys_miter->second->MatchSentAppend(all_domain, sent, result_vec);
	}

	return SUCCESS;
}

int UsrTemplateLib::CreateTemplateLib(TemplateLibInfo & templatelib_info)
{
	map<TemplateLibID, TemplateLib*> & templatelib_group = m_templatelib_map[templatelib_info.domain_info];

	TemplateLib * p_templatelib = TemplateLib::New(templatelib_info);

	if(NULL == p_templatelib)
		return ERR_NEW_TEMPLATELIB;

	templatelib_group.insert(make_pair(templatelib_info.templatelib_id, p_templatelib));

	return SUCCESS;
}

int UsrTemplateLib::ModifyTemplateLib(const DomainType & old_domain_info, const TemplateLibInfo & templatelib_info)
{
	/*lout << "before modify templatelib  id = " << templatelib_info.templatelib_id << endl;

	lout << "old domain = " << old_domain_info.first << endl;
	lout << "old srcl = " << old_domain_info.second.first << endl;
	lout << "old tgtl = " << old_domain_info.second.second << endl;
	lout << "====================" << endl;
	lout << "new domain = " << templatelib_info.domain_info.first << endl;
	lout << "new srcl = " << templatelib_info.domain_info.second.first << endl;
	lout << "new tgtl = " << templatelib_info.domain_info.second.second << endl;*/

	map<DomainType, map<TemplateLibID, TemplateLib*> >::iterator iter = m_templatelib_map.find(old_domain_info);

	if(m_templatelib_map.end() == iter)
	{
		lerr << "ERR_TEMPLATELIBGROUP_NOT_FIND_IN_USRTEMPLATELIB" << endl;
		return ERR_TEMPLATELIBGROUP_NOT_FIND_IN_USRTEMPLATELIB;
	}

	map<TemplateLibID, TemplateLib*>::iterator iter2 = iter->second.find(templatelib_info.templatelib_id);

	if(iter->second.end() == iter2)
	{
		lerr << "ERR_TEMPLATELIB_NOT_FIND" << endl;
		return ERR_TEMPLATELIB_NOT_FIND;
	}

	TemplateLib * p_templatelib = iter2->second;

	if(templatelib_info.domain_info != old_domain_info)
	{
		//在原位置删除
		iter->second.erase(iter2);

		//挂接到新位置
		if(false == m_templatelib_map[templatelib_info.domain_info].insert(make_pair(templatelib_info.templatelib_id, p_templatelib)).second ) 
		{
			iter->second.insert(make_pair(templatelib_info.templatelib_id, p_templatelib)); //放回原位置
			lerr << "ERR_TEMPLATELIB_MODIFY_SAME_TEMPLATELIBID" << endl;
			return ERR_TEMPLATELIB_MODIFY_SAME_TEMPLATELIBID;
		}
	}

	return p_templatelib->ModifyTemplateLibInfo(templatelib_info);
}


int UsrTemplateLib::DeleteTemplateLib(const DomainType & domain_info, const TemplateLibID templatelib_id)
{
	map<DomainType, map<TemplateLibID, TemplateLib*> >::iterator iter = m_templatelib_map.find(domain_info);

	if(m_templatelib_map.end() == iter)
	{
		return ERR_TEMPLATELIBGROUP_NOT_FIND_IN_USRTEMPLATELIB;
	}

	map<TemplateLibID, TemplateLib*>::iterator iter2 = iter->second.find(templatelib_id);

	if(iter->second.end() != iter2)
	{
		TemplateLib::Destroy( iter2->second );
		iter->second.erase(iter2);
	}else
	{
		lerr << "Not find this templatelib in usrid = " << m_usr_id << " , domain_name = " << domain_info.first << " , srclanguage = " << domain_info.second.first << " , tgtlanguage = " << domain_info.second.second << " , templatelib_id = " << templatelib_id << endl;
	}

	return SUCCESS;
}


int UsrTemplateLib::InsertTemplate(const DomainType & domain_info, const TemplateLibID templatelib_id, TemplateInfo & template_info)
{
	TemplateLib * p_templatelib = find_templatelib(domain_info, templatelib_id);

	if(NULL == p_templatelib)
		return ERR_TEMPLATELIB_NOT_FIND;

	return p_templatelib->InsertTemplate(template_info);
}

int UsrTemplateLib::InsertTemplateGroup(const DomainType & domain_info, const TemplateLibID templatelib_id, vector<TemplateInfo> & template_info_vec)
{
	for(size_t i=0; i<template_info_vec.size(); ++i)
	{
		//A2B
		CLanguage::Utf8A2B(template_info_vec[i].src);
		CLanguage::Utf8A2B(template_info_vec[i].tgt);
	}


	TemplateLib * p_templatelib = find_templatelib(domain_info, templatelib_id);

	if(NULL == p_templatelib)
		return ERR_TEMPLATELIB_NOT_FIND;

	return p_templatelib->InsertTemplateGroup(template_info_vec);
}

int UsrTemplateLib::ModifyTemplate(const DomainType & domain_info, const TemplateLibID templatelib_id, TemplateInfo & template_info)
{
	TemplateLib * p_templatelib = find_templatelib(domain_info, templatelib_id);

	if(NULL == p_templatelib)
		return ERR_TEMPLATELIB_NOT_FIND;

	return p_templatelib->ModifyTemplate(template_info);
}

int UsrTemplateLib::DeleteTemplate(const DomainType & domain_info, const TemplateLibID templatelib_id, const TemplateID template_id)
{
	//lout << "delete template's lib id = " << templatelib_id << endl;
	TemplateLib * p_templatelib = find_templatelib(domain_info, templatelib_id);

	if(NULL == p_templatelib)
		return ERR_TEMPLATELIB_NOT_FIND;

	//lout << "find delete template's lib id = " << templatelib_id << endl;

	return p_templatelib->DeleteTemplate(template_id);

}

TemplateLib * UsrTemplateLib::find_templatelib(const DomainType & domain_info, const TemplateLibID templatelib_id)
{
	map<DomainType, map<TemplateLibID, TemplateLib*> >::iterator iter = m_templatelib_map.find(domain_info);

	if(m_templatelib_map.end() == iter)
	{
		lerr << "errinfo :" <<  ERR_TEMPLATELIBGROUP_NOT_FIND_IN_USRTEMPLATELIB << endl;
	}


	map<TemplateLibID, TemplateLib*>::iterator iter2 = iter->second.find(templatelib_id);

	if(iter->second.end() == iter2)
	{
		lerr << "errinfo :" <<   ERR_TEMPLATELIB_NOT_FIND_IN_USRTEMPLATELIB << endl;
	}

	return iter2->second;
}


int UsrTemplateLib::MatchSentAppend(const DomainType & domain_info, const string & sent, vector<string> & result_vec)
{
	map<DomainType, map<TemplateLibID, TemplateLib*> >::iterator iter = m_templatelib_map.find(domain_info);

	if(m_templatelib_map.end() == iter)
	{
		return ERR_TEMPLATELIBGROUP_NOT_FIND_IN_USRTEMPLATELIB;
	}

	map<TemplateLibID, TemplateLib*> & templatelib_group = iter->second;

	//match
	for(map<TemplateLibID, TemplateLib*>::iterator iter = templatelib_group.begin();
		iter != templatelib_group.end(); ++iter)
	{
		const TemplateLibInfo & lib_info = iter->second->GetInfo();
		lout << "Match lib: " << endl;
		lout << lib_info.templatelib_name << endl;
		lout << lib_info.is_active << endl;
		lout << lib_info.usr_id << endl;

		lout << endl;
		
		if(1 == iter->second->GetInfo().is_active)
			iter->second->MatchAppend(sent, result_vec);
		
	}

	return SUCCESS;
}


int UsrTemplateLibManager::ToSysTemplateLib(const vector<int> & src_list, const TemplateLibID tgt_templatelib_id)
{
	WriteLockGuard guard(m_rw_lock);

	vector<TemplateInfo> import_template_info;

	DBOperation::GetImportTemplateInfo(src_list, import_template_info);

	for(int i=0; i<import_template_info.size(); i++)
	{
		import_template_info[i].is_checked = 1;
	}

	TemplateLibInfo * templatelib_info_t = find_templatelib_info(tgt_templatelib_id);

	if(NULL == templatelib_info_t)
		return ERR_TEMPLATELIB_NOT_FIND_IN_INFOMAP;

	UsrTemplateLib * usr_templatelib_t = find_usr_templatelib(templatelib_info_t->usr_id);
	if(NULL == usr_templatelib_t)
	{
		return ERR_TEMPLATELIB_NOT_FIND_IN_USRMAP;
	}

	TemplateLib * p_templatelib_t = usr_templatelib_t->find_templatelib(templatelib_info_t->domain_info, tgt_templatelib_id);
	if(NULL == p_templatelib_t)
	{
		return ERR_TEMPLATELIB_NOT_FIND;
	}

	int tag = p_templatelib_t->InsertTemplateGroup(import_template_info);

	if(SUCCESS == tag)
	{
		return DBOperation::ModifyStatus_T(src_list);
	}

	return tag;
}



int UsrTemplateLibManager::RecoverTemplateLib(const TemplateLibID & templatelib_id)
{
	WriteLockGuard guard(m_rw_lock);

	DBOperation::RecoverTemplateLib(templatelib_id);

	TemplateLibInfo templatelib_info;
	vector<TemplateInfo> templatelib_template_info;

	DBOperation::GetRecoverTemplateLibInfo(templatelib_id, templatelib_info);

	map<UsrID, UsrTemplateLib*>::iterator miter = m_usrtemplatelib_map.find(templatelib_info.usr_id);

	if(m_usrtemplatelib_map.end() == miter)
	{
		miter = m_usrtemplatelib_map.insert(make_pair(templatelib_info.usr_id, new UsrTemplateLib(templatelib_info.usr_id))).first;
	}

	miter->second->RecoverTemplateLib(templatelib_info);

	m_templatelib_info_map.insert(make_pair(templatelib_info.templatelib_id, templatelib_info));


	DBOperation::GetRecoverTemplateLibTemplateInfo(templatelib_id, templatelib_template_info);


	TemplateLibInfo * templatelib_info_t = find_templatelib_info(templatelib_id);

	if(NULL == templatelib_info_t)
		return ERR_TEMPLATELIB_NOT_FIND_IN_INFOMAP;

	UsrTemplateLib * usr_templatelib_t = find_usr_templatelib(templatelib_info.usr_id);
	if(NULL == usr_templatelib_t)
	{
		return ERR_TEMPLATELIB_NOT_FIND_IN_USRMAP;
	}

	TemplateLib * p_templatelib_t = usr_templatelib_t->find_templatelib(templatelib_info.domain_info, templatelib_id);
	if(NULL == p_templatelib_t)
	{
		return ERR_TEMPLATELIB_NOT_FIND;
	}

	return p_templatelib_t->RecoverTemplateGroup(templatelib_template_info);
}

int UsrTemplateLibManager::RecoverTemplate(const TemplateLibID & templatelib_id, const TemplateID & template_id)
{
	WriteLockGuard guard(m_rw_lock);

	TemplateLibInfo * p_templatelib_info = find_templatelib_info(templatelib_id);

	if(NULL == p_templatelib_info)
		return ERR_TEMPLATELIB_NOT_FIND_IN_INFOMAP;

	UsrTemplateLib * p_usr_templatelib = find_usr_templatelib(p_templatelib_info->usr_id);

	if(NULL == p_usr_templatelib)
	{
		return ERR_TEMPLATELIB_NOT_FIND_IN_USRMAP;
	}

	return p_usr_templatelib->RecoverTemplate(p_templatelib_info->domain_info, templatelib_id, template_id);
}

int UsrTemplateLib::RecoverTemplate(const DomainType & domain_info, const TemplateLibID templatelib_id, const TemplateID template_id)
{
	TemplateLib * p_templatelib = find_templatelib(domain_info, templatelib_id);

	if(NULL == p_templatelib)
		return ERR_TEMPLATELIB_NOT_FIND;

	return p_templatelib->RecoverTemplate(template_id);
}

int UsrTemplateLib::RecoverTemplateLib(TemplateLibInfo & templatelib_info)
{
	map<TemplateLibID, TemplateLib*> & templatelib_group = m_templatelib_map[templatelib_info.domain_info];

	TemplateLib * p_templatelib = TemplateLib::Recover(templatelib_info);

	if(NULL == p_templatelib)
		return ERR_NEW_TEMPLATELIB;

	templatelib_group.insert(make_pair(templatelib_info.templatelib_id, p_templatelib));

	return SUCCESS;
}
