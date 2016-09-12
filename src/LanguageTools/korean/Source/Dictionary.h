#ifndef  _DICTIONARY_H_
#define  _DICTIONARY_H_

#include "ArrayTrie.h"

typedef  struct
{
	int tag_num;
	int tags[10];
	char index_str[10];
	char morph_str[10][100];
}PRE_INFO;

typedef  struct
{
	int tag_id;
	char verb_adj;
}TAG_INFO;

class MorphRec 
{
public:
	char key[100];
	MorphRec();
	int tagInfo_num;
	TAG_INFO tagInfo[15];
	int preInfo_num;
	PRE_INFO preInfo[20];
	int noun_freq;
	bool is_compound;
	bool fromString(char *buff, bool kind);
	int  serialize(char *buff);
	void load(char *buff);
};

class MorphDic
{
	ArrayTrie trie;
	FILE *m_fp;
public:
	MorphDic();
	~MorphDic();
	bool create(char *fname);
	bool search(char *key, MorphRec *res, bool is_josa=false);
	int  search_all(char *str, int *offsets);
};

class ProbDic
{
	ArrayTrie trie;
	FILE *m_fp;
public:
	ProbDic();
	~ProbDic();
	bool create(char *fname);
	bool search(char *key, float *val);
};

class XrDic
{
	ArrayTrie trie_xr;
	FILE *m_fp;
public:
	XrDic();
	~XrDic();
	bool create(char *fname);
	bool search(char *key, char *tail_str);
	int search_all(char *key, int *offset);
};

#endif
 
