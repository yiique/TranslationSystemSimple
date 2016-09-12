// KMAnalyser.cpp: implementation of the KMAnalyser class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx_engine.h"
#include "KMAnalyser.h"
#include "Tag.h"
#include "StringTokenizer.h"
#include "ProbComputer.h"
#include "OptionSet.h"
#include "CompoundAnalysis.h"
#include "Zamo.h"
#include "KUnicode.h"
//#include "CodeConverter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _DB
#include "C:/mysql/include/mysql.h"
MYSQL *myData;
#endif
#include <iostream>
#include <string>
using namespace std;
#define BOUND_COUNT 61//2
DIC_KEY_VAL g_bound_list[BOUND_COUNT]={{"��", 1}, {"��", 3}, {"��", 3}, {"��", 3}, {"��", 1}, {"��", 1}, {"��", 3}, {"��", 3}, {"��", 3}, {"��", 1}, {"��", 3}, {"��", 1100}, {"��", 3}, /*{"��", 1}, */{"��", 3}, {"��", 3}, {"��", 1}, {"��", 3},{"��", 3}, {"��", 3}, {"��", 3}, {"��", 3}, {"��", 2}, {"��", 3}, {"��", 1}, {"��", 1}, {"��", 1101}, {"��", 1}, {"��", 3}, {"��",1100}, {"��",1100}, {"��", 1110},{"��", 3},{"��", 3},   {"��", 3}, {"��", 3}, {"��", 3}, /*{"��", 3}, */{"��", 3}, {"��", 3}, {"Ĵ", 1100}, {"Ů", 1100},{"Ƿ", 1100}, {"��", 1100}, {"ɬ", 3}, {"ʯ", 2}, {"ʿ", 1100}, {"��", 3}, {"��", 3}, {"��", 3}, {"��", 1100}, {"˺", 1100}, {"˼", 1}, {"˾", 1}, {"��", 2}, {"��", 1100}, {"��", 1001}, {"��", 31}, {"��", 31}, {"��", 3}, {"��", 1}, /*{"̩", 1}, */{"̮", 1}};

#define NNP_COUNT 57
DIC_KEY_VAL g_nnp_list[NNP_COUNT]={{"����", 0}, {"�ְ�", 0}, {"�ְ�", 0}, {"��", 0}, {"��", 0}, {"����", 0}, {"����", 0}, {"����", 0}, {"��", 0}, {"��", 0}, {"����", 0}, {"��", 0}, {"���", 0}, {"�����", 0}, {"��", 0}, {"��", 0}, {"��", 0}, {"�뻦", 0}, {"��", 0},  {"����", 0}, {"����", 0}, {"��", 0}, {"��", 0}, {"����", 0}, {"����", 0}, {"��",0}, {"����", 0}, {"����", 0}, {"����", 0}, {"����", 0}, {"����", 0}, {"����", 0}, {"����", 0}, {"����", 0}, {"����", 0}, {"����", 0}, {"����", 0}, {"��", 0}, {"��", 0}, {"����", 0}, {"ʭ��", 0}, {"ʭ�˱�", 0}, {"�̴�", 0}, {"�̴��̴�", 0}, {"�̴�", 0}, {"�ޱ�", 0}, {"�ޱ�����", 0}, {"����", 0}, {"����", 0}, {"���ü���", 0}, {"˧��", 0}, {"��", 0}, {"�˰�", 0}, {"�˰�", 0}, {"�˳�", 0}, {"����", 0}, {"�һ�", 0}};

#define TOWORD_COUNT 30
DIC_KEY_VAL g_to_word_list[TOWORD_COUNT]={{"��̩", 0}, {"�α�", 1}, {"��˾", 0},{"�ٴ�", 1},{"����", 1}, {"��ʯ", 1},{"��̩", 0}, {"����", 1},{"��̩", 0},{"��ʯ", 1},{"����", 1},{"����", 1},{"����", 0}, {"�д�", 1},{"����", 1},{"����", 1}, {"����", 0},{"��̩", 0},{"��̩", 0},{"�贪", 0}, {"��̩", 0},{"����", 1},{"ª��", 1},  {"��̩", 0},{"ã��", 0}, {"���", 1}, {"ɷ��", 1}, {"��", 1}, {"̩��", 0}, {"����", 1} };

bool g_test = false;


KMAnalyser::KMAnalyser()
{
	cout << "cqw is here" << endl;
	m_nInputCodeConvMode = 0;
	m_nOutputCodeConvMode = 0;
	m_pKmaSen = NULL;
	m_pCompoundAnalyer = NULL;
	m_pOptionSet = NULL;
	m_pZamo = NULL;
	m_pProbComputer = NULL;
	m_pDic = NULL;
	m_pUnicode = NULL;
}

KMAnalyser::~KMAnalyser()
{
	if(!m_pKmaSen)
		delete m_pKmaSen;
	if(!m_pCompoundAnalyer)
		delete m_pCompoundAnalyer;
	if(!m_pOptionSet)
		delete m_pOptionSet;
	if(!m_pZamo)
		delete m_pZamo;
	if(!m_pProbComputer)
		delete m_pProbComputer;
	if(!m_pDic)
		delete m_pDic;
	if(!m_pUnicode)
		delete m_pUnicode;
}

int code_comp(const void *v1, const void *v2)
{
	unsigned char *s1 = (unsigned char *)v1;
	unsigned char *s2 = (unsigned char *) v2;
	return strncmp((char *)s1, (char *)s2, 2);
}

void kor2byteto1byte(char *str, char *target)
{
	const char conv[] = "\xa1\xa1\x20\xa1\xa3\x2e\xa1\xa4\x2c\xa1\xa5\x2e\xa1\xa7\x3a\xa1\xa8\x3b\xa1\xa9\x3f\xa1\xaa\x21\xa1\xad\x7e\xa1\xb1\x5f\xa1\xb3\x2f\xa1\xb4\x5c\xa1\xb5\x7c\xa1\xb7\x2f\xa1\xb8\x5c\xa1\xbe\x5e\xa1\xca\x28\xa1\xcb\x29\xa1\xcc\x5b\xa1\xcd\x5d\xa1\xce\x5b\xa1\xcf\x5d\xa1\xd0\x7b\xa1\xd1\x7d\xa1\xd2\x3c\xa1\xd3\x3e\xa2\xa1\x2b\xa2\xa2\x2d\xa2\xa6\x3d\xa2\xa8\x3c\xa2\xa9\x3e\xa2\xd7\x23\xa2\xd8\x26\xa2\xd9\x2a\xa2\xda\x40\xa3\xb0\x30\xa3\xb1\x31\xa3\xb2\x32\xa3\xb3\x33\xa3\xb4\x34\xa3\xb5\x35\xa3\xb6\x36\xa3\xb7\x37\xa3\xb8\x38\xa3\xb9\x39\xa3\xc1\x41\xa3\xc2\x42\xa3\xc3\x43\xa3\xc4\x44\xa3\xc5\x45\xa3\xc6\x46\xa3\xc7\x47\xa3\xc8\x48\xa3\xc9\x49\xa3\xca\x4a\xa3\xcb\x4b\xa3\xcc\x4c\xa3\xcd\x4d\xa3\xce\x4e\xa3\xcf\x4f\xa3\xd0\x50\xa3\xd1\x51\xa3\xd2\x52\xa3\xd3\x53\xa3\xd4\x54\xa3\xd5\x55\xa3\xd6\x56\xa3\xd7\x57\xa3\xd8\x58\xa3\xd9\x59\xa3\xda\x5a\xa3\xe1\x61\xa3\xe2\x62\xa3\xe3\x63\xa3\xe4\x64\xa3\xe5\x65\xa3\xe6\x66\xa3\xe7\x67\xa3\xe8\x68\xa3\xe9\x69\xa3\xea\x6a\xa3\xeb\x6b\xa3\xec\x6c\xa3\xed\x6d\xa3\xee\x6e\xa3\xef\x6f\xa3\xf0\x70\xa3\xf1\x71\xa3\xf2\x72\xa3\xf3\x73\xa3\xf4\x74\xa3\xf5\x75\xa3\xf6\x76\xa3\xf7\x77\xa3\xf8\x78\xa3\xf9\x79\xa3\xfa\x7a\xa8\xa2\x27\xa8\xa3\x22\xa8\xa8\x24\xa8\xac\x25"; 
	int n = strlen(conv) /3;
	int i = 0;
	int j = 0;
	int len = strlen(str);
	unsigned char *po = (unsigned char *)str;
	while(i < len)
	{
		if(po[i] < 0x80)
		{	target[j] = po[i]; j++; i++;  continue;  }
		char *p = (char *)bsearch(po+i, conv, n, 3, code_comp); 
		if(p != NULL)
		{	target[j] = p[2]; j++; i+=2;}
		else
		{	target[j] = po[i]; j++; i++;
			target[j] = po[i]; j++; i++;
		}
	}
	target[j] = 0;
}


int get_count(char *str, const char*symbol, bool flag)
{
	char *po = str;
	int i=0;
	while (*po!=0)
	{
		if(*po == *symbol)
		{
			i++;
			char *p1=po-1;
			char *p2=po-2;
			if(*symbol=='>' && p1 && p2 && (*p1=='=' ||  *p2=='='))
				i--;
		}
		po++;
	}
	if(flag)
		return i+1;
	else
		return i;
}

int compare_pre(const void *v1, const void *v2)
{
	char *v11 = (char*)v1;
	PRE_DIC *v22 = (PRE_DIC*)v2;
	return strcmp(v11, v22->key);
}

int compare(const void *v1, const void *v2)
{
	char *v11 = (char*)v1;
	KEY_VAL *v22 = (KEY_VAL*)v2;
	return strcmp(v11, v22->key);
}

int compare_dic(const void *v1, const void *v2)
{
	char *v11 = (char*)v1;
	DIC_KEY_VAL *v22 = (DIC_KEY_VAL*)v2;
	return strcmp(v11, v22->key);
}

void trim(char *str)
{
	if(strlen(str) == 0)
		return;
	char *po1, *po2;
	po1 = str;
	while(*po1 > 0 && *po1 <=' ')
		po1++;
	while(*po1 == 0xa1 && *(po1+1) == 0xa1)
		po1+=2;
	po2 = str + strlen(str) -1;
	while(po2 >=str && *po2 > 0 && *po2 <= ' ')
		po2--;
	po2++;
	if(po2 == str)
	{	str[0] = 0;
		return;
	}
	*po2 = 0;
	if(po1 != str && po2 >=po1)
		memmove(str, po1, po2-po1+1);
}

void KMAnalyser::pre_proc(char *pInputSen)
{
	trim(pInputSen);
	m_SymProcer.insert_space(pInputSen);
	m_SymProcer.proc_string(pInputSen);
}

void KMAnalyser::proc_sen(const char *src_str)
{
	m_res_buff[0] = 0;
	int len = strlen(src_str);
	char* str = (char*)malloc(len*3);

	if(m_nInputCodeConvMode != 0)
	{
		*str = 0;
		int i=code_convert((char*)src_str, str, m_nInputCodeConvMode);
		//                printf("[i%d]",i);
		if(i<0)
		{
			free(str);
			return ;
		}
	}
	else
		strcpy(str, src_str);

	m_pInputSen = str;

	m_pKmaSen->wrdcnt=0;
	m_word_count = 0;
	trim(str);
	//printf("[2strlen:%d]\n",strlen(str));		
	if(strlen(str) == 0 || strlen(str) > MAX_SEN_LENGTH)
	{
		free(str);
		return ;
	}

	char buff[MAX_SEN_LENGTH];
	std::cout << "m_pOptionSet->m_outputMode:" << m_pOptionSet->m_outputMode << std::endl;
	if(m_pOptionSet->m_outputMode == 3)
		sprintf(m_res_buff,"[%s]\r\n",str);

	kor2byteto1byte(str, buff);

	pre_proc(buff);
	std::cout << "pre_proc buf:" << buff << std::endl;
	m_pKmaSen->wrdcnt = get_count(buff, " ", true);
	//printf("[m_pKmaSen->wrdcnt:%d]\n",m_pKmaSen->wrdcnt);
	if(m_pKmaSen->wrdcnt >= MAX_WORD)
	{
		free(str);
		return;
	}

	pre_eojol_last_tagid = 0;
	int word_idx=0;
	int pos=0;
	StringTokenizer token(buff, " ");
	char *po;
	while (po = (char*)token.nextToken())
	{
		//cout << "cqw debug: token " << po << endl;
		if(strlen(po) > MAX_WORD_LENGTH)
			continue;
		proc_word(po, &m_pKmaSen->words[word_idx]);
		//cout << "cqw debug: proc word " << po << endl;
		compute_whole_path_weight(word_idx);
		word_idx++;
	}
	printf("[word_count:%d]\n",word_idx);
	if(word_idx == 0 )
	{
		std::cout << "str wi: " << m_res_buff << endl;
		free(str);
		return ;
	}
	m_pKmaSen->wrdcnt = word_idx;
	m_word_count = word_idx;

	if(g_test)
		make_string();
	else
	{
		if(m_pOptionSet->m_ambigRes==0)
		{
			int goodID = get_last_max_ID();
			get_good_path(goodID);
			//cout << "cqw debug: before " << m_res_buff << endl;
			print_anal_result();
		}
		else
			print_anal_multi_result();
	}
	std::cout << "str:" << m_res_buff << std::endl;
	free(str);

}

bool KMAnalyser::load_assist_dic(const char *path, int kind)
{
	char fname[MAX_PATH];
	sprintf(fname, "%svi.dic", path);
	DIC_KEY_VAL *key_freq = 0;
	int co = 0;
	int limit = MAX_VI_DIC;

	FILE *fp = fopen(fname, "rt");
	if(fp == NULL)
		return false;
	key_freq = m_vi_dic;
	char buff[100];
	while (fgets(buff, 100, fp))
	{
		buff[strlen(buff) -1] = 0;
		char *p1 = strstr(buff, "#");
		if(p1 == NULL || buff[0] == '/')
			continue;
		key_freq[co].val = atoi(p1+1);
		*p1 = 0;
		if(strlen(buff) > 50)
			continue;
		strcpy(key_freq[co].key, buff);
		co++;
		if(co == limit)
			break;
	}
	m_vi_dic_co = co;
	fclose(fp);

	sprintf(fname, "%spre_info.dic", path);
	FILE *fp2 = fopen(fname, "rt");
	if(fp2 == NULL)
		return false;
	co=0;
	while (fgets(buff, 100, fp))
	{
		buff[strlen(buff) -1] = 0;
		char *p1 = strstr(buff, "#");
		if(p1==NULL) continue;
		if(strlen(buff) > 100)
			continue;
		strcpy(m_pre_dic[co].anal_res, p1+1);
		*p1 = 0;
		if(strlen(buff) > 50)
			continue;
		strcpy(m_pre_dic[co].key, buff);
		if(co == MAX_PRE_DIC)
			break;
		co++;
	}
	m_pre_dic_co = co;
	fclose(fp2);

	sprintf(fname, "%sadv.dic", path);
	FILE *fp3 = fopen(fname, "rt");
	if(fp3 == NULL)
		return false;
	co=0;
	while (fgets(buff, 100, fp))
	{
		buff[strlen(buff)-1] = 0;
		char *p1 = strstr(buff, "#");
		if(p1==NULL) 
			continue;
		if(strlen(buff) > 100)
			continue;
		m_adv_dic[co].val=atoi(p1+1);
		*p1 = 0;
		if(strlen(buff) > 50)
			continue;
		strcpy(m_adv_dic[co].key, buff);
		if(co == MAX_ADV_DIC)
			break;
		co++;
	}
	m_adv_dic_co = co;
	fclose(fp3);

	qsort(g_bound_list, BOUND_COUNT, sizeof(DIC_KEY_VAL), compare_dic);
	qsort(g_nnp_list, NNP_COUNT, sizeof(DIC_KEY_VAL), compare_dic);
	qsort(g_to_word_list, TOWORD_COUNT, sizeof(DIC_KEY_VAL), compare_dic);
	qsort(m_vi_dic, m_vi_dic_co, sizeof(DIC_KEY_VAL), compare_dic);
	//	qsort(m_pre_dic, m_pre_dic_co, sizeof(PRE_DIC), compare_dic);
	qsort(m_adv_dic, m_adv_dic_co, sizeof(DIC_KEY_VAL), compare_dic);
	return true;
}

