#ifndef AFTERTREAT_PERSISTENT_H
#define AFTERTREAT_PERSISTENT_H

#include "AfterTreatDef.h"

class AfterTreatPersistent
{
public:
	static int InsertWord(const AfterDictID dict_id, AfterWordInfo & word_info);
	static int InsertWordGroup(const AfterDictID dict_id, const vector<bool> & filter_vec, vector<AfterWordInfo> & word_info_vec);
	static int DeleteWord(const AfterWordID word_id);
	static int ModifyWord(const AfterWordInfo & word_info);

	static int CreateDict(AfterDictInfo & dict_info);
	static int ModifyDict(const AfterDictInfo & dict_info);
	static int DeleteDict(const AfterDictInfo & dict_info);

	
	static int LoadAllDict(vector<AfterDictInfo> & dict_info_vec);
	static int LoadWord(const AfterDictInfo & dict_info, vector<AfterWordInfo> & word_info_vec);

	//
	static int GetRecoverWordInfo(const AfterWordID & word_id, AfterWordInfo & word_info);
	static int RecoverWord(const AfterWordID word_id);
};

#endif
