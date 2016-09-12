#include "Stdafx_engine.h"
#include "ArrayTrie.h"


#if defined (_DB)
#include "mysql.h"
MYSQL  *cst_myData;
#endif


ArrayTrie::ArrayTrie()
{
#ifdef _MTDLL
	m_pTrie = NULL;
	m_pTail = NULL;
#endif
	m_fpTrie=NULL;	m_fpTail=NULL;
	m_words=NULL;
}

bool ArrayTrie::create(char *fname, int mode, bool is_db)
{
	char fname1[MAX_PATH];
	char fname2[MAX_PATH];
	sprintf(fname1, "%s.tri", fname);
	sprintf(fname2, "%s.tai", fname);
	return arrayTrie(fname1, fname2, mode, is_db);
}

bool ArrayTrie::create(char *fname1, char *fname2, int mode, bool is_db)
{
	return arrayTrie(fname1, fname2, mode, is_db);
}

bool ArrayTrie::arrayTrie(char *fname1, char *fname2, int mode, bool is_db)
{
	if(is_db)
	{	m_mode = -1;
		return true;
	}
	m_mode = mode;


	char buff[1000];
	if(mode == NEW_TRIE)
	{	
		m_fpTrie = fopen(fname1, "w+b");
		m_fpTail = fopen(fname2, "w+b");

		strcpy(m_head.version, "ARRTRIE01");
		m_head.fir_empty = SETSIZE * 2;  
		m_head.key_count = 0;
		m_head.count = SETSIZE * 10;
		fseek(m_fpTrie, 0, 0);
		fwrite(&m_head, sizeof(m_head),1, m_fpTrie);
		makeEmpty(m_pElem, SETSIZE);
		int k = 1;
		for(int j = 0; j< 10; j++)
		{
			for(int i=0; i<SETSIZE; i++)
			{	m_pElem[i].check = -k; k++; }
			int position = ftell(m_fpTrie);
			fseek(m_fpTrie, position, 0);
			fwrite(m_pElem, sizeof(ELEM), SETSIZE, m_fpTrie);
		}
		fflush(m_fpTrie);

		memset(buff, 0, 16);
		strcpy(buff, "TAIL_ARRAY_TRIE");
		fseek(m_fpTail, 0, 0);
		fwrite(buff, 16, 1, m_fpTail);
		fflush(m_fpTail);
	}
	else
	{
#ifdef _UD
		m_fpTrie = fopen(fname1, "r+b");
#else
		m_fpTrie = fopen(fname1, "rb");
#endif
		if (m_fpTrie == NULL)
			return false;

#ifdef _MTDLL
		fseek(m_fpTrie, 0, SEEK_END);
		int len = ftell(m_fpTrie);
		fseek(m_fpTrie, 0, SEEK_SET);
		m_pTrie = (char*)malloc(len+1);
		fread(m_pTrie,sizeof(char),len,m_fpTrie);
		*(m_pTrie+len) = 0;
		memcpy(&m_head, m_pTrie, sizeof(m_head));
#else
		fseek(m_fpTrie, 0, SEEK_SET);
		fread(&m_head, sizeof(m_head), 1, m_fpTrie);
#endif


#ifdef _MACOS
		cst_reverse_int(&m_head.fir_empty);
		cst_reverse_int(&m_head.count);
		cst_reverse_int(&m_head.key_count);
#endif

#ifdef _UD
		m_fpTail = fopen(fname2, "r+b");
#else
		m_fpTail = fopen(fname2, "rb");
#endif
		if (m_fpTail == NULL)
			return false;
	
#ifdef _MTDLL
		fseek(m_fpTail, 0, SEEK_END);
		len = ftell(m_fpTail);
		fseek(m_fpTail, 0, SEEK_SET);
		m_pTail = (char*)malloc(len);
		fread(m_pTail,sizeof(char),len,m_fpTail);
		m_lenTailFile = len;
#endif
	}
	m_words = NULL;
	m_words_count = 0;
	return true;
}

