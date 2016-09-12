#ifndef DICT_PERSISTENT_H
#define DICT_PERSISTENT_H

#include "DictDef.h"

namespace dictionary
{

class DictPersistent
{
public:

	static int GetMaxDictWordID(DictID & dict_id, WordID & word_id);
	static int InsertWord(const DictID dict_id, const WordInfo & word_info);
	static int InsertWord(const DictID dict_id, const vector<WordInfo> & word_info_vec);
	static int DeleteWord(const WordID word_id);
	static int ModifyWord(const WordInfo & word_info);

	static int CreateDict(DictInfo & dict_info);
	static int ModifyDict(const DictInfo & dict_info);
	static int DeleteDict(const DictID dict_id);

	static int LoadAllDict(vector<DictInfo> & dict_info_vec);
	static int LoadAllWord(vector<WordInfo> & word_info_vec);
	static int LoadWord(const vector<WordID> & word_id_vec, vector<WordInfo> & word_info_vec); 
	static int ModifyImportStatus(const vector<WordID> & word_id_vec);

	static int downloadWords(const DictID dict_id, const vector<WordInfo> & word_info_vec);

	//·ÏÆú
	static int GetRecoverWordInfo(const WordID & word_id, WordInfo & word_info);
	static int RecoverWord(const WordID word_id);
};
}

#endif //DICT_PERSISTENT_H
