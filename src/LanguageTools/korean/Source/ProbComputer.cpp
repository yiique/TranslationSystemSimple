// ProbComputer.cpp: implementation of the CProbComputer class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx_engine.h"
#include "ProbComputer.h"
#include <math.h>
#include <iostream>
using namespace std;
#include "Tag.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef _DB
#include "C:/mysql/include/mysql.h"
extern MYSQL *myData;
#endif
extern KMA_SEN g_kmasen;
// extern BiTagSet g_bitag;
// extern UniTagSet g_unitag;

CProbComputer::CProbComputer()
{
}

CProbComputer::~CProbComputer()
{

}
bool CProbComputer::create(char *path)
{
	FILE *fp = fopen(path, "rt");
	if(fp==NULL)
		return false;
	char buff[100];
	m_prob_model_co=0;
	while (fgets(buff, 100, fp))
	{
		buff[strlen(buff) -1] = 0;
		char *p1 = strstr(buff, "\t");
		if(p1==NULL)
			continue;
		m_prob_model[m_prob_model_co].prob = atof(p1+1);
		*p1=0;
		strcpy(m_prob_model[m_prob_model_co].key, buff);
		m_prob_model_co++;
		
	}
	fclose(fp);
	if(m_prob_model_co > 0)
		return true;
	return false;
// 	return	m_prob_dic.create(path);
}

float CProbComputer::compute_elem_weight(KMA_RESULT* left, KMA_RESULT* right)
{
	float pr, dpr, lgwei;//, minv = 1000 ;

	int mode = 1; // maxCnt, 
	int pretagid;
// 	if(!left)
// 		maxCnt = 1;
// 	else
// 		maxCnt = right->ntoken;
	dpr = 1;
	lgwei=0;
	if(!left)
		pretagid = 0;
	else
		pretagid = left->token_tag[left->ntoken-1];// 		pretagid = left->candis[left->ncandi-1]->tag_id;

	//cout << "cqw debug : ntoken " << right->ntoken << endl;
	for(int y=0;y<right->ntoken;y++)
	{

#ifdef _DB
		pr = get_elem_prob_DB((char*)tag_name[pretagid],(char*)tag_name[right->token_tag[y]],
				right->token_str[y], mode);
#else
		pr = get_elem_prob((char*)tag_name[pretagid], (char*)tag_name[right->token_tag[y]],right->token_str[y], mode);
#endif

		right->token_prob[y] = pr;

		if(y == 0 && right->ntoken != 1 && (pretagid==T_TDM ||pretagid==T_NNC)// (pretagid==T_ETM ||pretagid==T_NNC) 
			&& right->token_tag[0]==T_NNC 
			&& (right->token_tag[1]==T_SFV || right->token_tag[1]==T_VVB ))
			pr += 100;
		lgwei += pr;
		//cout << "cqw debug : pretag " << pretagid << endl;
		//cout << "cqw debug : logwei " << lgwei << endl;

		pretagid = right->token_tag[y];
		mode = 2;
	}
	return lgwei;
}	


float CProbComputer::get_elem_prob(char *prevtag, char *curtag, char *curmorp, int mode)
{
	float v = get_prob_val(prevtag, curtag, curmorp, mode);
	if(v == 100)
	{
		v = get_prob_val(prevtag, curtag, NULL, mode) ;
	}
	return v;
}

int compare(const void *v1, const void *v2);