void KMAnalyser::initialize()
{
	m_word->nres = 0;
	m_candi_co = 0;
	memset(m_end_idx, -1, sizeof(char*)*50);
	m_end_co = 0;
	memset(m_tail_pos, -1, sizeof(int)*10);
	memset(m_tail_val, -1, sizeof(int)*10);
	m_tail_co = 0;
	m_dic_res_co = 0;
	memset(m_offset_res_co, 0, sizeof(int)*MAX_WORD_LENGTH);
	memset(m_offset_res, -1, sizeof(int)*MAX_WORD_LENGTH*50);

	memset(m_sepa_pos, -1, sizeof(int)*20);
	memset(m_sepa_flags, -1, sizeof(int)*20);
	memset(m_sepa_str, 0, MAX_TOKEN_LENGTH*20);
	m_sepa_co = 0;

}

void KMAnalyser::proc_word(char *str, KMA_WORD *word)
{
	m_word = word;
	bool res = false;
	initialize();
	if(str==NULL|| strlen(str) > MAX_WORD_LENGTH)
		return;
	strcpy(m_input, str);
	if(proc_except(str))
	{ 
		return;
	}
	if(!proc_pre_anal_dic(str))
	{	
		if(find_bound(str))
		{
			make_result(1);
			return;
		}
		analyse_word(str, -1, 0);
		if(m_end_co == 0)
			proc_other(str);
		make_result(1);
	}

	if(m_word->nres == 0)
		make_result(3);
	m_max_id = get_eojol_prob();
}

bool KMAnalyser::proc_pre_anal_dic(char *str)
{
	if(strlen(str) > MAX_WORD_LENGTH)
		return false;
	char key[MAX_WORD_LENGTH];
	memset(key, 0, MAX_WORD_LENGTH);
	char* some[] = {"�ͼ���", "�ͼѻ����","�ճ�",  "�ջ�����", "�׳�", "�͵�", "�����", "�ٳ�", NULL};
	char* pp;
	int i;
	for(i=0; some[i]; i++)
	{
		pp = strstr(str, some[i]);
		if(pp > 0 && pp > str && strlen(pp) == strlen(some[i]))
		{
			strcpy(key, pp);
			break;
		}
	}
	if(strlen(key) == 0 || (i >1 && (strncmp(pp-2,"ʭ", 2)==0 || strncmp(pp-2,"��", 2)==0 || strncmp(pp-2,"��", 2)==0 ||strncmp(pp-2,"̮", 2)==0 || strncmp(pp-2,"̸", 2)==0)))
		strcpy(key, str); 

	PRE_DIC *p= (PRE_DIC*)bsearch(key, m_pre_dic, m_pre_dic_co, sizeof(PRE_DIC), compare_pre);
	if(p)
	{
		StringTokenizer token(p->anal_res, ",");
		char *po;
		while (po=(char*)token.nextToken())
		{
			memset(m_word->res_str[m_word->nres], 0, MAX_WORD_LENGTH);
			if(strlen(str) > strlen(key))
			{
				if(m_pOptionSet->m_add_letter == 1 && strstr(po, "VVB"))
				{
					strncpy(m_word->res_str[m_word->nres], str, strlen(str)-strlen(key));
					strcat(m_word->res_str[m_word->nres], po);
				}
				else if(strstr(po, "SFV"))
				{
					strncpy(m_word->res_str[m_word->nres], str, strlen(str)-strlen(key));
					strcat(m_word->res_str[m_word->nres], "/NNC+");
					strcat(m_word->res_str[m_word->nres], po);
				}
				else
					continue;
			}
			else
				strcpy(m_word->res_str[m_word->nres], po);

			m_word->results[m_word->nres].tag_list[0]=0;
			int k=make_sub_result(m_word->res_str[m_word->nres], m_word->results[m_word->nres].tag_list);
			m_word->results[m_word->nres].ntoken = k;
			m_word->results[m_word->nres].is_complete = true;
			m_word->nres++;
		}
		return true;
	}
	return false;
}

bool KMAnalyser::check_list(char *source)
{
	if(!source || strlen(source) > 3)
		return false;
	char *gana[20] = {"\xb0\xa1","\xb1\xfd","\xb3\xde","\xb5\xb9","\xb6\xeb","\xb8\xc6","\xba\xa1","\xca\xad","\xbb\xf4","\xbd\xd3","\xbe\xf2","\xc0\xb0","\xc1\xc4","\xc2\xd7", NULL};
	char *gunu[20] = {"\xa4\xa1","\xa4\xa2","\xa4\xa3","\xa4\xa4","\xa4\xa5","\xa4\xa6","\xa4\xa7","\xa4\xa8","\xa4\xa9","\xa4\xaa","\xa4\xab","\xa4\xac","\xa4\xad","\xa4\xae", NULL};
	char stem[3]; 
	char* tm;
	if(*source == '(')
		tm = source + 1;
	else
		tm = source;
	while(*tm == ' ')
		tm++;
	if(*(tm+strlen(tm)-1) != ')' && *(tm+strlen(tm)-1) != '.')
		return false;
	else
	{	
		strncpy(stem, tm, strlen(tm)-1);
		stem[strlen(tm)-1]=0;
	}
	int i;
	for(i = 0; gana[i]; i++)
	{	
		if(!strcmp(gana[i], stem))
			return true;
	}
	for(i = 0; gunu[i]; i++)
	{
		if(!strcmp(gunu[i], stem))
			return true;
	}
	if((strlen(stem) == 1 && stem[0] >= '1' && stem[0] <= '9') ||
			(strlen(stem) == 2 && stem[0] == '1' && stem[1] >= '0' && stem[1] <= '9')) 
		return true;
	if(strlen(stem) == 1 && ((stem[0] >= 'a' && stem[0] <= 'z') ||
				(stem[0] >= 'A' && stem[0] <= 'Z')))
		return true;
	return false;
}

bool KMAnalyser::proc_except(char *str)
{
//	printf("[begin of proc_except:%s\n",str);
//printf("strlen(str)%d\n",strlen(str));
//printf("MAX_WORD_LENGTH%d\n",MAX_WORD_LENGTH);
	if(strlen(str) > MAX_WORD_LENGTH)
	{
//		printf("[a:%s\n",str);
		m_word->results[0].token_tag[0] = T_SWT;	//T_SW;
		strcpy(m_word->results[0].token_str[0], str);
		m_word->results[0].is_complete = true;
		m_word->results[0].ntoken = 1;
		m_word->nres = 1;
		return true;
	}
	if (check_list(str))
	{
//		printf("[b:%s\n",str);
		m_word->results[0].token_tag[0] = T_LST;
		strcpy(m_word->results[0].token_str[0], str);
		m_word->results[0].is_complete = true;
		m_word->results[0].ntoken = 1;
		m_word->nres = 1;
		return true;
	}
	if(strcmp(str, ".") == 0 || strcmp(str, "?") == 0)
	{
//		printf("[c:%s\n",str);
		m_word->results[0].token_tag[0] = T_SEN;
		strcpy(m_word->results[0].token_str[0], str);
		m_word->results[0].is_complete = true;
		m_word->results[0].ntoken = 1;
		m_word->nres = 1;
		return true;
	}
	else if (strcmp(str, ",") == 0)
	{
//		printf("[d:%s\n",str);
		m_word->results[0].token_tag[0] = T_SCM;
		strcpy(m_word->results[0].token_str[0], str);
		m_word->results[0].is_complete = true;
		m_word->results[0].ntoken = 1;
		m_word->nres = 1;
		return true;
	}
	else
	{
//		printf("[e:%s\n",str);
		bool is_single=true;
		unsigned char *po = (unsigned char*)str;
		while (*po!=0)
		{
			if(*po>=0xb0)
			{
				is_single = false;
				break;
			}
			po++;
		}
		if(is_single)
		{
//			printf("[Single:%s\n",str);
//			printf("[po:%s\n",po);
			char res_str[100];//default:100
			int flags[20];
			int sepa_co = m_SymProcer.separate_sym(str, res_str, flags);
			char* po = res_str;
			int i, j = -1, flag, pre_tag=0;
			for (i=0; i<sepa_co; i++)
			{
				int tag = m_SymProcer.get_symbol_tag(po, flags[i]);

				if(m_pOptionSet->m_sym_mode == 1)
				{
//					printf("[ff:%s\n",po);
					if(j == -1)
						j ++;
					m_word->results[0].token_tag[j] = tag;
					strcpy(m_word->results[0].token_str[j], (char*)po);
					po+=strlen((char*)po)+1;
					j ++;
					flag = 0;
				}
				else
				{
					if(tag == T_SQR || tag == T_SQL)
					{
//						printf("[g:%s\n",po);
						j ++;
						m_word->results[0].token_tag[j] = tag;
						strcpy(m_word->results[0].token_str[j], (char*)po);
						po+=strlen((char*)po)+1;
						j ++;
						m_word->results[0].token_str[j][0] = 0;
						flag = 0;
					}
					else
					{
						if(j == -1)
						{
//							printf("[j:%d\n",j);
							j ++;
							m_word->results[0].token_str[j][0] = 0;
						}
//						printf("[pretag:%d\n",pre_tag);
//						printf("[tag:%d\n",tag);
						if(pre_tag != 0 && pre_tag != tag)
							m_word->results[0].token_tag[j] = T_SWT;
						else
							m_word->results[0].token_tag[j] = tag;
						strcat(m_word->results[0].token_str[j], (char*)po);
						po+=strlen((char*)po)+1;
						flag = 1;
						pre_tag = tag;
					}

				}
			}
			if(flag == 1)
				j ++;
			m_word->results[0].is_complete = true;
			m_word->results[0].ntoken = j;
			m_word->nres = 1;
//			printf("[return true,j:%d\n",j);
			return true;
		}

	}
//	printf("[k:%s\n",str);
	DIC_KEY_VAL *p = (DIC_KEY_VAL*)bsearch(str, &m_adv_dic, m_adv_dic_co, sizeof(DIC_KEY_VAL), compare_dic);
	if(p)
	{
//		printf("[p:%s\n",str);
		m_word->results[0].token_tag[0] = p->val;//T_ADV;
		strcpy(m_word->results[0].token_str[0], str);
		m_word->results[0].is_complete = true;
		m_word->results[0].ntoken = 1;
		m_word->nres = 1;
		return true;
	}
//	printf("[return false:\n");
	return false;
}

int KMAnalyser::find_dic_res(char *str, int pos, int kind, int *offset)
{
	if(strlen(str) == 0)
		return -1;
	int i;
	if (kind==1 || kind==2)
	{
		int dic_res_id = -1;
		for (i=0; i<m_offset_res_co[pos]; i++)
		{
			dic_res_id = m_offset_res[pos][i];
			if(strcmp(m_dic_res[dic_res_id].key, str)==0)
				return dic_res_id;
		}
		if(kind==2)
			return -1;
		if(m_dic_res_co == MAX_TOKEN)
			return -1;
#ifdef _DB
		m_pDic->search(str, &m_dic_res[m_dic_res_co], true);
#else
		m_pDic->search(str, &m_dic_res[m_dic_res_co], false);
#endif

		m_offset_res[pos][m_offset_res_co[pos]++] = m_dic_res_co;
		m_dic_res_co++;
		return m_dic_res_co-1;
	}
	else if(kind == 0)
	{
		return m_pDic->search_all(str, offset);
	}
	return -1;
}

