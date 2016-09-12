#if !defined(_ARRAY_TRIE_H_)
#define  _ARRAY_TRIE_H_
#include "Stdafx_engine.h"

typedef struct
{
	int base;
	int check;
}ELEM;

typedef struct
{
	char tail[500];
	int  data;
}TAIL;

typedef struct
{
	char version[10];
	char  extra[10];
	int  fir_empty;
	int  count;
	int  key_count;
}HEAD;

#define SETSIZE  256

class ArrayTrie
{
	FILE *m_fpTrie;
	FILE *m_fpTail;

#ifdef _MTDLL
	char *m_pTrie;
	char *m_pTail;
	int m_lenTailFile;
#endif

	int m_mode;
	ELEM m_pElem[SETSIZE];	
	void makeEmpty(ELEM *pElem, int count);
	int  sub_search(const char **str, int *base_index, bool *exist, const char **pp = NULL, int * curLen = NULL, int *data_poin= NULL);
	bool substring_search(const char **po, int *base_index, int *check_index);
	void sub_insert(const char *str, int data, int base_index, bool &exist);
	int  indexof(char c);
	TAIL tailof(int pos, int *data_poin = NULL);
	ELEM elemof(int index);
	void modify_elem(int index, ELEM &elem);
	int  add_tail(const char *str, int data, int poin = 0);
	int  getNewSet(int *pindex, int count);
	int  getAll(int base, int check_val,int *pn);
	void modify_empty(int cur_emp, int past_emp, int *pindex, int count);
	bool is_available(int cur_emp, int *pindex, int count);
	void cancel_empty(int index);
	void add_empty(int index);
	bool getOneSet(int startElemNum, ELEM *res);
	bool putOneSet(int startElemNum, ELEM *res);
	bool is_empty_set(int first_index, int base_index);
	bool sub_remove(const char *str, int base_index);
	void write_head();

	char **m_words;
	int  m_words_count;
	int  m_words_index;

public:

	HEAD m_head;
	
	ArrayTrie();
	bool create(char *fname, int mode, bool is_db = false);
	bool create(char *fname1, char *fname2, int mode, bool is_db = false);
	bool arrayTrie(char *fname1, char *fname2, int mode, bool is_db = false);
	~ArrayTrie();
	int  search(const char *str);
	int  search_longest(const char *str, int *n, int maxlen, bool get_offset = false);
	void insert(const char *str, int data);
	void remove(const char *str);
	void modify_data(const char *str, int data);

	int  getAllWords(char ***result);
	int  getAllWords_startWith(char *key, char ***result, int number);

	void getAllWords_sub(int base, int check_val, char *prep);

};

#define NEW_TRIE   1
#define OLD_TRIE   2

#endif