ArrayTrie::~ArrayTrie()
{	
	if(m_mode ==  -1)
		return;
 	if(m_mode == NEW_TRIE)
		write_head();
	if (m_fpTrie) fclose(m_fpTrie);
	if (m_fpTail) fclose(m_fpTail);
	if(m_words != NULL)
	{
		for(int i=0; i< m_words_count; i++)
		{	
			if(m_words[i])
				free(m_words[i]);
		}

		free(m_words);
		m_words = NULL;
	}
#ifdef _MTDLL
	if(m_pTrie)
		free(m_pTrie);
	if(m_pTail)
		free(m_pTail);
#endif
}

void ArrayTrie::write_head()
{
	fseek(m_fpTrie, 0, 0);
	fwrite(&m_head, sizeof(m_head), 1, m_fpTrie);
	fflush(m_fpTrie);
	fflush(m_fpTail);
}

void ArrayTrie::makeEmpty(ELEM *pElem, int count)
{
	memset(pElem, 0, count * sizeof(ELEM));
}


#if defined(_DB)
	char cst_szSQL[10000];

#endif
	int  cst_cur_len;
int ArrayTrie::search(const char *str)
{
	if(m_mode == -1)
	{
#if defined(_DB)
		int len = strlen(str);
		if (len>0 && str[len-1]==0x20)
			return -1;
		MYSQL_RES	*res; 
		MYSQL_ROW	row;

		CString ss = str;
		ss.Replace("\\", "\\\\");
		ss.Replace("'", "\\'");
		int r = -1;	
		sprintf(cst_szSQL, "select name from %s where name = '%s'", "engword_new", (LPCTSTR)ss);
		if (!mysql_query( cst_myData, cst_szSQL ) ) 
		{	
			res = mysql_store_result( cst_myData );
			if(row = mysql_fetch_row( res ) ) 
			{	
				r = 1;
			}
			mysql_free_result( res ) ;
		}
		return r;
#endif
	}
	int base = 0;
	const char *po[1];
	po[0] = str;
	return sub_search(po, &base, NULL);
}