void KMAnalyser::analyse_word(char *str, int pre_idx, int pos, bool is_err, bool is_one) //
{
	if (str == NULL || strlen(str) == 0 || strlen(str) > MAX_WORD_LENGTH)
		return;

	int tag0 = pos==0?0:m_candis[pre_idx].tag_id;
	char *str1 = pos==0?NULL:m_candis[pre_idx].candi_str;

	int offset[MAX_WORD_LENGTH];
	int co=-1;
	int dic_res_id = -1;
	if((dic_res_id = find_dic_res(str, pos, 2)) == -1)
		co = find_dic_res(str, pos, 0, offset);
	else
		co = strlen(m_dic_res[dic_res_id].key);

	bool is_compound = false;
	bool end_flag=false;
	bool is_end=false;

	if(co == 0 && tag0 == 0)
	{ 
		int len_num = get_num_str(str);
		if(len_num < 1 || len_num > 30)
			return;
		char num_str[30];
		strncpy(num_str, str, len_num);
		num_str[len_num] = 0;
		int cur_idx = make_candi(num_str, T_SNM, 0, pre_idx);
		if(cur_idx >= 0)
			analyse_word(str+len_num, cur_idx, len_num);
	}
	int len=0;
	for (int i = co-1; i >= 0; i--)
	{
		if(m_end_co>0 && end_flag)
			break;
		if(dic_res_id == -1 && (offset[i] == -1 || is_compound))
			continue;

		if(dic_res_id == -1)
			len = offset[i];
		else if(dic_res_id != -1 &&  i == co-1)
			len = co;
		else
			len -= 2;
		if(len <= 0)
			break;

		char key1[100];
		strncpy(key1, str, len);
		key1[len] = 0;
		int res_idx;

		if(dic_res_id != -1 &&  i < co-1)
		{
			while((res_idx = find_dic_res(key1, pos, 1)) == -1 && len >= 2)
			{
				key1[len-2] = 0;
				len -= 2;
			}
			if(res_idx == -1)
				break;
		}
		else
			res_idx = find_dic_res(key1, pos, 1);
		if(res_idx==-1)
			continue;
		is_end=len==(int)strlen(str)?true:false;

		if(m_dic_res[res_idx].is_compound)
			is_compound = true;
		if (m_dic_res[res_idx].preInfo_num > 0 && strcmp(key1, "��")!=0)
		{
			for (int j = 0; j < m_dic_res[res_idx].preInfo_num; j++)
			{
				int tag_num = m_dic_res[res_idx].preInfo[j].tag_num;
				int tag1 = m_dic_res[res_idx].preInfo[j].tags[0];
				if(is_err && (tag1 ==T_VVB || (tag0 == T_NNC && tag1 != T_TND && strcmp(key1, "��")==0)))
					continue;
				if(is_err && tag0 == 0)
					tag0 = T_NNC;
				if(!check_comb_str(tag0, tag1, str1, m_dic_res[res_idx].preInfo[j].morph_str[0]))
					continue;
				if(pos==0 && (tag1==T_VBT||tag1==T_VXT||tag1==T_VAT))
					continue;
				int cur_idx = pre_idx;
				bool is_sub_end = false;
				for (int k = 0; k < tag_num; k++)
				{
					char *morph_str = m_dic_res[res_idx].preInfo[j].morph_str[k];
					int morph_tag = m_dic_res[res_idx].preInfo[j].tags[k];
					if(is_end && k==tag_num-1)
						is_sub_end = true;
					cur_idx = make_candi(morph_str, morph_tag, pos, cur_idx, is_sub_end, is_compound);
					if(cur_idx == -1)
						break;
				}
				if(cur_idx >= 0 && !is_end)
					analyse_word(str+len, cur_idx, pos+len);
			}	

		}
		if (!is_compound && m_dic_res[res_idx].tagInfo_num > 0)
		{
			int tag1;
			for (int j = 0; j < m_dic_res[res_idx].tagInfo_num; j++)
			{
				tag1 = m_dic_res[res_idx].tagInfo[j].tag_id;
				if(is_err && (tag1 <T_TCS || tag1>T_TCC) && tag1!=T_TXP && tag1!=T_SFV && tag1!=T_TEN && tag1!=T_NNU && tag1!=T_NNX && tag1!=T_TND )
					continue;
				if(!check_comb_str(tag0, tag1, str1, key1) || 
						(is_end && check_comb(tag1, 0) == false))
					continue;
				int cur_idx = make_candi(key1, tag1, pos, pre_idx, is_end);
				if(cur_idx >= 0 && !is_end)
					analyse_word(str+len, cur_idx, pos+len);
				if(tag0 == T_SNM && tag1 == T_NNU)
					break;
			}	
		}
		end_flag=true;
		if(is_end && tag0 ==0)
			break;
		if(is_compound && is_one)
			break;
	}
}

void KMAnalyser::make_result(int type)
{
	if(m_word->nres == MAX_RESULT)
		return;
	int pre_id;
	if(type == 1)
	{
		char buff[MAX_WORD_LENGTH*3];
		int i;
		for (i=0; i<m_end_co; i++)
		{
			if(m_end_idx[i]==-1)
				continue;
			int cur_idx = m_end_idx[i];
			while (cur_idx!=-1 && m_candis[cur_idx].pre_idx!=-1)
			{
				cur_idx = m_candis[cur_idx].pre_idx;
			}
			if(m_candis[cur_idx].pos != 0)
				continue;
			strcpy(buff, m_candis[m_end_idx[i]].res_str);
			m_word->results[m_word->nres].tag_list[0] = 0;
			int k=make_sub_result(buff, m_word->results[m_word->nres].tag_list);

			//			if(strstr(m_word->results[m_word->nres].tag_list, "NNC+VVB+TDM+NNC")||strstr(m_word->results[m_word->nres].tag_list, "NNC+VAJ+TDM+NNC")||strstr(m_word->results[m_word->nres].tag_list, "NNC+VVB+TDN+NNB"))
			//				continue;
			strcpy(m_word->res_str[m_word->nres], m_candis[m_end_idx[i]].res_str);
			m_word->results[m_word->nres].ntoken = k;
			m_word->results[m_word->nres].is_complete = true;
			for (pre_id=0; pre_id<m_candis[m_end_idx[i]].pre_idx_co; pre_id++)
			{
				m_word->results[m_word->nres].candi_id[pre_id] = m_candis[m_end_idx[i]].pre_idxs[pre_id];
			}
			m_word->results[m_word->nres].candi_id[pre_id] = m_end_idx[i];
			m_word->nres++;
			if(m_word->nres == MAX_RESULT)
				return;
		}
	}
	else if(type==2)
	{
		int res_list[MAX_TOKEN];
		int res_list_co;
		bool is_start;
		int noun_co;
		if(m_word->nres == MAX_RESULT)
			return;
		int i, j, k;
		for (i=0; i<m_end_co; i++)
		{
			if(m_end_idx[i]==-1)
				continue;
			res_list_co = 0;
			is_start = false;
			noun_co = 0;
			memset(res_list, -1, 4*10);
			int cur_idx = m_end_idx[i];
			while (cur_idx >= 0 && cur_idx < m_candi_co)
			{
				res_list[res_list_co] = cur_idx;
				res_list_co++;
				if(res_list_co == MAX_TOKEN)
					break;
				is_start = m_candis[cur_idx].pos==0?true:false;
				cur_idx = m_candis[cur_idx].pre_idx;
			}
			if(is_start == false)
				continue;
			if(res_list_co >= MAX_TOKEN)
				continue;
			m_word->results[m_word->nres].ntoken = 0;
			k = 0;
			for (j=res_list_co-1; j>=0; j--)
			{
				int new_tag_id = m_candis[res_list[j]].tag_id;
				m_word->results[m_word->nres].token_tag[k] = new_tag_id;
				if(k==0)
					strcpy(m_word->results[m_word->nres].tag_list, tag_name[new_tag_id]);
				else
					sprintf(m_word->results[m_word->nres].tag_list, "%s+%s", m_word->results[m_word->nres].tag_list, tag_name[new_tag_id]);
				strcpy(m_word->results[m_word->nres].token_str[k], m_candis[res_list[j]].candi_str);
				m_word->results[m_word->nres].candi_id[k] = res_list[j];
				k++;
				if(k==MAX_TOKEN)
					break;
			}
			if(strstr(m_word->results[m_word->nres].tag_list, "NNC+VVB+TDM+NNC"))
				continue;
			m_word->results[m_word->nres].ntoken = k;
			m_word->results[m_word->nres].is_complete = true;
			for (pre_id=0; pre_id<m_candis[m_end_idx[i]].pre_idx_co; pre_id++)
			{
				m_word->results[m_word->nres].candi_id[pre_id] = m_candis[m_end_idx[i]].pre_idxs[pre_id];
			}
			m_word->results[m_word->nres].candi_id[pre_id] = m_end_idx[i];
			m_word->nres++;
			if(m_word->nres == MAX_RESULT)
				return;
		}
	}
	else if(type == 3)
	{
		m_word->results[m_word->nres].token_tag[0] = T_NNC;
		sprintf(m_word->results[m_word->nres].token_str[0], "%s[UNKNOWN]", m_input);
		m_word->results[m_word->nres].ntoken = 1;
		m_word->results[m_word->nres].is_complete = true;
		m_word->nres = 1;
	}
}

void KMAnalyser::make_string()
{
	char* res = m_res_buff;
	int max_len = MAX_SEN_LENGTH*3;

	if(m_nOutputCodeConvMode == L_K_UTF8)
		max_len /= 3; 
	if(m_word == NULL)
		return;

	strcpy(res, "");
	int i,j;
	for (i=0; i<m_word->nres; i++)
	{
		m_word->res_str[i][0] = 0;
		for (j = 0; j < m_word->results[i].ntoken; j++)
		{
			if(m_word->results[i].token_tag[j]==-1)
				continue;
			char *punk = strstr(m_word->results[i].token_str[j], "[UNKNOWN]");
			if(punk)
				*punk=0;
			if(j==0)
			{
				if (m_pOptionSet->m_outputMode == 2)
					sprintf(m_word->res_str[i], "%s",  m_word->results[i].token_str[j]);
				else if(m_pOptionSet->m_outputMode == 4)
					sprintf(m_word->res_str[i], "%s|%s",  m_word->results[i].token_str[j], tag_name[m_word->results[i].token_tag[j]]);
				else
					sprintf(m_word->res_str[i], "%s/%s",  m_word->results[i].token_str[j], tag_name[m_word->results[i].token_tag[j]]);
			}
			else
			{
				if(m_pOptionSet->m_outputMode == 2)
					sprintf(m_word->res_str[i], "%s %s", m_word->res_str[i], m_word->results[i].token_str[j]);
				else if(m_pOptionSet->m_outputMode == 1)
					sprintf(m_word->res_str[i], "%s %s/%s", m_word->res_str[i], m_word->results[i].token_str[j], tag_name[m_word->results[i].token_tag[j]]);
				else if(m_pOptionSet->m_outputMode == 4)
					sprintf(m_word->res_str[i], "%s %s|%s", m_word->res_str[i], m_word->results[i].token_str[j], tag_name[m_word->results[i].token_tag[j]]);
				else
					sprintf(m_word->res_str[i], "%s+%s/%s", m_word->res_str[i], m_word->results[i].token_str[j], tag_name[m_word->results[i].token_tag[j]]);
			}
		}
		if(m_word->results[i].is_complete==false)
			sprintf(m_word->res_str[i], "%s (������)", m_word->res_str[i]);
		sprintf(m_word->res_str[i], "%s%\t%f", m_word->res_str[i], m_word->results[i].weight);
		sprintf(res, "%s%s%s", res, m_word->res_str[i], "\r\n");
		if((int)strlen(res) > max_len)
			return;
	}
	if(m_nOutputCodeConvMode != 0)
	{
		int len = strlen(res);
		char* szTarget;
		if(m_nOutputCodeConvMode == L_K_UTF8)
			szTarget = (char*)malloc(len*3+1);
		else
			szTarget = (char*)malloc(len+1);
		*szTarget = 0;
		if(code_convert(res, szTarget, m_nOutputCodeConvMode)>=0)
			strcpy(res, szTarget);
		free(szTarget);
	}
}

int KMAnalyser::make_candi(char *str, int tag_id, int pos, int pre_idx, bool is_end, bool is_compound)
{
	if(str==NULL || strlen(str)==0 ||  strlen(str) > MAX_TOKEN_LENGTH || m_candi_co == MAX_CANDI)
		return -1;
	int new_tag_id = tag_id;
	if(is_compound == false && pre_idx != -1 && m_candis[pre_idx].tag_id ==T_NNC && tag_id ==T_NNC && (strlen(m_candis[pre_idx].candi_str)==2
				|| strlen(str) == 2))
		return -1;
	int i;
	for (i=0; i<m_candi_co; i++)
	{
		if(pos != 0 && new_tag_id == m_candis[i].tag_id && strcmp(str, m_candis[i].candi_str)==0 && pre_idx == m_candis[i].pre_idx)
			return i;
	}
	if(is_end && new_tag_id==T_NNC && strlen(str)==2)
	{
		for (int tail_idx=0; tail_idx<m_tail_co; tail_idx++)
		{
			if(m_tail_val[tail_idx] == 1 || m_tail_val[tail_idx] == 2)
				return -1;
		}
	}
	int cur_idx = m_candi_co;
	m_candis[cur_idx].tag_id = new_tag_id;
	if(strcmp(str, "+")==0)
		strcpy(m_candis[cur_idx].candi_str, "��");
	else
		strcpy(m_candis[cur_idx].candi_str, str);
	m_candis[cur_idx].pre_idx = pre_idx;
	m_candis[cur_idx].pos = pos;
	if (is_end && m_end_co < 100)
	{
		m_candis[cur_idx].is_end = true;
		m_end_idx[m_end_co++] = cur_idx;
	}
	sprintf(m_candis[cur_idx].tag_str, "%s/%s", str, tag_name[new_tag_id]);
	if(pre_idx==-1)
	{
		strcpy(m_candis[cur_idx].res_str, m_candis[cur_idx].tag_str);
		memset(m_candis[cur_idx].pre_idxs, -1, sizeof(int)*MAX_TOKEN);
		m_candis[cur_idx].pre_idx_co = 0;
	}
	else
	{
		sprintf(m_candis[cur_idx].res_str, "%s+%s", m_candis[pre_idx].res_str, m_candis[cur_idx].tag_str);
		memcpy(m_candis[cur_idx].pre_idxs, m_candis[pre_idx].pre_idxs, sizeof(int)*MAX_TOKEN);
		m_candis[cur_idx].pre_idxs[m_candis[pre_idx].pre_idx_co] = pre_idx;
		m_candis[cur_idx].pre_idx_co=m_candis[pre_idx].pre_idx_co+1;
	}
	m_candi_co++;
	return cur_idx;
}

bool KMAnalyser::find_bound(char *str)
{
	if(!str || strlen(str) > MAX_WORD_LENGTH)
		return false;
	char *po = str;
	m_single = true;
	char *pp=strstr(str, "��");
	char let[5];
	int tail_val = -1;


	while (*po != 0)
	{
		if((unsigned int)*po < 0x80)
		{
			po+=1;
			continue;
		}
		if(pp && po>pp)
			break;
		strncpy(let, po, 4);
		let[4] = 0;
		DIC_KEY_VAL *p1 = (DIC_KEY_VAL*)bsearch(let, &g_to_word_list, TOWORD_COUNT, sizeof(DIC_KEY_VAL), compare_dic);
		if(p1)
		{
			if(strlen(str) == strlen(let) && proc_noun(str, NULL, T_NNC, -1, true))
				return true;
			if(p1->val == 1)
			{
				po += 4;
				continue;
			}
		}
		let[2] = 0;
		if(strlen(str) < 20 && po < str+2)
		{
			po += 2;
			continue;
		}
		else if(strlen(str) >= 20 && po < str + strlen(str)/2)
		{
			po += 2;
			continue;
		}
		tail_val = -1;

		if(po==str+strlen(str)-2 && strcmp(let, "̩")==0)
		{
			if(proc_noun(str, po, T_NNC, T_TCP))
				return true;
			tail_val = T_TCP;
		}
		else if(po==str+strlen(str)-2 && (strcmp(let, "��")==0 || strcmp(let, "��")==0))
		{
			if(proc_noun(str, po, T_NNC, T_TCS))
				return true;
			tail_val = T_TCS;
		}
		else if(po==str+strlen(str)-2 && (strcmp(let, "˾")==0 || strcmp(let, "��")==0))
		{
			if(proc_noun(str, po, T_NNC, T_TCO))
				return true;
			tail_val = T_TCO;
		}
		else if(po==str+strlen(str)-2 && (strcmp(let, "̮")==0 || strcmp(let, "��")==0))
		{
			if(proc_noun(str, po, T_NNC, T_TCC))
				return true;
			tail_val = T_TCC;
		}
		else if(po==str+strlen(str)-2 && (strcmp(let, "˼")==0 || strcmp(let, "��")==0 || strcmp(let, "��")==0 || strcmp(let, "��")==0))
		{
			if(proc_noun(str, po, T_NNC, T_TXP))
				return true;
			tail_val = T_TXP;
		}
		else if((po==str+strlen(str)-4 || po==str+strlen(str)-6) && strcmp(po, "���")==0)
		{
			if(proc_noun(str, po, T_NNC, T_TCD))
				return true;
			tail_val = T_TCD;
		}
		else if(((po==str+strlen(str)-4 || po==str+strlen(str)-6) && strncmp(po, "�溷", 4)==0) ||(po==str+strlen(str)-4  && strncmp(po, "˺��", 4)==0) || (po==str+strlen(str)-4  && strncmp(po, "���", 4)==0)) 
		{
			if(proc_noun(str, po, T_NNC, T_TCL))
				return true;
			tail_val = T_TCL;
		}
		else if((po==str+strlen(str)-4 || po==str+strlen(str)-2) && strncmp(po, "��", 2)==0)
		{
			if(proc_noun(str, po, T_NNC, T_TCL))
				return true;
			tail_val = T_TCL;
		}
		else
		{
			DIC_KEY_VAL *p2 = (DIC_KEY_VAL*)bsearch(let, &g_bound_list, BOUND_COUNT, sizeof(DIC_KEY_VAL), compare_dic);
			if(p2)
				tail_val = 0;
		}

		if(tail_val >= 0 && m_tail_co < 10)
		{
			char pre[3];
			strncpy(pre, po-2, 2);
			pre[2] = 0; 
			if(check_comb_to(pre, po))
			{
				m_tail_pos[m_tail_co]=po-str;
				m_tail_val[m_tail_co++]=tail_val;
			}
		}
		po+=2;
	}

	return false;
}

