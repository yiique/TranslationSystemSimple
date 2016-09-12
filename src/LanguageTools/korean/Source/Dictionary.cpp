#include "Stdafx_engine.h"
#include "Dictionary.h"
#include "Tag.h"	
#include "StringTokenizer.h"
#ifdef _DB
#include "c:/mysql/include/mysql.h"
extern MYSQL *myData;
#endif

void trim(char *str);

MorphRec::MorphRec()
{
	tagInfo_num = 0;
	preInfo_num = 0;
}
	
int MorphRec::serialize(char *buff)
{
	char *po = buff;
	*po = tagInfo_num; po++;
	int i, j, k;
	for(i=0; i< tagInfo_num; i++)
	{	
		*po = tagInfo[i].tag_id; po++;
		*po = tagInfo[i].verb_adj; po++;
	}
	*po = preInfo_num; po++;
	for (j=0; j<preInfo_num; j++)
	{
		*po = preInfo[j].tag_num; po++;
		for (k=0; k<preInfo[j].tag_num; k++)
		{
			*po = preInfo[j].tags[k]; po++;
			if(preInfo[j].morph_str[k])
				strcpy(po, preInfo[j].morph_str[k]);
			else
				*po = 0;
			po += strlen(po) + 1;
		}
		if(preInfo[j].index_str)
			strcpy(po, preInfo[j].index_str);
		else
			*po = 0;
		po += strlen(po) + 1;
	}
	return po - buff;
}

void MorphRec::load(char *buff)
{
	char *po= buff;
	tagInfo_num = *po; po++;
	int i, j, k;
	is_compound=false;
	for(i=0; i< tagInfo_num; i++)
	{
		tagInfo[i].tag_id = *po; po++;
		tagInfo[i].verb_adj = *po; po++;
		if(i == 15)
			break;
	}
	preInfo_num = *po; po++;
	for (j=0; j<preInfo_num; j++)
	{
		preInfo[j].tag_num = *po; po++;
		for (k=0; k<preInfo[j].tag_num; k++)
		{
			preInfo[j].tags[k] = *po; po++;
			if (strlen(po)>0)
				strcpy(preInfo[j].morph_str[k], po);
			else
				strcpy(preInfo[j].morph_str[k], "");

			po += strlen(po) + 1;
			if(k == 10)
				break;
		}
		if(strlen(po)>0)
			strcpy(preInfo[j].index_str, po);
		else
			strcpy(preInfo[j].index_str, "");
		po += strlen(po) + 1;
		if(j == 20)
			break;
	}
	if(preInfo_num==1 && tagInfo_num==1 && tagInfo[0].tag_id==T_NNC)
	{
		int tag = preInfo[0].tags[preInfo[0].tag_num-1];
		if(tag >= T_NNC && tag<=T_NNU || tag==T_SFN || tag==T_TDN && preInfo[0].tags[0]==T_NNC)
			is_compound = true;
	}

}

bool MorphRec::fromString(char *buff, bool kind)
{
	if (buff == NULL || strlen(buff) == 0)
		return false;
	trim(buff);
	int i = 0;
	is_compound = false;
	if (kind)
	{
		tagInfo_num = 0;
		char *token = strtok(buff, ", ");
		while (token)
		{
			tagInfo[i].tag_id = get_tagid(token);
			i++;
			if(i ==15)
				break;
			token = strtok(NULL, ", ");
		}
		tagInfo_num = i;
	}
	else
	{
		preInfo_num = 0;
		StringTokenizer token(buff, ", ");
		char *pp;
		while (pp = (char*)token.nextToken())
		{
			strcpy(preInfo[i].index_str, "");
			char str[100];
			strcpy(str, pp);
			char *p1 = strstr(str, "(");
			char *p2 = strstr(str, ")");
			if((p1 && p2 == NULL) || (p2 && p1 == NULL))
				return false;
			if(p1 && p2)
			{
				*p2 = 0;
				strcpy(preInfo[i].index_str, p1+1);
				*p1 = 0;
			}
			int j = 0;
			char *token2 = strtok(str, "+ ");
			while (token2)
			{
				char str2[100];
				strcpy(str2, token2);
				char *po = strstr(str2, "/");
				if(po == NULL)
					return false;
				*po = 0;
				po++;
				if(strlen(str2) == 0)
					return false;
				strcpy(preInfo[i].morph_str[j], str2);
				preInfo[i].tags[j] = get_tagid(po);
				j++;
				if(j == 10)
					break;
				token2 = strtok(NULL, "+ ");
			}
			preInfo[i].tag_num = j;
			if(i == 20)
				break;
			i++;
		}
		preInfo_num = i;
	}
	if(preInfo_num==1 && tagInfo_num==1 && tagInfo[0].tag_id==T_NNC)
	{
		int tag = preInfo[0].tags[preInfo[0].tag_num-1];
		if(tag >= T_NNC && tag<=T_NNU || tag==T_SFN || tag==T_TDN)
			is_compound = true;
	}
	return true;
}

