/***********************************************************************
CompoundAnalysis.cpp  2011-12-24 written by Ri Hyon Sul 
Program for Analyzing Korean Compound Noun
***********************************************************************/



#include "Stdafx_engine.h"
#include "CompoundAnalysis.h"
#include "math.h"

#define     IS_2BYTECODE(c)      ((unsigned char )c>0x80 ? true : false) 

CompoundAnalysis::CompoundAnalysis()
{
	m_pUnit = NULL;
}
bool  CompoundAnalysis::create(char *path)
{	

	m_file_simple_info = NULL;

	char fname[1024];
	sprintf(fname, "%ssimple_noun", path);
	if(!m_trie_simple.create(fname, OLD_TRIE))
		return false;

	sprintf(fname, "%ssimple_info.dat", path);
	if(!(m_file_simple_info = fopen(fname, "rb")))
		return false;
	sprintf(fname, "%ssimple_pair", path);
	if(!m_trie_pair.create(fname, OLD_TRIE))
		return false;
	sprintf(fname, "%sunit_noun.txt", path);
	return load_unit(fname);
}

int comp_strpoin(const void *p1, const void *p2)
{
	char *v1 = *(char **)p1;
	char *v2 = *(char **)p2;
	return strcmp(v1, v2);
}

int comp_str_strpoin(const void *p1, const void *p2)
{
	char *v1 = (char *)p1;
	char *v2 = *(char **)p2;
	return strcmp(v1, v2);
}

bool  CompoundAnalysis::load_unit(char *fname)
{
	FILE *fp = fopen(fname, "rb");
	m_pUnit = NULL;

	if(fp == NULL)
		return false;
	fseek(fp, 0, 2);
	int len = ftell(fp);
	fseek(fp, 0, 0);
	m_pUnit = (char *)malloc(len + 10);
	fread(m_pUnit, 1, len, fp);
	fclose(fp);
	m_countUnit = 0;
	char *token = strtok(m_pUnit, "\r\n");
	int i = 0;
	while(token != NULL && i< 200)
	{
		m_poinUnit[i] = token;
		token = strtok(NULL, "\r\n");
		i++;
	}
	m_countUnit = i;
	qsort(m_poinUnit, m_countUnit, sizeof(char *), comp_strpoin);
	return true;

}

CompoundAnalysis::~CompoundAnalysis()
{
	if(m_pUnit != NULL)  free(m_pUnit);
	if(m_file_simple_info != NULL)
		fclose(m_file_simple_info);

}