void KMAnalyser::proc_other(char *str)
{
	if(strlen(str) > MAX_WORD_LENGTH)
		return;

	m_end_co = 0;
	int cur_idx = -1;
	int is_suc=-1;
	bool is_end=false;
	char stem[500];
	char tail[500];
	int i;

	int tail_tag;
	if(m_pOptionSet->m_compNounRes && m_pOptionSet->m_inputMode == 1 && (proc_compound(str, 0, -1, &cur_idx, false, true)>0))
		return;

	for (i=0; i<m_tail_co; i++)
	{
		int len = m_tail_pos[i];
		tail_tag = m_tail_val[i];

		strncpy(stem, str, len);
		stem[len] = 0;
		strcpy(tail, str+len);
		if(strlen(tail) > 16)
			continue;

		if(m_pOptionSet->m_compNounRes && len > 4)
		{
			if(i==m_tail_co-1)
				is_suc = proc_compound(stem, 0, -1, &cur_idx, true, false, true);
			else
				is_suc = proc_compound(stem, 0, -1, &cur_idx, true, true ,true);
			if(is_suc == -1)
				//				if(is_suc != 1)
			{
				sprintf(stem, "%s[UNKNOWN]", stem);
				cur_idx = make_candi(stem, T_NNC, 0, -1);
			}
		}
		else if(check_mixed_word(stem))
			cur_idx = proc_mixed_word(stem, -1, true);
		else
		{
			sprintf(stem, "%s[UNKNOWN]", stem);
			cur_idx = make_candi(stem, T_NNC, 0, -1);
		}
		if(tail_tag > 0 && (is_suc>0 || !m_pOptionSet->m_compNounRes))
		{
			proc_noun(str, tail, -1, tail_tag, false, cur_idx);
			return;
		}
		else
		{
			analyse_word(tail, cur_idx, len, true);
			//			if(m_end_co > 0 && (is_suc>0 || i == m_tail_co-1 && is_suc==0))
			if(m_end_co > 0 )
				return;
		}
	}
	bool is_mixed = check_mixed_word(str);
	if(m_pOptionSet->m_compNounRes && !is_mixed && (proc_compound(str, 0, -1, &cur_idx, false, false)!= -1))
		return;

	if(is_mixed && proc_mixed_word(str)>=0)
		return;

	if(strcmp(str+strlen(str)-2, "\xb1\xc2")==0 || strcmp(str+strlen(str)-2, "\xc3\xc5")==0)
	{
		make_candi(str, T_ADV, 0, -1, true);
	}
	else
		proc_noun(str, NULL, T_NNC, -1, true);
}

/*
   void KMAnalyser::proc_other(char *str)
   {
   if(strlen(str) > MAX_WORD_LENGTH)
   return;

   m_end_co = 0;
   int cur_idx = -1;
   int is_suc=-1;
   bool is_end=false;
   char stem[500];
   char tail[500];
   int i;

   int tail_tag;
   if(m_pOptionSet->m_compNounRes && m_pOptionSet->m_inputMode == 1 && (proc_compound(str, 0, -1, &cur_idx, false, true)>0))
   return;

   for (i=0; i<m_tail_co; i++)
   {
   int len = m_tail_pos[i];
   tail_tag = m_tail_val[i];

   strncpy(stem, str, len);
   stem[len] = 0;
   strcpy(tail, str+len);
   if(strlen(tail) > 16)
   continue;

   if(m_pOptionSet->m_compNounRes)
   {
//			if(i==m_tail_co-1)
//				is_suc = proc_compound(stem, 0, -1, &cur_idx, true, false, true);
//			else
is_suc = proc_compound(stem, 0, -1, &cur_idx, true, true ,true);
//			if(is_suc == -1)
if(is_suc != 1)
{
sprintf(stem, "%s[UNKNOWN]", stem);
cur_idx = make_candi(stem, T_NNC, 0, -1);
}
}
else if(check_mixed_word(stem))
cur_idx = proc_mixed_word(stem, -1, true);
else
{
sprintf(stem, "%s[UNKNOWN]", stem);
cur_idx = make_candi(stem, T_NNC, 0, -1);
}
if(tail_tag > 0 && (is_suc>0 || !m_pOptionSet->m_compNounRes))
{
proc_noun(str, tail, -1, tail_tag, false, cur_idx);
return;
}
else
{
analyse_word(tail, cur_idx, len, true);
//			if(m_end_co > 0 && (is_suc>0 || i == m_tail_co-1 && is_suc==0))
if(m_end_co > 0 )
return;
}
}
bool is_mixed = check_mixed_word(str);
//	if(m_pOptionSet->m_compNounRes && !is_mixed && (proc_compound(str, 0, -1, &cur_idx, false, false)!= -1))
if(m_pOptionSet->m_compNounRes && !is_mixed && (proc_compound(str, 0, -1, &cur_idx, false, true)>0))
return;
//	if(m_pOptionSet->m_compNounRes && !is_mixed && (proc_compound(str, 0, -1, &cur_idx, false, true)== 1))
//		return;

//	if(!m_pOptionSet->m_compNounRes && is_mixed && proc_mixed_word(str))
if(is_mixed && proc_mixed_word(str)>=0)
return;

if(strcmp(str+strlen(str)-2, "\xb1\xc2")==0 || strcmp(str+strlen(str)-2, "\xc3\xc5")==0)
{
	make_candi(str, T_ADV, 0, -1, true);
}
else
proc_noun(str, NULL, T_NNC, -1, true);
}
*/
//////calculate weight/////

void KMAnalyser::compute_whole_path_weight(int eojolID)
{
	float initvalue, minv, tempvalue;
	int leftCnt, good = 0, min_mp = 0;
	KMA_RESULT* left, *right;
	if(eojolID == 0)
		leftCnt = 1;
	else
		leftCnt = m_pKmaSen->words[eojolID-1].nres;
	int i,j;
	for(i=0;i<m_pKmaSen->words[eojolID].nres;i++)
	{
		if(m_pKmaSen->words[eojolID].results[i].is_complete==false)
			continue;
		minv = 1000; 
		right = &(m_pKmaSen->words[eojolID].results[i]);
		//cout << "cqw debug : right " << right << endl;
		for(j=0;j<leftCnt;j++)
		{
			if(eojolID == 0)
			{	left = NULL;
				initvalue = 0;}
			else
			{	left = &(m_pKmaSen->words[eojolID-1].results[j]);
				initvalue = left->StateValue;
			}
			//cout << "cqw debug : init " << initvalue << endl;
			tempvalue = initvalue + m_pProbComputer->compute_elem_weight(left,right);
			//cout << "cqw debug : tmpval " << tempvalue << endl;
			if(minv > tempvalue)
			{	minv = tempvalue; good=j;}
		}

		//cout << "cqw debug : goodID " << good << endl;
		right->StateValue = minv;
		right->GoodPreID = good;
	}
}

int KMAnalyser::get_eojol_prob() //��������˧������
{
	// 	int  pretagid;
	float minv = 1000 ; //pr,spr, dpr, lgwei, 
	int id = 0, min_mp = 0, mode = 1;
	int x,i;
	for(x=0;x<m_word->nres;x++)
	{
		m_word->results[x].weight = m_pProbComputer->compute_elem_weight(NULL,&(m_word->results[x]));

		if(minv > m_word->results[x].weight)
		{	minv = m_word->results[x].weight; id=x;}
	}

	int lasttagid;
	for(i=0;i<m_word->nres;i++)
	{
		if(i==0)
			lasttagid = m_word->results[i].token_tag[m_word->results[i].ntoken-1];
		else
		{
			if(lasttagid != m_word->results[i].token_tag[m_word->results[i].ntoken-1])
			{	
				lasttagid = 0;	
				break;
			}
		}
	}
	pre_eojol_last_tagid = lasttagid;
	return id;
}

void KMAnalyser::print_anal_result()
{
	const char* tagName;
	if(m_pOptionSet->m_tag_mode==0)
		tagName = (const char*)tag_short_name;
	else
		tagName = (const char*)tag_name;

	int max_len = MAX_SEN_LENGTH*3;
	char *rBuff = m_res_buff;
	rBuff[0] = 0;

	if(m_pOptionSet->m_outputMode == 3)
		sprintf(rBuff, "[%s]\n", m_pInputSen);

	KMA_RESULT *p_result;
	int i,y;
	for(i=0; i<m_pKmaSen->wrdcnt; i++)
	{
		p_result = &(m_pKmaSen->words[i].results[m_pKmaSen->goodPath[i]]);
		if(p_result->is_complete == false)
			continue;

		change_result(i, -1, 1, NULL);

		if(m_pOptionSet->m_outputMode == 3)
		{
			if(i>0)
			{
				rBuff[strlen(rBuff)-1] = 0;
				strcat(rBuff,"\n");
			}
			strcat(rBuff,"\t");
		}
		for(y = 0;y<p_result->ntoken;y++)
		{
			if(strlen(rBuff) > max_len-100)
				break;
			if(p_result->token_tag[y]==-1)
				continue;
			if(m_pOptionSet->m_outputMode == 3)
				sprintf(rBuff+strlen(rBuff),"%s/%s+", p_result->token_str[y],
						tagName+(p_result->token_tag[y]*5));
			else if(m_pOptionSet->m_outputMode == 0)
				sprintf(rBuff+strlen(rBuff),"%s/%s+", p_result->token_str[y],
						tagName+(p_result->token_tag[y]*5));
			else if(m_pOptionSet->m_outputMode == 1)
				sprintf(rBuff+strlen(rBuff),"%s/%s ", p_result->token_str[y],
						tagName+(p_result->token_tag[y]*5));
			else if(m_pOptionSet->m_outputMode == 4)
				sprintf(rBuff+strlen(rBuff),"%s|%s ", p_result->token_str[y],
						tagName+(p_result->token_tag[y]*5));
			else if(m_pOptionSet->m_outputMode == 2){
				//cout << "cqw debug: rbuff " << rBuff << endl;
				sprintf(rBuff+strlen(rBuff),"%s ", p_result->token_str[y]);
			}
		}
	}
	rBuff[strlen(rBuff)-1] = 0;
	if(m_pOptionSet->m_add_letter == 1)
		change_result(-1,-1,4, rBuff);
	if(m_pOptionSet->m_outputMode==1 && m_pOptionSet->m_change_letter3==1)
		change_result(-1,-1, 2, rBuff);
	else if(m_pOptionSet->m_outputMode==4 && m_pOptionSet->m_change_letter3==1)
		change_result(-1,-1, 5, rBuff);
	else if(m_pOptionSet->m_outputMode==2 && m_pOptionSet->m_change_letter3==1)
		change_result(-1,-1, 3, rBuff);

	if(m_pOptionSet->m_outputMode == 3)
		strcat(rBuff, "\n");

	if(m_nOutputCodeConvMode != 0)
	{
		int len = strlen(rBuff);
		char* szTarget;
		if(m_nOutputCodeConvMode == L_K_UTF8)
			szTarget = (char*)malloc(len*3+1);
		else
			szTarget = (char*)malloc(len+1);
		*szTarget = 0;
		if(code_convert(rBuff, szTarget, m_nOutputCodeConvMode)>=0)
			strcpy(rBuff, szTarget);
		free(szTarget);
	}

}

void KMAnalyser::print_anal_multi_result()
{
	const char* tagName;
	if(m_pOptionSet->m_tag_mode == 0)
		tagName = (const char*)tag_short_name;
	else
		tagName = (const char*)tag_name;
	int max_len = MAX_SEN_LENGTH*3;

	char *rBuff = m_res_buff;
	rBuff[0] = 0;

	if(m_pOptionSet->m_outputMode == 3)
		sprintf(rBuff, "[%s]\n", m_pInputSen);

	KMA_RESULT *p_result;
	int i,y;
	for(i=0; i<m_pKmaSen->wrdcnt; i++)
	{
		for(int j=0; j<m_pKmaSen->words[i].nres; j++)
		{
			p_result = &(m_pKmaSen->words[i].results[j]);
			if(p_result->is_complete == false)
				continue;

			change_result(i, j, 1, NULL);

			if(i > 0)
				rBuff[strlen(rBuff)-1] = 0;

			strcat(rBuff,"\t");
			for(y = 0;y<p_result->ntoken;y++)
			{
				if(strlen(rBuff) > max_len-100)
					break;
				if(p_result->token_tag[y]==-1)
					continue;
				sprintf(rBuff+strlen(rBuff),"%s/%s+", p_result->token_str[y],
						tagName+(p_result->token_tag[y]*5));
			}
			rBuff[strlen(rBuff)-1] = 0;
			strcat(rBuff,"\n");		}
	}
	rBuff[strlen(rBuff)-1] = 0;
	if(m_pOptionSet->m_add_letter == 1)
		change_result(-1,-1,4, rBuff);
	if(m_pOptionSet->m_outputMode==1 && m_pOptionSet->m_change_letter3==1)
		change_result(-1,-1,2, rBuff);
	else if(m_pOptionSet->m_outputMode==4 && m_pOptionSet->m_change_letter3==1)
		change_result(-1,-1,5, rBuff);
	else if(m_pOptionSet->m_outputMode==2 && m_pOptionSet->m_change_letter3==1)
		change_result(-1,-1,3, rBuff);

	if(m_pOptionSet->m_outputMode == 3)
		strcat(rBuff, "\n");

	if(m_nOutputCodeConvMode != 0)
	{
		int len = strlen(rBuff);
		char* szTarget;
		if(m_nOutputCodeConvMode == L_K_UTF8)
			szTarget = (char*)malloc(len*3+1);
		else
			szTarget = (char*)malloc(len+1);
		*szTarget = 0;
		if(code_convert(rBuff, szTarget, m_nOutputCodeConvMode)>=0)
			strcpy(rBuff, szTarget);
		free(szTarget);
	}
}

