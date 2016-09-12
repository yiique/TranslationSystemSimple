#ifndef _KMA_DATA_H_
#define _KMA_DATA_H_

#define MAX_TOKEN 50
#define MAX_TOKEN_LENGTH 80
#define MAX_RESULT 50
#define MAX_WORD 100
#define MAX_CANDI 200
#define MAX_SEN_LENGTH 2000
#define MAX_WORD_LENGTH 100

typedef struct  
{
	char key[50];
	int val;
	float prob;
}KEY_VAL;

typedef struct  
{
	char key[50];
	int val;
}DIC_KEY_VAL;

typedef struct  
{
	int id;
	float prob;
}ID_PROB;

typedef struct  
{
	char candi_str[MAX_TOKEN_LENGTH];
	int tag_id;
	int pre_idx;
	int pre_idxs[MAX_TOKEN];
	int pre_idx_co;
	int pos;
	bool is_end;
	char tag_str[MAX_TOKEN_LENGTH+10];
	char res_str[MAX_TOKEN_LENGTH*10];
}KMA_CANDI;

typedef struct 
{
	int ntoken;
	int token_tag[MAX_TOKEN];
	char token_str[MAX_TOKEN][MAX_TOKEN_LENGTH];
	char tag_list[MAX_TOKEN_LENGTH*2];
	int candi_id[MAX_TOKEN];
	float token_prob[MAX_TOKEN];
	bool is_complete;
	bool is_compound;
	float weight;
	float StateValue;
	int GoodPreID;
}KMA_RESULT;

typedef struct  
{
	int nres;
	int nuse;
	KMA_RESULT results[MAX_RESULT];
	char res_str[MAX_RESULT][MAX_WORD_LENGTH];
}KMA_WORD;

typedef struct 
{
	KMA_WORD words[MAX_WORD];
	int wrdcnt;
	int goodPath[MAX_WORD];
}KMA_SEN;

typedef struct  
{
	char key[50];
	char anal_res[100];
}PRE_DIC;

#endif