char  *CompoundAnalysis::str(int i)
{
	if(i < 0 || i >= m_simple_set.count)
		return NULL;
	return m_simple_set.sn[i].word;
}
int  CompoundAnalysis::find_bigger(int index, int len)
{
	for(int i = 0; i< m_head_count[index]; i++)
	{
		if(m_simple_set.sn[m_head_words[index][i]].len == len)
			return m_head_words[index][i];
	}
	return -1;
}
bool  CompoundAnalysis::analyse_word(char  *token, char *result, bool first_try)
{	
	char token0[1000];
	strcpy(token0, token);
	*result = 0;
//	strcpy(token0, "¸êÀôË±¸ËºÏºÂ°¬");

//	strcpy(token0, "½è´ªË¦¼è°éËË´´ÀÒ»¦¾Ë±Ë");
//	strcpy(token0, "Ãäµáº¼±Ëº¤ÊÕÃù±¨¼è");//º¼±Ëº¤ÊÕÃù±¨¼è°¼º¼º¡ÊÕ½úÃù¸ó°Ö");
	m_simple_set.clear();
	int k = 0;

	if((strlen(token0) <= 2) || strlen(token0)>=50)
	{	
		if(is_number(token0))
			sprintf(result, "%s/SNM", token0); 
		else if(is_english(token0))
			sprintf(result, "%s/SLG", token0); 
		else
			sprintf(result, "%s/NNC", token0); 
		return  false;
	}


	if(strncmp(token0, "±®ËÎºÂ", 6) == 0 || strncmp(token0, "±®¼³ËÎ", 6) == 0 ||
		strncmp(token0, "±®¼³ºä", 6) == 0 || strncmp(token0, "±®Âô¼é", 6) == 0 ||
		strncmp(token0, "±®¼³Ë¼", 6) == 0)
	{
		strncpy(result, token0, 6);
		result[6] = 0;
		if(strlen(token0) > 6)
		{	strncpy(result, token0, 6);
			strcpy(result+6, "/NNR+");
			bool nn = analyse_word(token0 + 6, result + 6 + 5, first_try);
			if(first_try && !nn)
				*result = 0;
			return nn;
		}
		sprintf(result, "%s/NNR", token0);
		return true;
	}


	TERM_LIST tl_set[1000];
	int n = all_group(token0, tl_set, 1000, first_try);

	if(n == 0 && first_try)
			return false;
	
	
	int mm = 0;
	bool is_valid;
	if(n >= 1)
		mm = decide(tl_set, n, is_valid);
	if(first_try && !is_valid)
		return 0;
	char *po = result;
	*po = 0;
	int mmm = mm;
	int count = 0;
//	for(mm = 0; mm<n; mm++)
//	{
	int cur_index = 0;

	for(int i = 0; i< tl_set[mm].count; i++)
	{	
		bool sw = false;
		if(i < tl_set[mm].count-1)
		{
			int big_n= find_bigger(cur_index, m_simple_set.sn[tl_set[mm].terms[i]].len + m_simple_set.sn[tl_set[mm].terms[i+1]].len);
			if(big_n >= 0)
			{	sw = true;
				tl_set[mm].terms[i] = big_n;
			}
		}
	
		if(i != 0)
		{	*po = '+';
			po++;
		}

		if(!first_try && m_simple_set.sn[tl_set[mm].terms[i]].kind != 1)
		{
			int ii = 0;
			while(i+ii < tl_set[mm].count && m_simple_set.sn[tl_set[mm].terms[i+ii]].len == 2 && m_simple_set.sn[tl_set[mm].terms[i+ii]].kind <=1)
				ii++;
			if(ii > 1)
			{	
				*po = 0;
				int count_common = 0;
				for(int j = 0; j < ii; j++)
					strcat(po, m_simple_set.sn[tl_set[mm].terms[i+j]].word);	
					
				strcat(po, "/NNC");
				po = po + strlen(po);
				i = i+ii-1;
				continue;
			}

		}


		char tag[8][5] = {"NNC", "SFN", "ADJ", "NNC", "SNM", "SLG", "SWT", "NNR"};
		char *t = tag[0];
		if(m_simple_set.sn[tl_set[mm].terms[i]].kind == 2)
		{	t = tag[2];
			strcpy(po, m_simple_set.sn[tl_set[mm].terms[i]].info);
		}
		else if(m_simple_set.sn[tl_set[mm].terms[i]].kind == 3)
		{	
			char hh[100];
			char tt[3];
			char *p = str(tl_set[mm].terms[i]);
			strncpy(hh, p, strlen(p) -2);
			hh[strlen(p) -2] = 0;
			strcpy(tt, p + (strlen(p) -2));
			
			sprintf(po, "%s/%s+%s/%s", hh, tag[0], tt, tag[1]);
		}
		else 
		{	int k = m_simple_set.sn[tl_set[mm].terms[i]].kind;
			if(k > 7)
				k = 0;
			sprintf(po, "%s/%s",str(tl_set[mm].terms[i]),tag[k]);
		}
		po += strlen(po);
		cur_index += m_simple_set.sn[tl_set[mm].terms[i]].len;
		count++;
		if(sw)
			i++;
	}
//	strcpy(po, "\r\n");
//	po+=strlen(po);
//	}
	*po = 0;
	return is_valid;
}