int KMAnalyser::get_last_max_ID()
{
	int goodID = 0; float goodval = 1000;
	if(m_pKmaSen->words[m_pKmaSen->wrdcnt-1].nres == 1)
		return goodID;
	else
	{
		int y;
		for(y=0;y<m_pKmaSen->words[m_pKmaSen->wrdcnt-1].nres;y++)
		{
			if(m_pKmaSen->words[m_pKmaSen->wrdcnt-1].results[y].is_complete==false)
				continue;
			if(goodval > m_pKmaSen->words[m_pKmaSen->wrdcnt-1].results[y].StateValue)
			{	goodval = m_pKmaSen->words[m_pKmaSen->wrdcnt-1].results[y].StateValue;
				goodID = y;
			}
		}
	}
	return goodID;
}

int compareIDProb(const void* v1, const void* v2)
{
	ID_PROB* s1=(ID_PROB*)v1;
	ID_PROB* s2=(ID_PROB*)v2;
	return s1->prob-s2->prob;

}

int KMAnalyser::sort_path(int *res)
{
	ID_PROB path[MAX_RESULT];
	int goodID = 0; float goodval = 1000;
	int y, i=0;
	for(y=0;y<m_pKmaSen->words[m_pKmaSen->wrdcnt-1].nres;y++)
	{
		if(m_pKmaSen->words[m_pKmaSen->wrdcnt-1].results[y].is_complete==false)
			continue;
		path[i].id=y;
		path[i].prob=m_pKmaSen->words[m_pKmaSen->wrdcnt-1].results[y].StateValue;
		i ++;

	}
	qsort(path, i, sizeof(ID_PROB), compareIDProb);
	return i;
}


void KMAnalyser::get_good_path(int ID)
{
	int id = ID;
	int i;
	for(i=m_pKmaSen->wrdcnt-1 ; i!=-1 ; i--)
	{
		//cout << "cqw debug: ID " << id << endl;
		m_pKmaSen->goodPath[i] = id;
		id = m_pKmaSen->words[i].results[id].GoodPreID;
	}
}

//////////////

bool KMAnalyser::proc_error(char *str)
{
	if(str == NULL || strlen(str) == 0)
		return false;
	int len = strlen(str);
	char err_tail[3][10] = {"����", "����", "����"};
	int i,j,k;
	for (k=0; k<3; k++)
	{
		if(len <= (int)strlen(err_tail[k]) || len > 50)
			continue;
		if (strcmp(str+len-strlen(err_tail[k]), err_tail[k]) == 0)
		{
			char tail[50];
			sprintf(tail, "%s%s", "��", err_tail[k]);

			char noun[50];
			strncpy(noun, str, len-4);
			noun[len-4] = 0;

			analyse_word(tail, make_candi(noun, T_NNC, 0, -1), len-4);
			if(m_end_co > 0)
				return true;
		}
	}

	for (i = 0; i < m_dic_res_co; i++)
	{
		if( m_dic_res[i].tagInfo_num == 0 || m_dic_res[i].key == NULL || strncmp(m_input, m_dic_res[i].key, strlen(m_dic_res[i].key)) != 0 )
			continue;
		for (j = 0; j < m_dic_res[i].tagInfo_num; j++)
		{
			if(m_dic_res[i].tagInfo[j].tag_id == T_NNC)
			{
				char rest[100];
				int len = strlen(m_dic_res[i].key);
				if(strncmp(m_input + len, "\xb5\xb9", 2) == 0/*��*/ || strncmp(m_input + len, "\xb7\xb2", 2) == 0/*��*/ || strncmp(m_input + len, "\xb3\xde", 2) == 0/*��*/ )
				{
					sprintf(rest, "%s%s", "\xcb\xcb", m_input + len); //��
					analyse_word(rest, make_candi(m_dic_res[i].key, T_NNC, 0, -1), len);
					if(m_end_co > 0)
						return true;
					else
						break;
				}
			}
		}
	}
	return false;
}

void KMAnalyser::change_result(int wrd_id, int res_id, int kind, char *str)
{
	if(kind==1)//in eojol
	{	char temp[500];
		int tag_id;
		KMA_RESULT *pres;
		if(res_id == -1)
			pres = &(m_pKmaSen->words[wrd_id].results[m_pKmaSen->goodPath[wrd_id]]);
		else
			pres = &(m_pKmaSen->words[wrd_id].results[res_id]);
		for (int j = 0; j < pres->ntoken; j++)
		{
			tag_id = pres->token_tag[j];
			char *str = pres->token_str[j];
			char *punk = strstr(str, "[UNKNOWN]");
			if(punk)
				*punk = 0;
			strcpy(temp,str);
			if((tag_id==T_VVB||tag_id==T_VAJ) && m_pOptionSet->m_add_letter == 1)
			{
				strcat(str, "_VV");
			}
			if(m_pOptionSet->m_change_letter1 == 1)
			{
				if (tag_id == T_TDT && strcmp(str, "\xca\xbe") == 0)//ʾ->��
					strcpy(str, "\xca\xdd");	
				if (tag_id == T_TDS || tag_id == T_TDJ)
				{
					if(strcmp(str, "\xca\xad") == 0)	//ʭ->��
						strcpy(str, "\xca\xcc");
					else if(strcmp(str, "\xca\xad\xca\xbf") == 0)	//ʭʿ->��ʿ
						strcpy(str, "\xca\xcc\xca\xbf");
					else if(strcmp(str, "\xca\xad\xba\xb7") == 0)	//ʭ��->�̺�
						strcpy(str, "\xca\xcc\xba\xb7");
				}
			}
			// by hhj-31
			strcpy(temp,str);
			// end
			if(m_pOptionSet->m_change_letter2 == 1)
			{
				if ((tag_id >= T_TCS && tag_id<= T_TCC)||tag_id==T_TXP)
				{
					if (strcmp(str, "\xb0\xa1") == 0 || strcmp(str, "\xcb\xcb") == 0)	//��/��
						strcpy(str, "^2");
					else if (strcmp(str, "\xcb\xbc") == 0 || strcmp(str, "\xb2\xf7") == 0)	//˼/��
						strcpy(str, "^3");
					else if (strcmp(str, "\xcb\xbe") == 0 || strcmp(str, "\xb6\xa6") == 0)	//˾/��
						strcpy(str, "^7");
					else if (strcmp(str, "\xcc\xae") == 0 || strcmp(str, "\xb1\xe1") == 0)	//̮/��
						strcpy(str, "^4");
					else if (strcmp(str, "\xcb\xba\xb5\xe1") == 0 || strcmp(str, "\xb5\xe1") == 0)	//˺��/��
						strcpy(str, "^5\xb5\xe1");
					else if (strcmp(str, "\xcb\xba\xb7\xb0") == 0 || strcmp(str, "\xb7\xb0") == 0)	//˺��/��
						strcpy(str, "^5\xb7\xb0");
					else if (strcmp(str, "\xcb\xcb\xb5\xbb") == 0 || strcmp(str, "\xb5\xbb") == 0)	//�˵�/��
						strcpy(str, "^1\xb5\xbb");
					else if (strcmp(str, "\xcb\xcb\xb5\xc0") == 0 || strcmp(str, "\xb5\xc0") == 0)	//�˵�/��
						strcpy(str, "^1\xb5\xc0");
					else if (strcmp(str, "\xcb\xcb\xb5\xb9") == 0 || strcmp(str, "\xb5\xb9") == 0)	//�˵�/��
						strcpy(str, "^1\xb5\xb9");
					else if (strcmp(str, "\xcb\xcb\xb7\xb0") == 0 || strcmp(str, "\xb7\xb0") == 0)	//�˷�/��
						strcpy(str, "^1\xb7\xb0");
					else if (strcmp(str, "\xcb\xcb\xb4\xc7") == 0 || strcmp(str, "\xb4\xc7") == 0)	//�˴�/��
						strcpy(str, "^1\xb4\xc7");
					else if (strcmp(str, "\xa4\xa6\xb3\xa3\xb3\xde") == 0)//������->����
						strcpy(str, "\xa4\xa2\xb3\xde");
					else if (strcmp(str, "\xa4\xa6\xbb\xa4\xb3\xde") == 0)//������->����
						strcpy(str, "\xbb\xa4\xca\xef");
					else if (strcmp(str, "\xbb\xa1\xb3\xa3\xb3\xde") == 0)//������->��
						strcpy(str, "\xb3\xde");
					else if (strcmp(str, "\xbb\xa1\xb3\xa3\xc4\xb4") == 0)//����Ĵ->����
						strcpy(str, "\xb2\xf7\xb0\xa1");
					else if(tag_id==T_TCP&&m_pOptionSet->m_syn_letter==1)
						sprintf(str,"#2#%s",temp);
					// 					else if(tag_id==T_TCC)
					// 						sprintf(str,"#3#%s",temp);
					else if(tag_id==T_TXP&&m_pOptionSet->m_syn_letter==1)
						sprintf(str,"#6#%s",temp);
					else if(m_pOptionSet->m_syn_letter==1) //if(tag_id!=T_TCP)
						sprintf(str,"#1#%s",temp);
				}
				else if ((tag_id == T_TDT||tag_id == T_TDR)&&m_pOptionSet->m_syn_letter==1)
					sprintf(str,"#4#%s",temp);
				else if ((tag_id >= T_TDM && tag_id<= T_TEN)&&m_pOptionSet->m_syn_letter==1)
					sprintf(str,"#5#%s",temp);
				else if ((tag_id == T_ADV||tag_id == T_ADC||tag_id == T_PNN)&&m_pOptionSet->m_syn_letter==1)
					sprintf(str,"#7#%s",temp);
			}
			if (m_pOptionSet->m_delete_letter==1 && ((tag_id==T_TDR && strcmp(str, "��")==0) || (tag_id==T_TPL &&strcmp(str, "��")==0)))//del
				tag_id=-1;
			if(tag_id==T_NNX)
			{
				if(j-1>=0 && pres->token_tag[j-1]==T_NNC)
					pres->token_tag[j] = T_NNB;
				else
					pres->token_tag[j] = T_NNC;
			}
			else if(tag_id==T_NNB && m_pOptionSet->m_syn_letter==1)
				sprintf(str,"#9#%s",temp);
		}
	}
	else if (kind==2) 
	{
		if(str[0]==0)
			return;
		char ori_str[][50]={"��/TCD �ඣ/VVB ʭ/TDJ","��/TCD �ඣ/VVB �̺�/TDJ","��/TCD ̩��/VVB ��/TDJ","��/TCD ����/VVB ��/TDJ","��/TCD ������/VVB ��/TDJ","��/TCD ����/VVB ��/TDJ","˾/TCO ������/VVB ��/TDJ","��/TCD ����/VVB ��/TDJ","��/TCD �˶�/VVB ��/TDJ","��/TCD ����/VVB ��/TDJ","��/TCD ������/VVB ��/TDJ","^7/TCO ����/VVB �̺�/TDJ","^7/TCO ����/VVB ��/TDJ","^7/TCO ����/VVB ��/TDJ","^4/TCL ��/VAJ ��/TDV","^7/TCO ̡��/VVB ��/TDJ","^7/TCO ����/VVB ��/TDJ","^7/TCO ����/VVB ��/TDJ","^5/TCD ����/VVB ��/TDJ", "��/TDM+��/NNB+��/VAJ", NULL};
		char change_str[][50]={"��_�ඣ_ʭ/TDJ","��_�ඣ_ʭ��/TDJ","��_̩��_��/TDJ","��_̩��_ʭ/TDJ","��_����_��/TDJ","��_������_��/TDJ","��_����_��/TDJ","��_����_ʭ/TDJ","^7_������_��/TDJ","��_����_��/TDJ","��_����_ʭ/TDJ","��_�˶�_��/TDJ","��_����_��/TDJ","��_����_ʭ/TDJ","^7_����_ʭ��/TDJ","^7_����_��/TDJ","^7_����_ʭ/TDJ","^7_����_��/TDJ","^4_��_��/TDV","^4_��Ů/ADV","^7_̡��_��/TDJ","^7_̡��_ʭ/TDJ","^7_����_��/TDJ","^7_����_ʭ/TDJ","^7_����_��/TDJ","^5_����_��/TDJ","^5_����_ʭ/TDJ","��_������_��/TDJ","��_��_��/VAJ", NULL};
		for (int i=0; ori_str[i][0]!=0; i++)
		{
			char *p1 = strstr(str, ori_str[i]);
			if (p1)
			{
				char *p2 = p1+strlen(ori_str[i]);
				char tail[1000];
				strcpy(tail, p2);
				sprintf(p1, "%s%s", change_str[i], tail);
			}
		}
	}
	else if (kind==3) 
	{
		if(str[0]==0)
			return;
		char ori_str[][50]={"�� �ඣ ʭ","�� �ඣ �̺�","�� ̩�� ��","�� ���� ��","�� ������ ��","�� ���� ��","˾ ������ ��","�� ���� ��","�� �˶� ��","�� ���� ��","�� ������ ��","^7 ���� �̺�","^7 ���� ��","^7 ���� ��","^4 �� ��","^7 ̡�� ��","^7 ���� ��","^7 ���� ��","^5 ���� ��", "��+��+��", NULL};
		char change_str[][50]={"��_�ඣ_ʭ","��_�ඣ_ʭ��","��_̩��_��","��_̩��_ʭ","��_����_��","��_������_��","��_����_��","��_����_ʭ","^7_������_��","��_����_��","��_����_ʭ","��_�˶�_��","��_����_��","��_����_ʭ","^7_����_ʭ��","^7_����_��","^7_����_ʭ","^7_����_��","^4_��_��","^4_��Ů","^7_̡��_��","^7_̡��_ʭ","^7_����_��","^7_����_ʭ","^7_����_��","^5_����_��","^5_����_ʭ","��_������_��","��_��_��", NULL};
		for (int i=0; ori_str[i][0]!=0; i++)
		{
			char *p1 = strstr(str, ori_str[i]);
			if (p1)
			{
				char *p2 = p1+strlen(ori_str[i]);
				char tail[1000];
				strcpy(tail, p2);
				sprintf(p1, "%s%s", change_str[i], tail);
			}
		}
	}
	else if (kind==4) 
	{
		if(str[0]==0)
			return;
		char *p1 = strstr(str, "/NNC+��/SFV");
		if (p1)
		{
			char *p2 = p1+11;
			int len = strlen(p2);
			char * tail = (char*)malloc(len+2);
			strcpy(tail, p2);
			if(m_pOptionSet->m_add_letter == 1)
				sprintf(p1, "%s%s", "��_VV/VVB", tail);
			else
				sprintf(p1, "%s%s", "��/VVB", tail);
			free(tail);
		}
	}
	else if (kind==5) 
	{
		if(str[0]==0)
			return;
		char ori_str[][50]={"��|TCD �ඣ|VVB ʭ|TDJ","��|TCD �ඣ|VVB �̺�|TDJ","��|TCD ̩��|VVB ��|TDJ","��|TCD ����|VVB ��|TDJ","��|TCD ������|VVB ��|TDJ","��|TCD ����|VVB ��|TDJ","˾|TCO ������|VVB ��|TDJ","��|TCD ����|VVB ��|TDJ","��|TCD �˶�|VVB ��|TDJ","��|TCD ����|VVB ��|TDJ","��|TCD ������|VVB ��|TDJ","^7|TCO ����|VVB �̺�|TDJ","^7|TCO ����|VVB ��|TDJ","^7|TCO ����|VVB ��|TDJ","^4|TCL ��|VAJ ��|TDV","^7|TCO ̡��|VVB ��|TDJ","^7|TCO ����|VVB ��|TDJ","^7|TCO ����|VVB ��|TDJ","^5|TCD ����|VVB ��|TDJ", "��|TDM+��|NNB+��|VAJ", NULL};
		char change_str[][50]={"��_�ඣ_ʭ|TDJ","��_�ඣ_ʭ��|TDJ","��_̩��_��|TDJ","��_̩��_ʭ|TDJ","��_����_��|TDJ","��_������_��|TDJ","��_����_��|TDJ","��_����_ʭ|TDJ","^7_������_��|TDJ","��_����_��|TDJ","��_����_ʭ|TDJ","��_�˶�_��|TDJ","��_����_��|TDJ","��_����_ʭ|TDJ","^7_����_ʭ��|TDJ","^7_����_��|TDJ","^7_����_ʭ|TDJ","^7_����_��|TDJ","^4_��_��|TDV","^4_��Ů|ADV","^7_̡��_��|TDJ","^7_̡��_ʭ|TDJ","^7_����_��|TDJ","^7_����_ʭ|TDJ","^7_����_��|TDJ","^5_����_��|TDJ","^5_����_ʭ|TDJ","��_������_��|TDJ","��_��_��|VAJ", NULL};
		for (int i=0; ori_str[i][0]!=0; i++)
		{
			char *p1 = strstr(str, ori_str[i]);
			if (p1)
			{
				char *p2 = p1+strlen(ori_str[i]);
				char tail[1000];
				strcpy(tail, p2);
				sprintf(p1, "%s%s", change_str[i], tail);
			}
		}
	}


}