int  ArrayTrie::search_longest(const char *str, int *n, int maxlen, bool get_offset)
{
	memset(n, 0xff, 4* maxlen);
	char buff[2000];

	if(m_mode == -1)
	{
#if defined(_DB)

		MYSQL_RES	*res; 
		MYSQL_ROW	row;

		const char *poin = str;
		while(*poin != 0)
		{	char *p = strstr(poin, " ");
			if(p != NULL)
			{	int ii = p - str +1;
				strncpy(buff, str, ii);
				buff[ii] = 0;


				CString ss = buff;
				ss.Replace("\\", "\\\\");
				ss.Replace("'", "\\'");
				int r = -1;	
				sprintf(cst_szSQL, "select name from %s where name like '%s%%' limit 0, 1", "engword_new", (LPCTSTR)ss);
				if (!mysql_query( cst_myData, cst_szSQL ) ) 
				{	
					res = mysql_store_result( cst_myData );
					if(row = mysql_fetch_row( res ) ) 
					{	
						r = 1;
					}
					mysql_free_result( res ) ;
				}
				
				if(r< 0)
				{	maxlen = ii-1;
					break;
				}
				poin = p+1;
			}
			else
			{
				maxlen = poin - str + strlen(poin);
				break;
			}
		}

		int k=strlen(str);
		if(k > maxlen ) k = maxlen;

		int last_len = 0;
		for(int i=1; i<=k; i++)
		{	
			strncpy(buff, str, i);
			buff[i] = 0;
			CString ss = buff;
			ss.Replace("\\", "\\\\");
			ss.Replace("'", "\\'");
			
			sprintf(cst_szSQL, "select name from %s where name = '%s'", "engword_new", (LPCTSTR)ss);
			int cur = 0;
			if (!mysql_query( cst_myData, cst_szSQL ) ) 
			{	
				res = mysql_store_result( cst_myData );
				if(row = mysql_fetch_row( res ) ) 
				{
					last_len = i;
					n[i-1] = i;
				}
				mysql_free_result( res ) ;
			}
		}
		if(last_len > 0)
		{	strncpy(buff, str, last_len);
			buff[last_len] = 0;
		}
		else
		{	strncpy(buff, str, 1);
			buff[1] = 0;
		}
			
		CString ss = buff;
		ss.Replace("\\", "\\\\");
		ss.Replace("'", "\\'");
		ss.Replace("%", "\\%");
		sprintf(cst_szSQL, "select name from %s where name like '%s", "engword_new", (LPCTSTR)ss);
		strcat(cst_szSQL, "%' order by name");
		if (!mysql_query( cst_myData, cst_szSQL ) ) 
		{	
			res = mysql_store_result( cst_myData );
			while((row = mysql_fetch_row( res )) && strncmp(row[0], str, last_len) <=0 ) 
			{	if(strncmp(row[0], str, strlen(row[0])) == 0)
				{
				
				}
				else
				{
					for(int j= last_len; j < (int)strlen(row[0]); j++)
					{	if(row[0][j] != str[j])
							break;
					}
	
					last_len = j;
				}

			}
			mysql_free_result( res ) ;
		}
		return last_len;
#endif	
	}		
	const char *poins[300];
	memset(poins, 0, 300 * 4);
	const char **pp = &poins[0];
	int base = 0;
	const char *po[1];
	po[0] = str;
	int curLen = 0;
	sub_search(po, &base, NULL, pp, &curLen);

//	int max_length = curLen;
	int i;
	for(i=0; i<300; i++)
	{	if(poins[i] == 0)
			break;
	}
	int max_length = 0;
	if(i > 0)
	{	
		int len = 0;
		for(int j=0; j< i; j++)
		{	len = poins[j] - str;
			max_length = len;
			if(get_offset)
				n[len-1] = len;
			else
			{
				strncpy(buff, str, len);
				buff[len] = 0;
				n[len-1] = search(buff);
			}
		}
	}
	return max_length;

}	

int ArrayTrie::sub_search(const char **po, int *base_index, bool *exist, const char **pp, int * curLen, int *data_poin)
{
	const char *str = *po;
	ELEM e = elemof(*base_index);
	int base = e.base;	
	if(base < 0)
	{	
		TAIL tail = tailof(-base, data_poin);
		if(str != NULL)
		{	int i;
			for(i=0; i<(int)strlen(str) && i<(int)strlen(tail.tail); i++)
			{	if(str[i] != tail.tail[i])
					break;
			}
			if(curLen)
				*curLen = *curLen + i;
		}

		if(str != NULL && pp != NULL)
		{	
			if(strncmp(tail.tail, str, strlen(tail.tail)) == 0)
			{		pp[0] = str + strlen(tail.tail);
					*pp++;
			}
		}	
		if(str != NULL && strcmp(tail.tail, str) == 0 ||
			str == NULL && tail.tail[0] == 0) 
		{	
			return tail.data;
		}
		else
			return -1;
	}
	else
	{
		if(pp != NULL)
		{
			ELEM e = elemof(base);
			if(e.check == *base_index)
			{	pp[0] = str;
				*pp++;

			}
		}
		int i = indexof(str[0]);
		int t = base + i;
		ELEM elem = elemof(t);	
		if(exist != NULL)
		{
			if(elem.check < 0)
				*exist = false;
			else
				*exist = true;
		}
		if(elem.check == *base_index)
		{	if(*str == 0)
				*po = NULL;
			else
			{	*po = str+1;
				if(curLen)
					*curLen = *curLen + 1;
			}
			*base_index = t;

			return sub_search(po, base_index, exist, pp, curLen, data_poin);
		}
		else
		{	*po = str; 
			return -1;
		}
	}		
}