char *CompoundAnalysis::get_larger(char *left, char *center, char *right)
{
	bool left_cross = false;
	bool right_cross = false;	
	if(left != NULL && strlen(left) > 50 || center != NULL && strlen(center) > 50 || right != NULL && strlen(right) > 50)
		return NULL;
	char b1[100]; *b1 = 0;
	char b2[100]; *b2 = 0;

	if(left != NULL)
	{	sprintf(b1, "%s%s", left, center);
		if(m_simple_set.search(b1) >= 0)
			left_cross = true;
	}
	if(right != NULL)
	{	sprintf(b2, "%s%s", center, right);
		if(m_simple_set.search(b2) >= 0)
			right_cross = true;
	}
	if(left_cross && !right_cross)
		return left;
	return NULL;
}

typedef  struct
{
	int  count;
	int  terms[25];
	short  occ[25];
	char  len[25];  
	int  index;
	bool comb_match;
	char  comb_match_count;
	int  error;
	double average;
}TERM_LIST0;

int comp_tl_1(const  void *v1, const  void *v2);

bool CompoundAnalysis::match_num_unit(char *num, char *unit)
{
	
	if(is_num_sym(num) && is_unit_noun(unit))
		return true;
	return false;
}

bool CompoundAnalysis::is_comb_match(TERM_LIST *p, int &match_count)
{
	char buff[100];
//	bool  pre_match = false; 
	match_count = 0;
	int n=0;
	for(int i = 1; i< p->count; i++)
	{
		if(m_simple_set.search_pair(p->terms[i-1], p->terms[i], this))
			n++;
/*		{	

			if(!pre_match)
				return false;
			pre_match = false;
		}
		else
		{	//pre_match = true; 
			match_count++;
		}
*/
	}
//	return pre_match;
	match_count = p->count - n -1;
	return (match_count == 0);
}

int low_fre_num(TERM_LIST0  *v)
{	
	int m = 0;
	int i;
	for(i = 0; i< v->count; i++)
	{	
		if(v->len[i] > 2 && v->occ[i] < 3 || v->len[i] <= 2 && v->occ[i] < 10)
			m++;
	}
	return m;
}

double geometric_average(TERM_LIST0  *v)
{	
	double m = 1.;
	int i;
	for(i = 0; i< v->count; i++)
	{	
		m *= v->occ[i];
	}
	m = pow(m, 1.0/(double)(v->count));
	return m;
}

int CompoundAnalysis::cal_count(int *words, int count, bool error)
{
	int n = 0;
/*	for(int i = 0; i< count; i++)
		if(m_simple_set.sn[words[i]].kind == 1)
			n--;
*/	if(!error)
		return count;	
	for(int i = 0; i< count; i++)
		if(m_simple_set.sn[words[i]].kind == 0 && m_simple_set.sn[words[i]].len == 2 && m_simple_set.sn[words[i]].occ == 1)
			n++;
	return n;
}


int CompoundAnalysis::decide(TERM_LIST  *tl_set, int n, bool &is_valid)
{
	TERM_LIST0 *temp = (TERM_LIST0 *)malloc(sizeof(TERM_LIST0) * n);
	for(int i=0; i<n; i++)
	{	memcpy(&temp[i], &tl_set[i], sizeof(TERM_LIST));
		for(int j = 0; j< temp[i].count; j++)
		{	temp[i].occ[j] = m_simple_set.sn[temp[i].terms[j]].occ;
			if(temp[i].occ[j]==0)
				temp[i].occ[j] = 1;
		}
		temp[i].index = i;
		temp[i].count = cal_count(temp[i].terms, temp[i].count);
		int match_count = 0;
		temp[i].comb_match = is_comb_match(&tl_set[i], match_count);
		temp[i].comb_match_count = match_count;
		temp[i].error = cal_count(temp[i].terms, temp[i].count, true);
//		temp[i].comb_match = 1;
//		temp[i].comb_match_count = 1;

		temp[i].average = geometric_average(&temp[i]);
	}
	qsort(temp, n, sizeof(TERM_LIST0), comp_tl_1);
	int m= temp[0].index;

//	is_valid = temp[0].count == 1 || temp[0].comb_match_count == temp[0].count - 1 || 
	is_valid = temp[0].count == 1 || //temp[0].comb_match_count == temp[0].count - 1 || 
		temp[0].average > 5.0 && temp[0].error == 0;
	free(temp);
	return m;
}