bool KMAnalyser::check_tail(char *str, int pre_tag, int pos)
{
	if(!str || strlen(str)==0)
		return true;
	int offset[MAX_WORD_LENGTH];
	int dic_res_id=-1, res_co;
	// by hhj-28
	if((dic_res_id = find_dic_res(str, pos, 2)) == -1)
		res_co = find_dic_res(str, pos, 0, offset);
	else
		res_co = strlen(m_dic_res[dic_res_id].key);
	// end
	int i,j;
	for (i=res_co-1; i>=0; i--)
	{
		if(offset[i]==-1)
			continue;
		if(dic_res_id != -1 &&  i < res_co-1)
			break;
		int len=0;
		if(dic_res_id == -1)
			len = offset[i];
		else
			len = res_co;

		char key1[100];
		strncpy(key1, str, len);
		key1[len] = 0;
		int res_idx = find_dic_res(key1, pos, 1);
		if(res_idx == -1)
			continue;
		for (j=0; j<m_dic_res[res_idx].tagInfo_num; j++)
		{
			int tag_id = m_dic_res[res_idx].tagInfo[j].tag_id;
			int tag_num = m_dic_res[res_idx].preInfo[j].tag_num;
			if((tag_id < T_TCS && tag_id!=T_NNC && tag_id!=T_VVB && tag_id!=T_VXV && tag_id!=T_VXA) || tag_id==T_SFN || tag_id==T_NNB)
				continue;
			if(check_comb(pre_tag, tag_id) && check_tail(str+len, tag_id, pos+len))
				return true;
		}			
		for (j=0; j<m_dic_res[res_idx].preInfo_num; j++)
		{
			int tag_id = m_dic_res[res_idx].preInfo[j].tags[0];
			int tag_num = m_dic_res[res_idx].preInfo[j].tag_num;
			if(tag_id < T_TCS || tag_id==T_SFN || tag_id==T_NNB)
				continue;
			if(check_comb(pre_tag, tag_id) && check_tail(str+len, m_dic_res[res_idx].preInfo[j].tags[tag_num-1], pos+len))
				return true;
		}			
	}
	return false;
}

bool KMAnalyser::check_comb_str(int tag1, int tag2, char *str1, char *str2, int level)
{
	if(check_comb(tag1, tag2)==false)
		return false;

	if(tag1 == T_NNC && tag2 == T_VVB)// && m_pOptionSet->m_compNounRes)
		return false;

	if(tag1==0 || str1==NULL)
		return true;
	int i=0;
	if(level==1)
	{
		if(tag1 != 0 && tag2 == T_VVB && strcmp(str2, "��")==0)
			return false;
		if((tag1==T_TDS||tag1==T_TDJ)&&(tag2==T_VVB||tag2==T_VAJ||tag2==T_VXV)&&(strcmp(str1, "ʭ")!=0 && strcmp(str1, "��")!=0 && strcmp(str1, "��")!=0))
			return false;
		if(tag1 == T_TDM && tag2 == T_NNB && ((!((strcmp(str1, "��") == 0 || strcmp(str1, "��") == 0 || strcmp(str1, "��") == 0) && strcmp(str2, "��") == 0)) && (!(strcmp(str1, "��") == 0 && strcmp(str2, "��") == 0)) && (!(strcmp(str1, "��") == 0 && strcmp(str2, "Ƿ") == 0)) && (!(strcmp(str1, "��") == 0 && strcmp(str2, "��") == 0)) && !((strcmp(str1, "��") == 0 && strcmp(str2, "��") == 0))))
			return false;
		if(tag1 == T_TDM && tag2 == T_TEN && (!(((strcmp(str1, "��") == 0 && strcmp(str2, "Ĵ") == 0)) || (strcmp(str1, "��") == 0 && strcmp(str2, "��") == 0) || (strcmp(str1, "��") == 0 && strcmp(str2, "��") == 0) || (strcmp(str1, "��") == 0 && strcmp(str2, "��") == 0))))
			return false;
		if(tag1 == T_TDJ && ( tag2 == T_VVB || tag2 == T_VAJ) && (strcmp(str1, "��") == 0 || strcmp(str1, "ʭ") == 0 || strcmp(str1, "��") == 0) )//&& strcmp(str2, "��") == 0)
			return false;
		if(tag1 == T_NNC && tag2 == T_TEN && !((strcmp(str2, "��") == 0) || (strcmp(str2, "��") == 0)))
			return false;
		if(tag1 == T_TDN && tag2 == T_VAJ && (strcmp(str1, "��") == 0))
			return false;
		if((tag1==T_TXP && tag2==T_TCP && str1 && str2 && !strcmp(str1, "��") && !strcmp(str2, "̩"))
				||(tag1==T_NNU && tag2==T_NNC && str1 && str2 && !strcmp(str1, "��") && !strcmp(str2, "�ݽ�"))
				||(tag1==T_TXP && tag2==T_TCL && str1 && str2 && !strcmp(str1, "��") && !strcmp(str2, "��"))
				||(tag1==T_NNC && tag2==T_NNX && str1 && str2 && !strcmp(str1, "��") && !strcmp(str2, "��"))
				||(tag1==T_TDJ && tag2==T_VVB && str1 && str2 && !strcmp(str1, "��") && !strcmp(str2, "��"))
				||(tag1==T_TDJ && tag2==T_VVB && str1 && str2 && !strcmp(str1, "��") && !strcmp(str2, "��")))
			return false;
		if(tag1==T_NNC && tag2==T_NNB && str2 && !find_tag(str2, T_NNB, NULL, 3))
			return false;
		if(tag1==T_NNC && tag2==T_SFN && str2  && !find_tag(str2, T_SFN, NULL, 2))
			return false;
		if(tag1==T_TXP && str1 && strcmp(str1, "��")==0)
			return false;
		if(tag1!=T_SUT && tag1!=T_SWT && tag1!=T_SQR && tag1!=T_SQL && tag2>=T_TCS && tag2<=T_TXP && !check_comb_to(str1, str2))
			return false;
		if((tag1==T_NNC ||tag1==T_SUT)&& str2 && strcmp(str2, "��")==0)
			return false;
		if(tag2==T_NNC && str1 && (strcmp(str1, "��")==0/*||strcmp(str1, "��")==0*/))
			return false;
		if(tag1==T_TDN && tag2==T_NNB && str1 && str2 && strcmp(str1, "��")==0 && strcmp(str2, "��")==0)/*strcmp(str2, "��")==0*/
			return false;
		if(tag1==T_PNN && tag2==T_NNU && str1 && strcmp(str1, "��")!=0)/*strcmp(str2, "��")==0*/
			return false;
		if(tag1==T_ADV && (tag2==T_VVB||tag2==T_VAJ) && str1 && strcmp(str1, "��")!=0)/*strcmp(str2, "��")==0*/
			return false;
		if(tag1==T_NNU && tag2==T_NNC && m_pOptionSet->m_inputMode==1)
			return false;
		if(str2 && tag2==T_TEN && strcmp(str2, "��")!=0 && m_pOptionSet->m_inputMode==1)
			return false;
		// 	 	if(tag1==T_NNC && tag2==T_VAJ/*(tag2==T_VVB||tag2==T_VAJ)*/ && strlen(str2)==2 && strcmp(str2, "��")!=0 && strcmp(str2, "��")!=0)/*strcmp(str2, "��")==0*/
		// 	 		return false;
		// 		if(str1 && strcmp(str1, "��")!=0 && tag2==T_TEN && m_sen_type!=sentence)
		// 			return false;
		if(tag1==T_NNC && tag2==T_VVB && str2)/*str && strlen(str) == 2 && */
		{
			DIC_KEY_VAL *p = (DIC_KEY_VAL*)bsearch(str2, &m_vi_dic, m_vi_dic_co, sizeof(DIC_KEY_VAL), compare_dic);
			if(p)
			{
				if(p->val==2) return true;
				else return false;
			}
			if(strlen(str2)==2)
				return false;
		}	
	}
	else if(level==2)
	{
	}
	return true;
}

bool KMAnalyser::check_comb_to(char *stem_str, char *to_str)//, char* eoganTag
{
	char str1[50];
	char str2[50];
	char stem_buff[MAX_TOKEN_LENGTH];
	char *pp;
	if((pp=strstr(stem_str, "[UNKNOWN]"))!=NULL)
	{
		strncpy(stem_buff, stem_str, pp-stem_str);
		stem_buff[pp-stem_str]=0;
	}
	else
		strcpy(stem_buff, stem_str);
	strcpy(str1, stem_buff+strlen(stem_buff)-2);
	strncpy(str2, to_str, 2);
	str2[2]=0;
	char to_sym[10];
	char stem_sym[10];
	if(!m_pZamo->decompose(str1, stem_sym))
		return true;
	m_pZamo->decompose(str2, to_sym);
	int stem_len = strlen(stem_sym);
	int to_len = strlen(to_sym);
	if(to_sym[0]=='h' && to_sym[1]=='A')// ʭ,ʾ
	{
		if(stem_sym[stem_len-1]=='Z' && stem_sym[stem_len-2]=='I')
		{
			if(stem_len>3 && stem_sym[stem_len-5]!='A' && stem_sym[stem_len-5]!='B' && stem_sym[stem_len-5]!='E')
				return false;
			else if (stem_len==3)
				return false;
		}
		else if(stem_sym[stem_len-2]!='A' && stem_sym[stem_len-2]!='B' && stem_sym[stem_len-2]!='E'
				&& stem_sym[stem_len-2]!='K' && stem_sym[stem_len-2]!='M')
		{	if(stem_sym[stem_len-1]=='Z' && stem_sym[stem_len-2]=='O' && stem_sym[stem_len-3]=='c')
			return true;
			else
				return false;
		}
		else if(stem_sym[stem_len-1]=='f' && (stem_sym[stem_len-2]=='A' && stem_sym[stem_len-3]!='i')	// !��
				||(stem_sym[stem_len-2]=='E' && stem_sym[stem_len-3]=='d'))							// ��
			return false;
	}
	else if(strcmp(stem_sym+strlen(stem_sym)-3,"nAZ") && to_sym[0]=='h' && to_sym[1]=='C')	//��,��
	{
		if(stem_sym[stem_len-1]=='Z' && stem_sym[stem_len-2]=='I')
		{	if(stem_len>3 && stem_sym[stem_len-5]!='C' && stem_sym[stem_len-5]!='G' && stem_sym[stem_len-5]!='I' && stem_sym[stem_len-5]!='J'
				&& stem_sym[stem_len-5]!='K' && stem_sym[stem_len-5]!='M' && stem_sym[stem_len-5]!='O' && stem_sym[stem_len-5]!='P')
			return false;
		}
		else if(stem_sym[stem_len-2]!='C' && stem_sym[stem_len-2]!='D' && stem_sym[stem_len-2]!='G'
				&& stem_sym[stem_len-2]!='I' && stem_sym[stem_len-2]!='J' && stem_sym[stem_len-2]!='K'
				&& stem_sym[stem_len-2]!='M' && stem_sym[stem_len-2]!='O' && stem_sym[stem_len-2]!='P')
		{
			if(stem_sym[stem_len-1]=='f' && (stem_sym[stem_len-2]=='A' && stem_sym[stem_len-3]!='i')
					||(stem_sym[stem_len-2]=='E' && stem_sym[stem_len-3]=='d'))
				return true;
			else
				return false;
		}
	}
	else if(to_len%3==1 && to_sym[0]=='d' && to_sym[1]=='d' && (to_sym[2]=='A'||to_sym[2]=='C'))	//����/����
	{
		if(stem_sym[stem_len-1]=='d' && (stem_sym[stem_len-2]=='A'||stem_sym[stem_len-2]=='C'))//stem_sym[stem_len-2]==cur_sym[2])
			return false;
	}
	// 	else if((to_sym[0]=='a'||to_sym[0]=='h') && to_sym[1]=='R' && to_sym[2]=='Z')	//̮/��
	// 	{
	// 		if((stem_sym[stem_len-1]!='Z' && to_sym[0]=='h')||(stem_sym[stem_len-1]=='Z' && to_sym[0]=='a'))
	// 			return false;
	// 	}
	// 	else if(to_len%3==0 && to_sym[0]=='d' && to_sym[1]=='A' && (to_sym[2]=='b'||to_sym[2]=='h'))	//��/��
	// 	{
	// 		if(stem_sym[stem_len-1]!='Z' )
	// 			return false;
	// 	}

	// 	if(stem_len%3==1)
	// 	{	if(to_sym[0]=='d' && stem_sym[stem_len-1]!='Z')
	// 			return false;
	// 	}

	if(((to_sym[0]=='a'&&to_sym[1]=='A')||(to_sym[0]=='h'&&to_sym[1]=='J'))&&to_sym[2]=='Z')// ��/��
	{
		if(((stem_sym[stem_len-1]!='Z'&&to_sym[0]=='a')||(stem_sym[stem_len-1]=='Z'&&to_sym[0]=='h')) && !(strlen(to_str) > 2 && (strncmp(to_str+2, "��", 2) == 0 || strncmp(to_str+2, "��", 2) == 0)))
			return false;
	}
	else if((to_sym[0]=='b'||to_sym[0]=='h')&&to_sym[1]=='I'&&to_sym[2]=='b')	//˼/��
	{
		if((stem_sym[stem_len-1]!='Z'&&to_sym[0]=='b')||(stem_sym[stem_len-1]=='Z'&&to_sym[0]=='h'))
			return false;
	}
	else if((to_sym[0]=='a'||to_sym[0]=='h')&&to_sym[1]=='R'&&to_sym[2]=='Z')	//̮/��
	{
		if((stem_sym[stem_len-1]!='Z'&&to_sym[0]=='h')||(stem_sym[stem_len-1]=='Z'&&to_sym[0]=='a'))
			return false;
	}
	else if((to_sym[0]=='d'||to_sym[0]=='h')&&to_sym[1]=='I'&&to_sym[2]=='d')	//	˾/��
	{
		if((stem_sym[stem_len-1]!='Z'&&to_sym[0]=='d')||(stem_sym[stem_len-1]=='Z'&&to_sym[0]=='h'))
			return false;
	}
	else if((to_sym[0]=='d'&&to_sym[1]=='A'&&(to_sym[2]=='h'||to_sym[2]=='b'||to_sym[2]=='Z'))//	��,��,��
			||(to_sym[0]=='b'&&to_sym[1]=='A'&&to_sym[2]=='Z')||(to_sym[0]=='e'&&to_sym[1]=='D'&&to_sym[2]=='Z')//	��,��
			||(to_sym[0]=='c'&&to_sym[1]=='I'&&to_sym[2]=='b'))		// ��
	{
		if(stem_sym[stem_len-1]!='Z')
			return false;
	}
	else if(to_sym[0]=='d'&&to_sym[1]=='E'&&to_sym[2]=='Z')	//	��
	{
		if(stem_sym[stem_len-1]!='d'&&stem_sym[stem_len-1]!='Z')
			return false;
	}
	else if(to_sym[0]=='h'&&to_sym[1]=='I'&&to_sym[2]=='Z')	//	˺
	{
		if(stem_sym[stem_len-1]=='d'||stem_sym[stem_len-1]=='Z')
			return false;
	}
	else if(to_sym[0]=='h'&&(to_sym[1]=='B'||to_sym[1]=='F')&&to_sym[2]=='Z')	//	ʿ,��
	{
		if(stem_sym[stem_len-1]!='Z')
			return false;
	}
	return true;
}