bool ArrayTrie::substring_search(const char **po, int *base_index, int *check_index)
{
	const char *str = *po;
	ELEM e = elemof(*base_index);
	int base = e.base;	
	if(base < 0)
	{	
		TAIL tail = tailof(-base);
		if(str != NULL)
		{	int i;
		    for(i=0; i<(int)strlen(str) && i<(int)strlen(tail.tail); i++)
			{	if(str[i] != tail.tail[i])
					break;
			}
		
			if(i >= strlen(str))
			{	*base_index = base;  return true;  }		

		}

		return false;
	}
	else
	{
		int i = indexof(str[0]);
		int t = base + i;
		ELEM elem = elemof(t);	
		if(elem.check == *base_index)
		{	if(*(str+1) == 0)
			{	*check_index = t;
				*base_index = elem.base;
				return true;
			}
			else
			{	*po = str+1;
			}
			*base_index = t;

			return substring_search(po, base_index, check_index);
		}
		else
		{	
			return false;
		}
	}		
}


int ArrayTrie::indexof(char c)
{
	return (int)((unsigned char)c);
}
ELEM ArrayTrie::elemof(int index)
{
	ELEM elem;
	int poin = sizeof(m_head) + index * sizeof(ELEM);

#ifdef _MTDLL
	memcpy(&elem, m_pTrie+poin, sizeof(ELEM));
#else
	fseek(m_fpTrie, poin, 0);
	fread(&elem, sizeof(ELEM), 1, m_fpTrie);
#endif

#ifdef _MACOS
	cst_reverse_int(&elem.base);
	cst_reverse_int(&elem.check);
#endif

	return elem;
}

TAIL  ArrayTrie::tailof(int pos, int *data_poin)
{
	TAIL tail;

#ifdef _MTDLL
	int len = sizeof(tail.tail);
	if(m_lenTailFile - pos < len)
		len = m_lenTailFile - pos;
	memcpy(tail.tail, m_pTail + pos, len);
#else
	fseek(m_fpTail, pos, 0);
	fread(tail.tail, sizeof(tail.tail), 1, m_fpTail);
#endif

	int i=0;
	while(tail.tail[i] != 0x01)
		i++;
	i++;
	tail.data = *(int *)(tail.tail + i);
#ifdef _MACOS
	cst_reverse_int(&tail.data);
#endif


	if(i == 1)
	{	tail.tail[0] = 0; }
	if(data_poin != NULL)
		*data_poin = pos+i;
	return tail;
}


void ArrayTrie::insert(const char *str, int data)
{
	if(str == NULL || str[0] == 0)
		return;

	bool exist;
	exist = false;
	sub_insert(str, data, 0, exist);
	if(!exist)
		m_head.key_count++;

	if(m_mode == OLD_TRIE)
		write_head();
}

void ArrayTrie::sub_insert(const char *str, int data, int base_index, bool &exist)
{
	const char *po[1];
	po[0] = str;
	int val = sub_search(po, &base_index, &exist);
	if(val >=0)
		return;
	ELEM elem = elemof(base_index);
	int base = elem.base;	
	if(base < 0)
	{	int len;
		TAIL tail = tailof(-base, &len);
		int n[1];
		n[0] = indexof(tail.tail[0]);
		int new_index = getNewSet(n,1);
		elem.base = new_index;
		modify_elem(base_index, elem);
		int pos;
		if(*tail.tail == 0)
			pos = add_tail(NULL, tail.data, -base);
		else
			pos = add_tail(tail.tail+1, tail.data, -base);
		elem.base = -pos;
		elem.check = base_index;
		modify_elem(new_index + n[0], elem);
		sub_insert(*po, data, base_index, exist);
	}
	else if(!exist)
	{	str = *po;
		int t=base + indexof(str[0]);
		ELEM elem0;
		elem0.check = base_index;
		int pos;
		if(*str == 0)
			pos = add_tail(NULL,data);
		else
			pos = add_tail(str+1,data);
		elem0.base = -pos;

		cancel_empty(t);
		modify_elem(t, elem0);
	}
	else
	{
		str = *po;
		int n[SETSIZE];
		int count = getAll(base, base_index, n);
		n[count] = indexof(str[0]);
		count++;
		int new_index = getNewSet(n,count);
		elem.base = new_index;
		modify_elem(base_index, elem);

		ELEM elem0; 
		for(int i=0; i< count-1; i++)
		{
			int t= base + n[i];
			elem0 = elemof(t);
    		add_empty(t);
			modify_elem(new_index + n[i], elem0);
			ELEM ee[SETSIZE];
			if(elem0.base >= 0)
			{
				getOneSet(elem0.base, &ee[0]);
				for(int j=0; j<SETSIZE; j++)
				{	
					if(ee[j].check == t)
					{	ee[j].check = new_index + n[i];
					}
				}	
				putOneSet(elem0.base, &ee[0]);
			}
		}
		int pos;
		if(*str == 0)
			pos = add_tail(NULL,data);
		else
			pos = add_tail(str+1,data);
		elem0.base = -pos;
		modify_elem(new_index + n[count-1], elem0);
	}
}