int min_occ(TERM_LIST0  *v)
{
	int m = 100;
	for(int i = 0; i< v->count; i++)
	{	
		if(v->occ[i] < m)
			m = v->occ[i];
	}
	return m;
}


int comp_tl_1(const  void *v1, const  void *v2)
{
	TERM_LIST0  *p1 = (TERM_LIST0 *)v1;
	TERM_LIST0  *p2 = (TERM_LIST0 *)v2;

//	return (int)(geometric_average(p2) - geometric_average(p1));	

	int n;

	n = p2->comb_match - p1->comb_match;

	if(n!=0)
		return n;

	
	n = p1->comb_match_count - p2->comb_match_count;
	if(n!=0)
		return n;

	if(min_occ(p1) >= 5 && min_occ(p2)>= 5)
	{	n = p1->count - p2->count;
		if(n != 0)
			return n;
	}
	n = p1->error - p2->error;
	if(n!=0)
		return n;


	n = (int)(p2->average - p1->average);	
	return n;
}


bool valid_boarder(char *str, int len)
{
	if(len> 0 && str[len-1] > ' ' && str[len-1] <= 'z' && str[len] > ' ' && str[len] <= 'z')
		return false;
	return true;
}


int  CompoundAnalysis::head_terms(char *str, int *res)
{
	int n[100];

	int count = m_trie_simple.search_longest(str, n, strlen(str));
	int k = 0;
	for(int i =0; i< count; i++)
	{	if(n[i] >= 0 && valid_boarder(str, i+1))
		{	
			char bu[100];
			strncpy(bu, str, i+1);
			bu[i+1] = 0;
			if((res[k] = m_simple_set.add(bu, this)) >= 0)
				k++;
		}
	}

	return k;
}

void CompoundAnalysis::reverse(int *vals, int count)
{	if(count==0)
		return;
	int n=count/2+count%2;

	int temp;
	for(int i=0;i<n;i++)
	{	temp=vals[i];
		vals[i]=vals[count-1-i];
		vals[count-1-i]=temp;
	}
}


char *CompoundAnalysis::head_word(char *str, int n)
{	
	strncpy(m_head,str,n);
	m_head[n]=0;
	return m_head;
}

char *CompoundAnalysis::mid_word(char *str, int start, int n)
{	
	
	strncpy(m_head,str+ start,n);
	m_head[n]=0;
	return m_head;
}