int KMAnalyser::proc_mixed_word(char *str, int pre_idx, bool is_sub, bool is_compound)
{
	int i;
	int cur_idx = pre_idx;
	int tag_id; int sym_len;
	int spos=-1;
	char str_sepa[MAX_TOKEN_LENGTH];
	int pre_sepa_idx = -1, sepa_pos = 0;
	for (i=0; i<m_sepa_co; i++)
	{
		if(m_sepa_flags[i]==0 || m_sepa_flags[i]==17)
		{
			spos = -1;
			if(strcmp(m_sepa_str[i], "��˼")==0)
			{
				cur_idx = make_candi(m_sepa_str[i], T_ADV, m_sepa_pos[i], cur_idx, false);
			}
			else
			{
				if(i>0 && m_sepa_flags[i-1]==1 && m_pCompoundAnalyer->match_num_unit(m_sepa_str[i-1], m_sepa_str[i]))
				{
					cur_idx = make_candi(m_sepa_str[i], T_NNU, m_sepa_pos[i], cur_idx, false);
				}
				else
				{

					analyse_word(m_sepa_str[i], cur_idx, m_sepa_pos[i]);
					if(m_end_co > 0)
					{
						//						if(i == m_sepa_co-1)
						//							return m_candi_co-1;
						//						else
						//						{
						cur_idx = m_end_idx[m_end_co-1];
						m_end_co = 0;
						//						}
					}
					else
					{
						if(i == m_sepa_co-1)
						{
							if(m_tail_val[m_tail_co-1]>0 && m_tail_pos[m_tail_co-1]<m_sepa_pos[i]+strlen(m_sepa_str[i]) && m_tail_pos[m_tail_co-1]>m_sepa_pos[i])
							{
								char szTemp[MAX_TOKEN_LENGTH];
								strncpy(szTemp, m_sepa_str[i],m_tail_pos[m_tail_co-1]-m_sepa_pos[i]);
								szTemp[m_tail_pos[m_tail_co-1]-m_sepa_pos[i]] = 0;
								cur_idx = make_candi(szTemp, T_NNC, m_sepa_pos[i], cur_idx, false, true);

								strcpy(szTemp, m_sepa_str[i]+(m_tail_pos[m_tail_co-1]-m_sepa_pos[i]));
								proc_noun(m_sepa_str[i], szTemp, -1, m_tail_val[m_tail_co-1], false, cur_idx);
								if(cur_idx >0 )
									return cur_idx;
							}
							else
							{
								if(m_pOptionSet->m_compNounRes && strlen(m_sepa_str[i])>4 && !is_compound)
								{
									int is_suc = -1;
									int end_idx;
									is_suc = proc_compound(m_sepa_str[i], m_sepa_pos[i], cur_idx, &end_idx, is_sub, false);
									if(is_suc == -1)
										cur_idx = make_candi(m_sepa_str[i], T_NNC, m_sepa_pos[i], cur_idx, !is_sub, true);
									else
										cur_idx = end_idx;
								}
								else
									cur_idx = make_candi(m_sepa_str[i], T_NNC, m_sepa_pos[i], cur_idx, !is_sub, true);
								//if(cur_idx >0 )
								//	return cur_idx;
							}
						}
						else
						{
							if(m_pOptionSet->m_compNounRes && strlen(m_sepa_str[i])>4 && !is_compound)
							{
								int is_suc = -1;
								int end_idx;
								is_suc = proc_compound(m_sepa_str[i], m_sepa_pos[i], cur_idx, &end_idx, true, false);
								if(is_suc == -1)
									cur_idx = make_candi(m_sepa_str[i], T_NNC, m_sepa_pos[i], cur_idx, false, true);
								else
									cur_idx = end_idx;
							}
							else
								cur_idx = make_candi(m_sepa_str[i], T_NNC, m_sepa_pos[i], cur_idx, false, true);
						}
					}
				}

			}
		}
		else
		{
			tag_id = m_SymProcer.get_symbol_tag(m_sepa_str[i], m_sepa_flags[i]);
			if(m_pOptionSet->m_sym_mode)
				cur_idx = make_candi(m_sepa_str[i], tag_id, m_sepa_pos[i], cur_idx, false);
			else
			{
				if(tag_id == T_SQL || tag_id == T_SQR)
				{
					sym_len = strlen(m_sepa_str[i]);
					cur_idx = make_candi(m_sepa_str[i], tag_id, m_sepa_pos[i], cur_idx, false);
					spos = -1;
				}
				else
				{
					if(spos == i-1 && i > 0)
					{
						m_candi_co --;
						strcat(str_sepa, m_sepa_str[i]);
						cur_idx = make_candi(str_sepa, T_SWT, sepa_pos, pre_sepa_idx, false);
					}
					else
					{
						strcpy(str_sepa, m_sepa_str[i]);
						pre_sepa_idx = cur_idx;
						sepa_pos = m_sepa_pos[i];
						if(tag_id != T_SLG)
							tag_id = T_SWT;
						cur_idx = make_candi(str_sepa, tag_id, m_sepa_pos[i], cur_idx, false);
					}
					spos = i;
				}
			}
		}
	}
	if(i == m_sepa_co && cur_idx >= 0 && !is_sub)
	{
		m_candis[cur_idx].is_end = true;
		m_end_idx[m_end_co++] = cur_idx;
		return cur_idx;
	}
	return cur_idx;
}

int KMAnalyser::proc_compound(char *str, int pos, int pre_idx, int *end_idx, bool is_sub, bool first_try, bool is_mixed)
{
	if(!str || strlen(str)==0 || strlen(str) > MAX_WORD_LENGTH)
		return true;
	bool is_end = false;
	char res_str[MAX_WORD_LENGTH*3];
	res_str[0]=0;
	bool res_co;
	int cur_idx = pre_idx;
	//	if((res_co=m_pCompoundAnalyer->analyse_word(str, res_str, first_try))>0)
	//	{
	res_co=m_pCompoundAnalyer->analyse_word(str, res_str, first_try);
	if(strlen(res_str) > 0)
	{
		StringTokenizer token(res_str, "+");
		char *po;int res_id=0;
		while (po=(char*)token.nextToken())
		{
			if(strlen(po) > MAX_TOKEN_LENGTH)
				continue;
			char *p1= strrchr(po, '/');
			if(p1==NULL) continue;
			int tag_id = get_tagid(p1+1);
			*p1=0;

			if(is_mixed && strlen(po)>2 && check_mixed_word(po))//tag_id > T_SCM)
			{
				cur_idx = proc_mixed_word(po, cur_idx, true, true);
			}
			else
				cur_idx = make_candi(po, tag_id, pos, cur_idx, false, true);

			if(cur_idx==-1)
				return -1;
			pos+=strlen(po);
			res_id++;
		}
		if(!is_sub && res_id > 0)
		{
			m_candis[cur_idx].is_end = true;
			m_end_idx[m_end_co++] = cur_idx;
		}
		*end_idx = cur_idx;
	}
	else if(check_mixed_word(str))
		*end_idx = proc_mixed_word(str, cur_idx, true);
	else
		return -1;
	return res_co;
}

bool KMAnalyser::find_tag(char *str, int tag_id, MorphRec *recInfo, int kind)
{
	int i;
	if(kind==1)
	{
		for (i=0; i<recInfo->tagInfo_num; i++)
		{
			if(recInfo->tagInfo[i].tag_id == tag_id)
				return true;
		}
	}
	else if(kind==2)
	{
		char usable_sfn[][10]={"��", "��", "��", "��", "��", "��", "��", "��", "��", "˦", "��", "����", "�պ���", "����", "�̾�", NULL};
		for (i=0;usable_sfn[i][0]!=0;i++)
		{
			if(strcmp(str, usable_sfn[i])==0)
				return true;
		}
	}
	else if (kind==3)
	{
		char usable_nnb[][5] = {"��","��","����","��","��","����","����","�ʶ�","����","��̡","�Ǻ�","�Ѷ�","��",
			"�д�","�к�","�м�","�ݵ�","���","����","����","����","���","��̡","ɸ","��","�޸�","����",
			"����","�˵�","��","Ƿ","��","��","����","��","��","��","��", NULL};
		for (i=0;usable_nnb[i][0]!=0;i++)
		{
			if(strcmp(str, usable_nnb[i])==0)
				return true;
		}
	}
	return false;
}

bool KMAnalyser::filter_result()
{
	// 	return false;
	int i, j;	
	m_word->nuse = m_word->nres;
	int res_co = m_word->nres;
	for (i=0; i<res_co; i++)
	{
		KMA_RESULT *pres = &m_word->results[i];
		char *p0 = pres->tag_list;
		char *po;
		if ((po=strstr(p0, "NNC+VVB+TDM+NNC")) && po==p0)
		{
			pres->is_complete = false;
			m_word->nuse--;
		}
		else if (((po=strstr(p0, "VVB+TDM+NNC"))||
					(po=strstr(p0, "VVB+TDM+NNB"))||
					(po=strstr(p0, "VVB+TDN+NNC"))||
					(po=strstr(p0, "VVB+TDN+NNB"))||
					(po=strstr(p0, "VXV+TDM+NNC"))||
					(po=strstr(p0, "NNC+VVB+TDM"))||
					(po=strstr(p0, "VXV+TDM+NNB"))) && po==p0)
		{
			char tokens[100];			
			int pos = m_candis[pres->candi_id[2]].pos+strlen(m_candis[pres->candi_id[2]].candi_str);
			strncpy(tokens, m_input, pos);
			tokens[pos] = 0;
			int res_id = find_dic_res(tokens, 0, 2, NULL);
			if(res_id!=-1 && find_tag(NULL, T_NNC, &m_dic_res[res_id], 1))
			{
				pres->is_complete = false;
				m_word->nuse--;
			}
		}
		else if(((po=strstr(p0, "VVB+TDN")) || (po=strstr(p0, "VAJ+TDN")) || (po=strstr(p0, "NNU+NNC"))) && po==p0)
		{
			char tokens[100];			
			int pos = m_candis[pres->candi_id[1]].pos+strlen(m_candis[pres->candi_id[1]].candi_str);
			strncpy(tokens, m_input, pos);
			tokens[pos] = 0;
			int res_id = find_dic_res(tokens, 0, 2, NULL);
			if(res_id!=-1 && find_tag(NULL, T_NNC, &m_dic_res[res_id], 1))
			{
				pres->is_complete = false;
				m_word->nuse--;
			}
		}
		else if ((po=strstr(p0, "VVB+TDS+VXV")) && po==p0)
		{
			char tokens[100];			
			int pos = m_candis[pres->candi_id[2]].pos+strlen(m_candis[pres->candi_id[2]].candi_str);
			strncpy(tokens, m_input, pos);
			tokens[pos] = 0;
			int res_id = find_dic_res(tokens, 0, 2, NULL);
			if(res_id!=-1 && find_tag(NULL, T_VVB, &m_dic_res[res_id], 1))
			{
				pres->is_complete = false;
				m_word->nuse--;
			}
		}
		else if (((po=strstr(p0, "NNC+NNX"))||
					(po=strstr(p0, "PNN+NNC"))||
					(po=strstr(p0, "NNX+NNC"))||
					(po=strstr(p0, "NNU+SFN"))||
					(po=strstr(p0, "NNC+NNB"))) && po==p0)
		{
			char tokens[100];
			sprintf(tokens, "%s%s", pres->token_str[0], pres->token_str[1]);
			int res_id = find_dic_res(tokens, 0, 2, NULL);
			if(res_id!=-1 && find_tag(NULL, T_NNC, &m_dic_res[res_id], 1))
			{
				pres->is_complete = false;
				m_word->nuse--;
			}
		}
		else if (((po=strstr(p0, "NNC+SFV")) || (po=strstr(p0, "NNC+SFA"))) && po==p0)
		{
			char tokens[100];
			sprintf(tokens, "%s%s", pres->token_str[0], pres->token_str[1]);
			int res_id = find_dic_res(tokens, 0, 2, NULL);
			if(res_id!=-1 && (find_tag(NULL, T_VVB, &m_dic_res[res_id], 1)||find_tag(NULL, T_VAJ, &m_dic_res[res_id], 1)))
			{
				pres->is_complete = false;
				m_word->nuse--;
			}
		}

		int verb_co = 0;
		for (j=0; j<pres->ntoken; j++)
		{
			if(pres->token_tag[j]==T_VVB || pres->token_tag[j]==T_VAJ)
				verb_co++;
		}
		if(verb_co>2)
		{
			pres->is_complete = false;
			m_word->nuse--;
		}
	}
	if(m_word->nuse == 0)
	{
		int best_id = 0;
		m_word->results[best_id].is_complete = true;
		return false;
	}
	return true;
}

