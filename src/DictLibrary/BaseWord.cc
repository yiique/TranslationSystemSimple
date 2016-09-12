#include "BaseWord.h"
#include "Dictionary.h"
#include "Log/LogStream.h"
#include <sstream>

using namespace dictionary;

vector<dict_ptr> BaseWord::CheckOwner(const UsrID & usrid, const DomainType & domain_info)
{
	vector<dict_ptr> sp_dict_vec;

	for(map<dict_ptr, UWord>::iterator iter = dict_map.begin(); iter != dict_map.end(); ++iter)
	{
		dict_ptr sp_dict = iter->first;

		/*lout << "usrid : " << usrid << endl;
		lout << "sp_dict->dict_info.usr_id : " << sp_dict->dict_info.usr_id << endl;

		lout << "sp_dict->dict_info.domain_info : " << sp_dict->dict_info.domain_info << endl;
		lout << "iter->second.is_checked : " << iter->second.is_checked << endl;
		lout << "iter->second.is_active : " << iter->second.is_active << endl;*/

		if( (sp_dict->dict_info.usr_id == usrid || sp_dict->dict_info.usr_id == SYSMANAGER )
			&& sp_dict->dict_info.domain_info.second == domain_info.second
			&& (sp_dict->dict_info.domain_info.first == DOMAIN_ALL || sp_dict->dict_info.domain_info.first == domain_info.first)
			&& iter->second.is_checked == 1
			&& iter->second.is_active == 1)
		{
			sp_dict_vec.push_back(sp_dict);
		}
	}

	return sp_dict_vec;

}

string BaseWord::DebugInfo(dict_ptr sp_dict) const
{
	stringstream ss;

	map<dict_ptr, UWord>::const_iterator iter = dict_map.find(sp_dict);

	if(iter != dict_map.end())
	{
		ss << "     (" << src << ") -> (" << tgt << ") , dictid(" << sp_dict->dict_info.dict_id << ") , wordid(" << iter->second.word_id << ") , active(" << iter->second.is_active << ") , check(" << iter->second.is_checked << ")" << endl;
	}else
	{
		ss << "     (" << src << ") -> (" << tgt << ") , dictid(" << "NULL" << ") , wordid(" << "NULL" << ") , active(" << "NULL" << ") , check(" << "NULL" << ")" << endl;
	}

	ss << endl;
	
	return ss.str();
}