void CompoundAnalysis::filter()
{
//	return;
	for(int index = 1; index< m_word_len; index++)
	{
		for(int m = 0; m< m_head_count[index]; m++)
		for(int n = 0; n< m_end_count[index]; n++)
		{	
			if(m_simple_set.sn[m_end_words[index][n]].len < 4 || m_simple_set.sn[m_head_words[index][m]].len < 4 )
				continue;
//			if(m_simple_set.sn[m_end_words[index][n]].len < 100 || m_simple_set.sn[m_head_words[index][m]].len < 100 )
//				continue;

			if(m_simple_set.search_pair(m_end_words[index][n],m_head_words[index][m], this))
			{	m_head_words[index][0] = m_head_words[index][m];
				m_head_count[index] = 1;
				int index1 = index - m_simple_set.sn[m_end_words[index][n]].len;
				int index2 = index + m_simple_set.sn[m_head_words[index][m]].len;
				int k;
				for(k = 0; k<m_head_count[index1]; k++)
				{
					if(m_head_words[index1][k] == m_end_words[index][n])
					{	m_head_words[index1][0] = m_head_words[index1][k];
						m_head_count[index1] = 1;
						break;
					}	
				}
				for(k = index1+1;k<index; k++)
					m_head_count[k] = 0;
		
				for(k = index+1;k<index2; k++)
					m_head_count[k] = 0;


			}
		}
	}
}
int CompoundAnalysis::all_group(char *w, TERM_LIST *tl_set, int max_num, bool first_try)
{	
	if(strlen(w) == 0 || strlen(w) >= MAX_WORD_LEN)
		return 0;	

	if(max_num==0)
		return 0;
	m_word_len = strlen(w);
	strcpy(m_word, w);
	int i;
	for(i=0; i< m_word_len+1; i++)
	{	m_head_count[i] = 0;
		m_end_count[i] = 0;
		m_head_first_child[i] = -100;
	}

	all_search(0, first_try);
//	filter();
	m_cur_node = 0;
	m_start_node = make_tree(0);
	if(m_start_node < 0)
		return 0;
	for(i = 0; i< max_num; i++)
		tl_set[i].count = 0;
	int  n = all_group(m_start_node, tl_set, max_num, first_try);
	for(i = 0; i< n; i++)
	{
		reverse(tl_set[i].terms, tl_set[i].count);
	}
	return n;
}


int CompoundAnalysis::add_node(int word, int child, int brother)
{
	if(m_cur_node > 490)
		return -1;
	m_nodes[m_cur_node].word  = word;
	m_nodes[m_cur_node].first_child  = child;
	m_nodes[m_cur_node].next_brother  = brother;
	m_cur_node++;
	return m_cur_node-1;
}
int CompoundAnalysis::make_tree(int index) // return first child node
{

	if(m_head_first_child[index] != -100)
		return m_head_first_child[index];
	if(index >= m_word_len)
		return -1;
	if(m_cur_node > 5490)
		return -1;
	if(m_head_count[index] == 0)
		return -2;

	int old_node = -1;
	int cc = 0;
	for(int i = m_head_count[index]-1; i>= 0; i--)
	{	
		int n = make_tree(index + m_simple_set.sn[m_head_words[index][i]].len);
		if(n == -2)
			continue;
		old_node = add_node(m_head_words[index][i], n, old_node);
		cc++;
	}

	if(cc == 0)
		old_node = -2;
	m_head_first_child[index] = old_node;
	return old_node;
}

void CompoundAnalysis::add_word(int index, int word)
{
	if(word < 0)
		return;
	m_head_words[index][m_head_count[index]]=word;
	m_head_count[index]++;
	int end = index + m_simple_set.sn[word].len;
	m_end_words[end][m_end_count[end]] = word;
	m_end_count[end]++;
}

bool CompoundAnalysis::is_suffix(char*str, int index)
{

	char suf[9][3] = {"Âô", "Ãù", "Ë¦", "µù", "µü", "ºÂ", "»¥", "¼¬", "»¤"};
	for(int i = 0; i<9; i++)
	{	if(strncmp(str+index, suf[i], 2) == 0 && (i != 8 || index <strlen(str) -2))
			return true;
	}
	return false;
}