bool KMAnalyser::check_mixed_word(char *str)
{
	m_sepa_co = 0;
	unsigned char *po = (unsigned char*)str;
	bool is_exist =false;
	int pp;
	while (*po != 0)
	{
		pp = *po;
		if(*po < 0xb0)
		{
			is_exist = true; break;
		}
		else
			po+=2;
	}
	if(!is_exist)
		return false;

	memset(m_sepa_pos, -1, sizeof(int)*20);	
	char res_str[MAX_WORD_LENGTH+50];	
	m_sepa_co = m_SymProcer.separate_sym(str, res_str, m_sepa_flags);
	if(m_sepa_co > 1 || (m_sepa_co==1 && m_sepa_flags[0]!=0 && m_sepa_flags[0]!=17))
	{
		po = (unsigned char*)res_str;
		int pos = 0;
		int i;
		for (i=0; i<m_sepa_co; i++)
		{
			strcpy(m_sepa_str[i], (char*)po);
			m_sepa_pos[i] = pos;
			pos+=strlen((char*)po);
			po+=strlen((char*)po)+1;
		}
		return true;
	}
	m_sepa_co=0;
	return false;
}

bool KMAnalyser::create(const char* ini_file)
{
	//cout << "creating analyser : " << ini_file << endl;
	m_pKmaSen = new KMA_SEN();
	m_pProbComputer = new CProbComputer();
	m_pCompoundAnalyer = new CompoundAnalysis();
	m_pOptionSet = new COptionSet();
	m_pZamo = new Zamo();
	m_pDic = new MorphDic();

	char fname[MAX_PATH];

	//cout << "creating analyser 2: " << ini_file << endl;
	if(!m_pOptionSet->load(ini_file))
		return false;

	//cout << "creating analyser 3: " << ini_file << endl;
	char buff[MAX_PATH];
	int len = strlen(m_pOptionSet->m_dicPath);
	strcpy(buff, m_pOptionSet->m_dicPath);
	if(len>0 && !(buff[len-1]=='/' || buff[len-1]=='\\'))
		strcat(buff, "/");

	//cout << "cqw debug : " <<  buff << endl;
	sprintf(fname, "%sHMM.mdl", buff);
	if(!m_pProbComputer->create(fname))
		return false;

	//cout << "creating analyser 4: " << ini_file << endl;
	if(!m_pCompoundAnalyer->create(buff))
		return false;

	//cout << "creating analyser 5: " << ini_file << endl;
	sprintf(fname, "%scode.txt", buff);
	if(!m_pZamo->create(fname))
		return false;

#ifdef _DB
	if(!m_pDic->create(""))
		return false;
#else
	//cout << "creating analyser 6: " << ini_file << endl;
	sprintf(fname, "%skma_dic", buff);
	if(!m_pDic->create(fname))
		return false;
#endif
	if(!load_assist_dic(buff, 0))
		return false;
	if(m_pOptionSet->m_code_input != 1 || m_pOptionSet->m_code_output!=1)
	{
		sprintf(fname, "%sMLLocalCodeU.tbl", buff);
		m_pUnicode = new KUnicode();
		cout << "creating analyser 6: " << ini_file << endl;
		if(!m_pUnicode->SetCodeTableFile(fname))
			return false;
		switch(m_pOptionSet->m_code_input)
		{
			case 2:
				m_nInputCodeConvMode = L_UTF8_K;
				break;
			case 3:
				m_nInputCodeConvMode = L_KS_K;
				break;
			default:
				m_nInputCodeConvMode = 0;
				break;
		}
		switch(m_pOptionSet->m_code_output)
		{
			case 2:
				m_nOutputCodeConvMode = L_K_UTF8;
				break;
			case 3:
				m_nOutputCodeConvMode = L_K_KS;
				break;
			default:
				m_nOutputCodeConvMode = 0;
				break;
		}
	}
	return true; 
}

int KMAnalyser::code_convert(char* szSource, char* szTarget, int nConvMode)
{
	if(szSource == NULL || szTarget == NULL)
		return -1;
	int len;

	if(nConvMode == L_K_UTF8)
		len = m_pUnicode->GetKorUTF8CodeString((const char*)szSource, (unsigned char*)szTarget, 0);
	else if(nConvMode == L_UTF8_K)
		len = m_pUnicode->GetUTF8_KorCodeString((unsigned char*)szSource, (unsigned char*)szTarget);
	else
		len = m_pUnicode->GetKorConvString((const char*)szSource, (char*)szTarget, nConvMode);
	//printf("[code_convert:%d]\n",len);
	return len;
}

bool KMAnalyser::check_indep_word(char *str)
{
	MorphRec recInfo;
#ifdef _DB
	m_pDic->search(str, &recInfo, true);
#else
	m_pDic->search(str, &recInfo, false);
#endif
	int i;
	for (int idx1=0; idx1<recInfo.preInfo_num; idx1++)
	{
		if(recInfo.is_compound==false)
			continue;
		int cur_idx = -1;
		int cur_pos = 0;
		bool is_sub_end =false;
		for (int idx2=0; idx2<recInfo.preInfo[idx1].tag_num; idx2++)
		{
			if(idx2==recInfo.preInfo[idx1].tag_num-1)
				is_sub_end=true;
			cur_idx=make_candi(recInfo.preInfo[idx1].morph_str[idx2], recInfo.preInfo[idx1].tags[idx2], cur_pos, cur_idx, is_sub_end, true);
			cur_pos+=strlen(recInfo.preInfo[idx1].morph_str[idx2]);
		}
		make_result(1);
		if(m_word->nres > 0)
			return true;
	}
	for (i=0; i<recInfo.tagInfo_num; i++)
	{
		if(recInfo.tagInfo[i].tag_id==T_ADV || recInfo.tagInfo[i].tag_id==T_PNN || recInfo.tagInfo[i].tag_id==T_ADC || recInfo.tagInfo[i].tag_id==T_NNC)
		{
			make_candi(str, recInfo.tagInfo[i].tag_id, 0, -1, true);
		}
		make_result(1);
		if(m_word->nres > 0)
			return true;
	}
	return false;
}

bool KMAnalyser::analyse_xr(char *str)
{
	if(str==NULL)
		return false;
	int i,j;
	int offset[MAX_WORD_LENGTH];
	int xr_co = m_xr_dic.search_all(str, offset);
	for (i=xr_co-1; i>=0; i--)
	{
		if(offset[i]==-1)
			continue;
		int len = offset[i];
		char key[100];
		strncpy(key, str, len);
		key[len] = 0;
		char next_letter[5];
		strncpy(next_letter, str+len, 2);
		next_letter[2]=0;
		char tail[100];
		tail[0] = 0;
		if(!m_xr_dic.search(key, tail))
			continue;
		char *p0 = strstr(tail, "#");
		if(p0==NULL || strstr(p0, (char*)next_letter)==NULL)
			continue;
		char bound_letter[5];
		strncpy(bound_letter, tail, 2);
		bound_letter[2]=0;
		analyse_word(str+len, -1, len);
		if(m_end_co == 0)
			continue;
		bool is_succss = false;
		for (j=0; j<m_end_co; j++)
		{
			int first_id = m_candis[m_end_idx[j]].pre_idxs[0];
			int tag_id = m_candis[first_id].tag_id;
			if((tag_id != T_VBT && tag_id != T_VAT && tag_id != T_VXT)||m_candis[first_id].pre_idx != -1||strcmp(m_candis[first_id].candi_str, bound_letter)!=0)
			{
				m_end_idx[j] = -1;
				continue;
			}
			char key1[100];
			sprintf(key1, "%s%s", key, m_candis[first_id].candi_str);
			MorphRec recInfo;
			if(m_pDic->search(key1, &recInfo, true) == false || recInfo.tagInfo_num == 0)
				continue;
			int new_tag_id = -1;
			if(m_candis[first_id].tag_id==T_VBT && find_tag(NULL, T_VVB, &recInfo, 1))
				new_tag_id = T_VVB;
			else if (m_candis[first_id].tag_id==T_VAT && find_tag(NULL, T_VAJ, &recInfo, 1))
				new_tag_id = T_VAJ;
			else if (m_candis[first_id].tag_id==T_VXT && find_tag(NULL, T_VXV, &recInfo, 1))
				new_tag_id = T_VXV;
			if(new_tag_id != -1)
			{
				m_candis[first_id].tag_id = new_tag_id;
				strcpy(m_candis[first_id].candi_str, key1);
				m_candis[first_id].pos = 0;
				is_succss = true;
			}
			else
				m_end_idx[j] = -1;
			if(is_succss)
			{
				if(strcmp(str, "����")==0 )
					make_candi(str, T_NNC, 0, -1, true);
				else if(strcmp(str, "�ݳ���")==0)
					make_candi(str, T_ADV, 0, -1, true);
				make_result(2);
				return true/*filter_res(3) & filter_res(2)*/;
			}
		}
	}
	return false;
}

int KMAnalyser::make_sub_result(char *str, char *tag_list)
{
	StringTokenizer token(str, "+");
	char *po; int k=0;
	char po1[MAX_TOKEN_LENGTH*2];
	po1[0] = 0;
	while (po = (char*)token.nextToken())
	{
		char *p1 = strrchr(po, '/');
		if(p1==NULL)
		{
			strcat(po1, po);
			strcat(po1, "+");
			continue;
		}
		else
			strcat(po1, po);
		p1 = strrchr(po1, '/');
		m_word->results[m_word->nres].token_tag[k] = get_tagid(p1+1);
		if(k==0)
			strcpy(tag_list, p1+1);
		else
			sprintf(tag_list, "%s+%s", m_word->results[m_word->nres].tag_list, p1+1);
		*p1 = 0;
		char *pp = po1;
		while (*pp!=0)
		{
			if(strncmp(pp, "��", 2)==0)
				sprintf(pp, "+%s", pp+2);
			pp++;
		}
		strcpy(m_word->results[m_word->nres].token_str[k], po1);			
		k++;
		po1[0] = 0;
		if(k==MAX_TOKEN)
			break;
	}
	return k;
}

bool KMAnalyser::proc_noun(char *str1, char *str2, int tag1, int tag2, bool is_noun, int cur_idx)
{
	if(strlen(str1) == 0 || strlen(str1) > MAX_TOKEN_LENGTH || str2 != NULL && strlen(str2) == 0 || tag1 == -1 && cur_idx == -1)
		return false;
	int ntoken = 0;
	int n;

	char* szNNX;
	if(str2 == NULL)
	{
		n = strlen(str1);
		szNNX = str1+n-2;
	}
	else
	{
		n = strlen(str1)-strlen(str2);
		szNNX = str1+n-2;
	}
	char szTemp[MAX_TOKEN_LENGTH];
	strncpy(szTemp, str1, n);
	szTemp[n]=0;
	int candi_idx = cur_idx;

	// process noun
	if((strncmp(szNNX, "��", 2)==0 || strncmp(szNNX, "̩", 2)==0) && szNNX>=str1+2 && !is_noun)
	{
		ntoken = 0;
		if(strncmp(szNNX-2, "ʭ", 2)==0 || strncmp(szNNX-2, "��", 2)==0 || strncmp(szNNX-2, "��", 2)==0)
			return false;
		else
		{
			DIC_KEY_VAL *p = NULL;
			if(strncmp(szNNX, "��", 2)==0)
			{
				strncpy(szTemp, str1, n-2);
				szTemp[n-2]=0;
				p = (DIC_KEY_VAL*)bsearch(szTemp, &g_nnp_list, NNP_COUNT, sizeof(DIC_KEY_VAL), compare_dic);
				if(p)
					candi_idx = make_candi(szTemp, T_NNP, 0, candi_idx, false);
			}
			if(!p && n > 6 && m_pOptionSet->m_compNounRes)
			{
				analyse_word(szTemp, -1, 0, false, true);
				if(m_end_co > 0)
				{
					candi_idx = m_candi_co -1;
					m_candis[candi_idx].is_end=false;
					m_end_co = 0;
				}
				else if(!proc_compound(szTemp, 0, -1, &candi_idx, true))
					candi_idx = make_candi(szTemp, T_NNC, 0, candi_idx, false);
			}
			else if(!p)
				candi_idx = make_candi(szTemp, T_NNC, 0, candi_idx, false);

			if(strncmp(szNNX, "��", 2)==0)
				candi_idx = make_candi("��", T_TPL, n-2, candi_idx, false);
		}
	}
	else if(strncmp(szNNX, "��", 2)==0 && szNNX == str1 && !is_noun)
		candi_idx = make_candi("��", T_NNB, 0, candi_idx, false);
	else if(is_noun)
	{
		if(tag2 == -1)
		{
			candi_idx = make_candi(str1, T_NNC, 0, candi_idx, true);
			return true;
		}
		else
			candi_idx = make_candi(szTemp, T_NNC, 0, candi_idx, false);
	}
	else if(tag1 != -1)
	{
		DIC_KEY_VAL *p = (DIC_KEY_VAL*)bsearch(szTemp, &g_nnp_list, NNP_COUNT, sizeof(DIC_KEY_VAL), compare_dic);
		if(p)
			candi_idx = make_candi(szTemp, T_NNP, 0, candi_idx, false);
	}
	if(candi_idx == -1)
		return false;
	// process to
	if((tag2 == T_TCL || tag2 == T_TCD) && strlen(str2) == 6)// �溷��, �溷��, ��´�
	{
		strncpy(szTemp, str2, 4);
		szTemp[4] = 0;
		candi_idx = make_candi(szTemp, tag2, n, candi_idx, false);
		strcpy(szTemp, str2+4);
		if(strcmp(szTemp, "̩") == 0)
			candi_idx = make_candi(szTemp, T_TCP, n+4, candi_idx, true);
		else
			candi_idx = make_candi(szTemp, T_TXP, n+4, candi_idx, true);
	}
	else if(tag2 == T_TCL && strlen(str2) == 4) //���, �洪
	{
		if(strcmp(str2, "�溷") == 0 || strcmp(str2, "˺��") == 0 || strcmp(str2, "���") == 0)
			candi_idx = make_candi(str2, tag2, n, candi_idx, true);
		else
		{
			strncpy(szTemp, str2, 2);
			szTemp[2] = 0;
			candi_idx = make_candi(szTemp, tag2, n, candi_idx, false);
			strcpy(szTemp, str2+2);
			candi_idx = make_candi(szTemp, T_TXP, n+2, candi_idx, true);
		}
	}
	else
	{
		candi_idx = make_candi(str2, tag2, n, candi_idx, true);
	}
	return true;
}

int KMAnalyser::get_num_str(char *str)
{
	int i = 0;
	unsigned char *str1 = (unsigned char *)str;
	int len = strlen(str);
	while(i < len)
	{	if(str1[i] >= 0x30 && str1[i] <= 0x39) 
		i++;
		else
			break;
	}

	return i;
}