void ArrayTrie::remove(const char *str)
{
	if(str == NULL || str[0] == 0)
		return;
	if(sub_remove(str, 0))
	{	m_head.key_count--;
		if(m_mode == OLD_TRIE)
			write_head();
	}
}

bool ArrayTrie::sub_remove(const char *str, int base_index)
{
	ELEM e = elemof(base_index);
	int base = e.base;	
	if(base < 0)
	{	
		TAIL tail = tailof(-base);
		if(str != NULL && strcmp(tail.tail, str) == 0 ||
			str == NULL && tail.tail[0] == 0) 
		{	add_empty(base_index);
			return true;
		}
		else
			return false;
	}
	else
	{
		int i = indexof(str[0]);
		int t = base + i;
		ELEM elem = elemof(t);	
		const char *po;
		if(elem.check == base_index)
		{	if(*str == 0)
				po = NULL;
			else
				po = str+1;
//			base_index = t;
//			bool res =sub_remove(po, base_index);
			bool res =sub_remove(po, t);
			if(!res)
				return false;
			if(is_empty_set(base, base_index))
				add_empty(base_index);
			return true;
		}
		else
			return false;
	}
}

bool ArrayTrie::is_empty_set(int first_index, int check_index)
{
	for(int i=0; i< SETSIZE; i++)
	{
		if(elemof(first_index +i).check == check_index)
			return false;
	}
	return true;
}


void ArrayTrie::modify_elem(int index, ELEM &elem)
{
	fseek(m_fpTrie, sizeof(m_head) + index * sizeof(ELEM), 0);
	fwrite(&elem, sizeof(ELEM),1 , m_fpTrie);
}

int   ArrayTrie::add_tail(const char *str, int data,  int poin)
{	char buff[1000];
	int len = 0;
	if(str != NULL)
	{	strcpy(buff, str);
		len = strlen(str) +1;
	}
	buff[len] = 0x01;
	len++;
	*(int *)(buff + len) = data;
	int pos;
	if(poin == 0)
	{	
		fseek(m_fpTail, 0,2);
		pos = ftell(m_fpTail);
	}
	else
	{	
		fseek(m_fpTail, poin, 0);
		pos = poin;
	}
	fwrite(buff, len +sizeof(int), 1, m_fpTail);


	return  pos;
}

int cst_comp_int(const void *v1, const void *v2)
{	
	return (*(int *)v1 - *(int *)v2); 
}

int ArrayTrie::getNewSet(int *pindex, int count)
{
	
	int n[SETSIZE];
	memcpy(n, pindex, count * sizeof(int));
	qsort(n, count, sizeof(int), cst_comp_int);

	int cur_emp = m_head.fir_empty;
	int past_emp = -1;
	while(cur_emp < m_head.count)
	{	if(cur_emp < 0)
			int u=0;
		if(is_available(cur_emp, n, count))
		{	int base_index = cur_emp - n[0];
			modify_empty(cur_emp,past_emp, n, count);
			return base_index;
		}
		else
		{	past_emp = cur_emp;
			cur_emp = -elemof(cur_emp).check;
		}
	}
	modify_empty(cur_emp, past_emp, n, count);
	return m_head.count - SETSIZE; 
}