bool CompoundAnalysis::all_search(int index, bool first_try)
{
	if(m_head_count[index] > 0)
		return true;

	bool hh = 0;
	char *w = m_word;
	
	if(index> 0 && is_suffix(w, index))
	{	add_word(index, m_simple_set.add(mid_word(w, index, 2), 1, 1000)); 
		all_search(index + 2, first_try);
		hh = 1;
	}

	int res[100];
	int n=head_terms(w+index, res);
		
	if(n == 0)
	{	int len = 0;
		int tag = -1;
		int occ = 100;
		if(index == 0 && strncmp(w, "½£", 2) == 0 && strlen(w) > 3 && w[2] >= '1' && w[2] <= '9')
		{	len = 2;
			tag = 0;
		}
		else
		{	len = get_num_sym(w+index);
			tag = 6;
		}
		if(len == 0 && index > 0)
		{	len = search_unit(w+index);
			if(len > 0)
				tag = 0;
		}
		if(len == 0)
		{	if(first_try)   return false;
			else if(index <= strlen(m_word) - 2)
			{	len = 2; tag = 0;  occ  = 1;}

		}
		if(len > 0)
		{	
			char *bb = mid_word(w, index, len);
			if(is_number(bb))
				tag = 4;
			else if(is_english(bb))
				tag = 5;

			add_word(index, m_simple_set.add(head_word(w+index, len), tag, occ));
			return all_search(index + len, first_try);
		}
	}
	else
	{
		for(int i = 0; i<n; i++)
		{	if( i == 0 && hh && m_simple_set.sn[res[i]].len == 2)
				continue;
			add_word(index, res[i]);	
			all_search(index + m_simple_set.sn[res[i]].len, first_try);
		}
	}
	if(n == 0)
		return false;
	return true;	
}

/*
char *CompoundAnalysis::tail_word(char *str, int n)
{
	return str+n;
}
*/

bool CompoundAnalysis::valid_neigh(int wd1, int wd2, int brother_node)
{
	if(m_simple_set.sn[wd1].len == 2 && m_simple_set.sn[wd2].len == 2 && m_simple_set.sn[wd1].kind < 3)
	{	if(m_simple_set.sn[wd2].kind == 1)
			return false;
		if(*m_simple_set.sn[wd2].word < 0x80 && *m_simple_set.sn[wd2].word > 0 ||
			*m_simple_set.sn[wd1].word < 0x80 && *m_simple_set.sn[wd1].word > 0)
			return true;
		if(m_simple_set.sn[wd1].occ < 10 && m_simple_set.sn[wd2].occ < 10)
			return false;
	}
	if(brother_node >= 0 && (m_simple_set.sn[wd1].len == 2 || m_simple_set.sn[wd2].len == 2 && m_simple_set.sn[wd2].kind == 0))
	{
		if(m_simple_set.sn[m_nodes[brother_node].word].len ==m_simple_set.sn[wd1].len + m_simple_set.sn[wd2].len)
			return false;
	}
	return true;
}
int CompoundAnalysis::all_group(int first_child_node, TERM_LIST *tl_set, int max_num, bool first_try)
{
	if(max_num == 0)  return 0;

	int node = first_child_node;
	TERM_LIST *po=tl_set;
	int n = 0;
	while(node != -1)
	{	int nn=0;
		if(m_nodes[node].first_child == -1)
			nn = 1;
		else
			nn =all_group(m_nodes[node].first_child, po, max_num -n, first_try);
		int j = 0;
		for(int i = 0; i< nn ; i++)
		{	if(po[i].count> 0 && first_try && !valid_neigh(m_nodes[node].word,  po[i].terms[po[i].count-1], m_nodes[node].next_brother))
				continue;
			if(i != j)
				memcpy(&po[j], &po[i], sizeof(TERM_LIST));
			po[j].terms[po[j].count] = m_nodes[node].word;
			po[j].count++;
			j++;
		}
		n+=j;
		if(max_num - n <= 0)
			break;
		int j1 = j;
		while(j < nn)
		{	po[j].count = 0;  j++;}
		po+=j1;
		node = m_nodes[node].next_brother;
	}
	return n;
}

bool CompoundAnalysis::is_number(char  *str)
{	
	int i;
	for(i=0;i< (int)strlen(str);i++)
		if(str[i] <= 0 || (isdigit((unsigned  char)(str[i]))==0 &&
			 str[i] != '.' && str[i] != ',' && str[i] != '%' && str[i] != '$'))
			break;
	if(i==(int)strlen(str))
		return true;
	else
		return  false;
}
bool CompoundAnalysis::is_english(char  *str)
{
	int i;
	for(i=0;i< (int)strlen(str);i++)
		if(!(str[i] >= 'a' && str[i] <='z' || str[i] >= 'A' && str[i] <='Z')) 
			break;
	if(i==(int)strlen(str))
		return true;
	else
		return  false;
}

