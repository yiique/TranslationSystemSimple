/***********************************************************************
CompoundAnalysis.h  2011-12-24 written by Ri Hyon Sul 
***********************************************************************/


#if !defined (_COMPOUND_ANALYSIS_H_)
#define _COMPOUND_ANALYSIS_H_

#include "ArrayTrie.h"

typedef  struct
{	int  count;
	int terms[25];
}TERM_LIST;

typedef struct
{
	char *word;
	int   len;
	int   kind;  // 0: noun, 1: suffix, 2: adj, 4: number, 5: english, 6: symbol, 3: noun_with_suffix, 7: proper noun
	char *info;
	int   occ;
}SIMPLE_NOUN;

class SimpleNounSet{

	char buff[1000];
	char *cur_pos;
	char pairs[50][50];
public:
	int  count;
	SIMPLE_NOUN  sn[50];
	SimpleNounSet();
	int add(char *noun, int kind, int occ, char *info= NULL);
	int add(char *noun, /*CompoundAnalysis*/void *compound_analysis);
	bool search_pair(int i, int j, /*CompoundAnalysis*/void *compound_analysis);
	void clear();
	int  search(char *noun);
};


typedef struct{
	int first_child;
	int next_brother;
	int word;
}NODE;


#define MAX_WORD_LEN	100

class  CompoundAnalysis{

	SimpleNounSet  m_simple_set;

	char m_head[MAX_WORD_LEN];  // to use as a buffer
	
	int  m_head_words[MAX_WORD_LEN][11];  // because every simple word is less than 11
	int  m_end_words[MAX_WORD_LEN][11];  

	int  m_head_count[MAX_WORD_LEN];
	int  m_end_count[MAX_WORD_LEN];

	int  m_head_first_child[MAX_WORD_LEN];

	int  m_word_len;
	char m_word[MAX_WORD_LEN];

	void add_word(int index, int word); // add a word to m_head_words
	char *str(int i); // get string from simple noun number

	NODE  m_nodes[500];
	int   m_cur_node;
	int   m_start_node;
	int  add_node(int word, int child, int brother);
	int  make_tree(int index);  //return the first child

	bool all_search(int index, bool first_try);

	void filter();
	bool check_end(int index, int pre, int last_index);
	int  cal_count(int *words, int count, bool error = false);
	int  find_bigger(int index, int len);

	void conv_to_low(char  *str);
	bool is_number(char  *str);
	bool is_english(char  *str);

	bool is_to(char *str);
	int  head_terms(char *str, int *res);
	void reverse(int *vals, int count);
	char *head_word(char *str, int n);
	char *mid_word(char *str, int start, int n);
	int all_group(int first_child_node, TERM_LIST *tl_set, int max_num, bool first_try = true);
	int decide(TERM_LIST  *tl_set, int n, bool &comb_match);
//	int get_division(char *token0, char *result);
	int	 all_group(char *w, TERM_LIST *tl_set, int max_num, bool first_try = true);
	char *get_larger(char *left, char *center, char *right);
	
	bool is_comb_match(TERM_LIST *p, int &match_count);
	bool load_unit(char *fname);
	int  search_unit(char *str);
	
	bool is_num_sym(char *num);
	int  get_num_sym(char *num);
	bool is_unit_noun(char *unit);
	bool is_suffix(char*str, int index);

	bool valid_neigh(int wd1, int wd2, int brother_node);
	
	char *m_pUnit;
	int   m_countUnit;
	char *m_poinUnit[200];

public:
	ArrayTrie  m_trie_simple;
	FILE     *m_file_simple_info;
	ArrayTrie  m_trie_pair;

	CompoundAnalysis();
	bool  create(char *fname);
	~CompoundAnalysis();
	bool  analyse_word(char  *token, char *result, bool first_try);
	bool match_num_unit(char *num, char *unit);

};

#endif