bool ArrayTrie::getOneSet(int startElemNum, ELEM *res)
{
	if(startElemNum < m_head.count + SETSIZE)
	{
#ifdef _MTDLL
		memcpy(res, m_pTrie + sizeof(m_head) + startElemNum * sizeof(ELEM), SETSIZE * sizeof(ELEM));
#else
		fseek(m_fpTrie, sizeof(m_head) + startElemNum * sizeof(ELEM), 0);
		fread(res, SETSIZE * sizeof(ELEM), 1, m_fpTrie);
#endif
		
#ifdef _MACOS
		for(int i=0; i<SETSIZE; i++)
		{	cst_reverse_int(&res[i].base);
			cst_reverse_int(&res[i].check);
		}
#endif

		return true;
	}
	else
		return false;
}
bool ArrayTrie::putOneSet(int startElemNum, ELEM *res)
{
	if(startElemNum < m_head.count + SETSIZE)
	{	fseek(m_fpTrie, sizeof(m_head) + startElemNum * sizeof(ELEM), 0);
		fwrite(res, SETSIZE * sizeof(ELEM), 1, m_fpTrie);
		return true;
	}
	else
		return false;
}

bool ArrayTrie::is_available(int cur_emp, int *pindex, int count)
{
	if(cur_emp < pindex[0])
		return false;
	int base = cur_emp - pindex[0];
	for(int i=0; i<count; i++)
	{	if(base + pindex[i] >= m_head.count)
			return true;
		else if(elemof(base + pindex[i]).check >= 0)
			return false;
	}
	return true;
}

void ArrayTrie::cancel_empty(int index)
{

	if(index < SETSIZE *2)
		return;
	ELEM elem = elemof(index);
	if(elem.check >=0)
		return;

	if(index < m_head.fir_empty)
		return;

	if(index == m_head.fir_empty)
	{	m_head.fir_empty = -elem.check;
		return;
	}

	int i=index-1;
	ELEM e;
	while(i >= m_head.fir_empty)
	{	e = elemof(i);
		if(e.check < 0)
		{	
			e.check = elem.check;
			modify_elem(i, e);
			return;
		}
		i--;
	}

}

void ArrayTrie::add_empty(int index)
{

	ELEM elem = elemof(index);
	if(index < SETSIZE * 2)
	{	elem.check = -(index +1);
		modify_elem(index, elem);
		return;
	}
		
	if(elem.check < 0)
		return;

	if(index < m_head.fir_empty)
	{	elem.check = -m_head.fir_empty;
		modify_elem(index, elem);
		m_head.fir_empty = index;
		return;
	}

	int past_emp = m_head.fir_empty;
	int cur_emp = -elemof(past_emp).check;
	while(cur_emp < m_head.count && cur_emp < index)
	{	past_emp = cur_emp;
		cur_emp = -elemof(past_emp).check;
	}
	ELEM e = elemof(past_emp);
	e.check = -index;
	modify_elem(past_emp, e);
	elem.check = -cur_emp;
	modify_elem(index, elem);

}
void ArrayTrie::modify_empty(int cur_emp, int past_emp, int *pindex, int count)
{
	int base = cur_emp - pindex[0];
	ELEM cur_elem;
	ELEM past_elem;
	if(past_emp >=0)
		past_elem = elemof(past_emp);

	if(base + SETSIZE > m_head.count)
	{	
		int co = base + SETSIZE - m_head.count;
		makeEmpty(m_pElem, co);
		for(int j=0; j<co; j++)
			m_pElem[j].check = -(m_head.count +j +1);
		fseek(m_fpTrie, 0,2);
		fwrite(m_pElem, sizeof(ELEM), co, m_fpTrie);
		m_head.count += co;
	}

	int i=0;
	while(i< count)
	{	
		cur_elem = elemof(cur_emp);
		int check = cur_elem.check;
		if(cur_emp < base + pindex[i])
		{	past_elem = cur_elem;
			past_emp = cur_emp;
			cur_emp =-check;
		}
		else if(cur_emp == base + pindex[i])
		{	cur_emp = -check;
			if(past_emp >=0)
			{	past_elem.check = check;
				modify_elem(past_emp,past_elem);
			}
			else
				m_head.fir_empty = -check;
	
			i++;
		}
		else
			break;
	}
}