int CompoundAnalysis::get_num_sym(char *str0)
{
	int i = 0;
	unsigned char *str = (unsigned char *)str0;
	int len = strlen(str0);
	while(i < len)
	{	if(str[i] < 0x80) 
			i++;
		else if(str[i] >= 0xa0 && str[i] < 0xb0)
			i+=2;
		else
			break;
	}	
	return i;
}

bool CompoundAnalysis::is_num_sym(char *str0)
{
	unsigned char *str = (unsigned char *)str0;
	int len = strlen(str0);
	if(len == 0)
		return false;
	if(str[0] < 0xb0 && (str[len-1] < 0x80 || len > 1 && str[len-1] > 0x80 && str[len-2] <0xb0 && str[len-2] >= 0xa0))
		return true;
	return false;
}


bool CompoundAnalysis::is_unit_noun(char *str)
{
	void *p = bsearch(str, m_poinUnit, m_countUnit, sizeof(char *), comp_str_strpoin);
	if( p != NULL)
		return true;
	return false;
}

int CompoundAnalysis::search_unit(char *str)
{
	if(str == NULL || strlen(str) < 2)
		return 0;
	char *po = str;
	char buff[10];
	while(*po < 0 && po - str <= 4)
	{	po+=2;	
		strncpy(buff, str, po - str);
		buff[po - str] = 0;
	    void *p = bsearch(buff, m_poinUnit, m_countUnit, sizeof(char *), comp_str_strpoin);
		if( p != NULL)
			return po - str;
	}
	return 0;
}


SimpleNounSet::SimpleNounSet()
{	
	clear();
}
int SimpleNounSet::add(char *noun, void *ca)
{
	if(count >= 50)
		return -1;
	CompoundAnalysis *cs= (CompoundAnalysis *)ca;
	int pos = cs->m_trie_simple.search(noun);
	fseek(cs->m_file_simple_info, pos, 0);
	char kind;
	short occ; char info[100];
	fread(&kind, 1, 1, cs->m_file_simple_info);
	fread(&occ, 2, 1, cs->m_file_simple_info);
	fread(info, 100, 1, cs->m_file_simple_info);
	return add(noun, kind, occ, info);

}
bool  SimpleNounSet::search_pair(int i, int j, void *cs)
{

	CompoundAnalysis *ca = (CompoundAnalysis *)cs;
	if(!(i >= 0  && i<count && j >= 0 && j< count))
		return false;
	if(pairs[i][j] != -1)
		return pairs[i][j];
	char buff[100];
	sprintf(buff, "%s/%s", sn[i].word, sn[j].word);
	if(ca->m_trie_pair.search(buff) < 0 && !ca->match_num_unit(sn[i].word, sn[j].word))
		pairs[i][j] = 0;
	else pairs[i][j] = 1;
	return pairs[i][j];
}

int SimpleNounSet::add(char *noun, int kind, int occ, char *info)
{
	if(count >= 50)  return -1;
	sn[count].word = cur_pos;
	sn[count].len = strlen(noun);
	strcpy(cur_pos, noun);
	cur_pos += strlen(cur_pos) + 1;
	if(info != NULL)
	{	sn[count].info = cur_pos;
		strcpy(cur_pos, info);
		cur_pos += strlen(cur_pos) + 1;
	}
	else
		sn[count].info = NULL;
	sn[count].kind = kind;
	sn[count].occ = occ;
//	if(strlen(noun) == 2 && occ > 10)
//		sn[count].occ = 10;	
	count++;
	return count-1;
}

void SimpleNounSet::clear()
{
	count = 0;
	cur_pos = buff;
	memset(pairs, 0xff, 50*50); 
}

int SimpleNounSet::search(char *noun)
{

	for(int i = 0; i< count; i++)
	{	if(strcmp(sn[i].word, noun) == 0)
		{	
			return i;
		}
	}
	return -1;
}
