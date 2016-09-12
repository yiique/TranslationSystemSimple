#include "TemplatePersistent.h"
#include "DataBase/DBOperation.h"


int TemplatePersistent::InsertTemplate(const TemplateLibID templatelib_id, TemplateInfo & template_info)
{
	return DBOperation::InsertTemplate(templatelib_id, template_info);
}

int TemplatePersistent::InsertTemplateGroup(const TemplateLibID templatelib_id, const vector<bool> & filter_vec, vector<TemplateInfo> & template_info_vec)
{
	return DBOperation::InsertTemplateGroup(templatelib_id, filter_vec, template_info_vec);
}


int TemplatePersistent::DeleteTemplate(const TemplateID template_id)
{
	return DBOperation::DeleteTemplate(template_id);
}

int TemplatePersistent::ModifyTemplate(const TemplateInfo & template_info)
{
	return DBOperation::ModifyTemplate(template_info);
}

int TemplatePersistent::CreateTemplateLib(TemplateLibInfo & templatelib_info)
{
	return DBOperation::CreateTemplateLib(templatelib_info);
}

int TemplatePersistent::ModifyTemplateLib(const TemplateLibInfo & templatelib_info)
{
	return DBOperation::ModifyTemplateLib(templatelib_info);
}

int TemplatePersistent::DeleteTemplateLib(const TemplateLibInfo & templatelib_info)
{
	return DBOperation::DeleteTemplateLib(templatelib_info);
}

int TemplatePersistent::LoadAllTemplateLib(vector<TemplateLibInfo> & templatelib_info_vec)
{
	return DBOperation::LoadAllTemplateLib(templatelib_info_vec);
}

int TemplatePersistent::LoadTemplate(const TemplateLibInfo & templatelib_info, vector<TemplateInfo> & template_info_vec)
{
	return DBOperation::LoadTemplate(templatelib_info, template_info_vec);
}



int TemplatePersistent::RecoverTemplate(const TemplateID & template_id)
{
	return DBOperation::RecoverTemplate(template_id);
}

int TemplatePersistent::GetRecoverTemplateInfo(const TemplateID & template_id, TemplateInfo & template_info)
{
	return DBOperation::GetRecoverTemplateInfo(template_id, template_info);
}

int TemplatePersistent::GetMaxTemplateID(TemplateLibID & templatelib_id, TemplateID & template_id)
{
	return DBOperation::GetMaxTemplateID(templatelib_id, template_id);
}