MorphDic::MorphDic()
{
	m_fp = NULL;
}

MorphDic::~MorphDic()
{
	if(m_fp != NULL)
		fclose(m_fp);
}

bool MorphDic::create(char *fname)
{
#ifdef _DB
	if(!trie.create(fname, OLD_TRIE, true))
		return false;
	return true;
#else
	if(!trie.create(fname, OLD_TRIE))
		return false;
#endif
	char buff[1024];
	sprintf(buff, "%s.dat", fname);
	m_fp = fopen(buff, "rb");
	if(m_fp == NULL)
		return false;
	return true;
}

int  MorphDic::search_all(char *str, int *offsets)
{
	return trie.search_longest(str, offsets, strlen(str), true);
}

bool  MorphDic::search(char *key, MorphRec *res, bool is_josa)
{
	strcpy(res->key, key);
	res->noun_freq = 0;
	#ifdef _DB
		char szSQL[1000];
		MYSQL_RES *res_sql;
		MYSQL_ROW row;
		sprintf(szSQL, "select tag, result from dictionary where name='%s'", key);
		if (!mysql_query(myData, szSQL))
		{
			res_sql = mysql_store_result(myData);
			if (row = mysql_fetch_row(res_sql))
			{
				if(row[0])
					res->fromString(row[0], true);
				else
					res->tagInfo_num = 0;

				if(row[1])
					res->fromString(row[1], false);
				else
					res->preInfo_num = 0;
			}
			mysql_free_result(res_sql);
		}
		return true;
	#else
		char buff[500];
		int poin;
		if((poin = trie.search(key)) < 0)
			return false;
		fseek(m_fp, poin, 0);
		short len;
		fread(&len, 2, 1, m_fp);
		fread(buff, len, 1, m_fp);
		res->load(buff);
		return true;
	#endif
}

ProbDic::ProbDic()
{
	m_fp = NULL;
}

ProbDic::~ProbDic()
{
	if(m_fp != NULL)
		fclose(m_fp);
}

bool ProbDic::create(char *fname)
{
	if(!trie.create(fname, OLD_TRIE))
		return false;
	char buff[1024];
	sprintf(buff, "%s.dat", fname);
	m_fp = fopen(buff, "rb");
	if(m_fp == NULL)
		return false;
	return true;
}

bool ProbDic::search(char *key, float *val)
{
	int poin;
	if((poin = trie.search(key)) < 0)
		return false;
	fseek(m_fp, poin, 0);
	short len;
	fread(&len, 2, 1, m_fp);
	char buff[50];
	fread(buff, len, 1, m_fp);
	*val = (float)atof(buff);
	return true;
}

XrDic::XrDic()
{
	m_fp = NULL;
}

XrDic::~XrDic()
{
	if(m_fp)
		fclose(m_fp);
}

bool XrDic::create(char *fname)
{
	if(!trie_xr.create(fname, OLD_TRIE))
		return false;
	char buff[1024];
	sprintf(buff, "%s.dat", fname);
	m_fp = fopen(buff, "rb");
	if(m_fp == NULL)
		return false;
	return true;
}

int XrDic::search_all(char *key, int *offset)
{
	return trie_xr.search_longest(key, offset, strlen(key), true);
}

bool XrDic::search(char *key, char *tail_str)
{
	int poin;
	if((poin = trie_xr.search(key)) < 0)
		return false;
	fseek(m_fp, poin, 0);
	short len;
	fread(&len, 2, 1, m_fp);
	char buff[100];
	fread(buff, len, 1, m_fp);
	buff[len]=0;
	strcpy(tail_str, buff);
	return true;
}