int ArrayTrie::getAll(int base, int check_val,int *pn)
{
	int j = 0;
	ELEM elem;
	for(int i=0; i< SETSIZE; i++)
	{
		elem = elemof(base +i);
		if(elem.check == check_val)
		{	pn[j] = i;
			j++;
		}
	}
	return j;
}

int ArrayTrie::getAllWords(char ***result)
{
	if(m_words != NULL)
	{
		for(int i=0; i< m_words_count; i++)
			free(m_words[i]);
		free(m_words);
		m_words = NULL;
	}

	m_words_count = m_head.key_count;
	m_words = (char **)malloc(sizeof(int) * m_words_count);
	memset(m_words, 0, 4 * m_words_count);
	char ww[1000]; 
	ww[0] = 0;
	m_words_index = 0;
	getAllWords_sub(0, 0, ww);
	*result = m_words;
	return m_words_index;
}
int ArrayTrie::getAllWords_startWith(char *key, char ***result, int number)
{
	if(m_words != NULL)
	{
		for(int i=0; i< m_words_count; i++)
		{	if(m_words[i]) 	free(m_words[i]); }
		free(m_words);
		m_words = NULL;
	}

// 	if (m_head.key_count<0)
// 		m_head.key_count = 0;
	m_words_count = number;
	m_words = (char **)malloc(sizeof(int) * m_words_count);
	memset(m_words, 0, 4 * m_words_count);
	char ww[1000]; 
	strcpy(ww, key);
	m_words_index = 0;
	int base_index = 0;
	int check_index = 0;

	const char *po[1];
	po[0] = ww;

	bool r= substring_search(po, &base_index, &check_index); 
	if(r)
	{	if(base_index < 0)
		{	
				TAIL tail = tailof(-base_index);
				m_words[0] = (char *)malloc(100);
				char *ppp = (char *)po[0];
				strcpy(ppp, tail.tail);
				strcpy(m_words[0], ww);
				m_words_index = 1;
		}
		else
			getAllWords_sub(base_index, check_index, ww);
	
		*result = m_words;
		return m_words_index;
	}
	return 0;
}

void ArrayTrie::getAllWords_sub(int base, int check_val, char *prep)
{
	ELEM elem;
	int len = strlen(prep);
	for(int i=0; i< SETSIZE; i++)
	{
		if(m_words_index >= m_words_count-1)
			return;
		elem = elemof(base +i);
		if(elem.base != 0 && elem.check == check_val)
		{	
			prep[len] = i;
			prep[len + 1] = 0;
			if(elem.base< 0)
			{
				TAIL tail = tailof(-elem.base);
				m_words[m_words_index] = (char *)malloc(strlen(prep) + strlen(tail.tail) + 1);
				sprintf(m_words[m_words_index], "%s%s", prep, tail.tail);
				m_words_index++;
			}
			else
			{
				getAllWords_sub(elem.base, base + i, prep);
			}
		}
	}
}


void ArrayTrie::modify_data(const char *str, int data)
{
	int base = 0;
	const char *po[1];
	po[0] = str;
	int data_poin;
	int d = sub_search(po, &base, NULL, NULL, NULL, &data_poin);
	if(d < 0 || d == data)
		return ;
	fseek(m_fpTail, data_poin, 0);
	fwrite(&data, 4, 1,m_fpTail);
	fflush(m_fpTail);
}
