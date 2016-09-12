#include "AfterTreatPersistent.h"
#include "DataBase/DBOperation.h"


int AfterTreatPersistent::InsertWord(const AfterDictID dict_id, AfterWordInfo & word_info)
{
	return DBOperation::InsertAfterWord(dict_id, word_info);
}

int AfterTreatPersistent::InsertWordGroup(const AfterDictID dict_id, const vector<bool> & filter_vec, vector<AfterWordInfo> & word_info_vec)
{
	return DBOperation::InsertAfterWordGroup(dict_id, filter_vec, word_info_vec);
}


int AfterTreatPersistent::DeleteWord(const AfterWordID word_id)
{
	return DBOperation::DeleteAfterWord(word_id);
}

int AfterTreatPersistent::ModifyWord(const AfterWordInfo & word_info)
{
	return DBOperation::ModifyAfterWord(word_info);
}

int AfterTreatPersistent::CreateDict(AfterDictInfo & dict_info)
{
	return DBOperation::CreateAfterDict(dict_info);
}

int AfterTreatPersistent::ModifyDict(const AfterDictInfo & dict_info)
{
	return DBOperation::ModifyAfterDict(dict_info);
}

int AfterTreatPersistent::DeleteDict(const AfterDictInfo & dict_info)
{
	return DBOperation::DeleteAfterDict(dict_info);
}

int AfterTreatPersistent::LoadAllDict(vector<AfterDictInfo> & dict_info_vec)
{
	return DBOperation::LoadAllAfterDict(dict_info_vec);
}

int AfterTreatPersistent::LoadWord(const AfterDictInfo & dict_info, vector<AfterWordInfo> & word_info_vec)
{
	return DBOperation::LoadAfterWord(dict_info, word_info_vec);
}


///
int AfterTreatPersistent::RecoverWord(const AfterWordID word_id)
{
	return DBOperation::RecoverAfterWord(word_id);
}

int AfterTreatPersistent::GetRecoverWordInfo(const AfterWordID & word_id, AfterWordInfo & word_info)
{
	return DBOperation::GetRecoverAfterWordInfo(word_id, word_info);
}