float CProbComputer::get_prob_val(char *pre_tag, char *cur_tag, char *cur_mop, int mode)
{
	char key[100];
	float val = 0.0;
	if(cur_mop && !strstr(cur_mop, "[UNKNOWN]"))
	{
		if(!strlen(pre_tag))
			sprintf(key, "BEGIN/%s/%s/%d", cur_tag, cur_mop, mode);
		else
			sprintf(key, "%s/%s/%s/%d", pre_tag, cur_tag, cur_mop, mode);
		KEY_VAL *p = (KEY_VAL*)bsearch(key, m_prob_model, m_prob_model_co, sizeof(KEY_VAL), compare);
		if(p)
			val = p->prob;
		else
			return 100;
// 		if(!m_prob_dic.search(key, &val))
// 			return 100;
	}
	else
	{
		if(!strlen(pre_tag))
			sprintf(key, "BEGIN/%s/%d", cur_tag, mode);
		else
			sprintf(key, "%s/%s/%d", pre_tag, cur_tag, mode);
		KEY_VAL *p = (KEY_VAL*)bsearch(key, m_prob_model, m_prob_model_co, sizeof(KEY_VAL), compare);
		if(p)
			val = p->prob;
		else
			return 100;
// 		if(!m_prob_dic.search(key, &val))
// 			return 100;
	}
	return val;
}

#ifdef _DB
float CProbComputer::get_elem_prob_DB(char* prevtag, char* curtag, char* curmorp, int mode)
{
	float prob, a, b;
	char szSQL[500];
	int n_fttag, n_fdtag, n_curtag, n_bitag, n_moptag;
	MYSQL_RES *res;
	MYSQL_ROW row;
	if(!strlen(prevtag))
	{	n_fttag = 1820; n_fdtag = 1820;}
	else
	{	sprintf(szSQL,"select * from uni_tag_freq where tag = '%s' ", prevtag);

		if(mysql_query(myData,szSQL) == 0)
		{
			res = mysql_store_result(myData);
			if (res->row_count)
			{
				row = mysql_fetch_row( res );
				n_fttag = atoi(row[1]) ;
				n_fdtag = atoi(row[2]) ;
			}
			else
			{	n_fttag = 0; n_fdtag = 0; }
			mysql_free_result(res);
		}
	}
	sprintf(szSQL,"select freq1 from uni_tag_freq where tag ='%s'", curtag);
	if(mysql_query(myData,szSQL) == 0)
	{
		res = mysql_store_result(myData);
		if (res->row_count)
		{
			row = mysql_fetch_row( res );
			n_curtag = atoi(row[0]) ;
		}
		else
			n_curtag = 0;
		mysql_free_result(res);
	}

	if(strlen(prevtag))
		sprintf(szSQL,"select freq from bi_tag_freq where previous = '%s' and next = '%s' and flag = '%d'", prevtag, curtag, mode);
	else
		sprintf(szSQL,"select freq from bi_tag_freq where previous = 'BEGIN' and next = '%s'", curtag);
	if(mysql_query(myData,szSQL) == 0)
	{
		res = mysql_store_result(myData);
		if (res->row_count)
		{
			row = mysql_fetch_row( res );
			n_bitag = atoi(row[0]) ;
		}
		else
			n_bitag = 0;
		mysql_free_result(res);
	}

	sprintf(szSQL,"select freq from co_tag_morphem where tag ='%s' and morphem = '%s'", curtag,curmorp);
	if(mysql_query(myData,szSQL) == 0)
	{
		res = mysql_store_result(myData);
		if (res->row_count)
		{
			row = mysql_fetch_row( res );
			n_moptag = atoi(row[0])+3 ;
		}
		else
			n_moptag = 1;
		mysql_free_result(res);
	}
	if(n_fttag==0||n_curtag==0||n_bitag==0)
		return 100;
	else if(mode == 1)
	{
		a = (float)n_bitag/n_fdtag;
		a = -(float)log10(a);
		b = (float)n_moptag/n_curtag;
		b = -(float)log10(b);		//(3*n_moptag)
 		prob = (float)(5*a + 3*b);///10)  ;
	}
	else if(mode == 2)
	{
		a = (float)n_bitag/(n_fttag-n_fdtag);//(7*n_bitag)
		a = -(float)log10(a);
		b = (float)n_moptag/n_curtag;
		b = -(float)log10(b);		//(3*n_moptag)
 		prob = (float)(2*a + 3*b);///10)  ;
	}

	return prob;
}//*/
#endif
