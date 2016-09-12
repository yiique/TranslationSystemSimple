#include "Stdafx_engine.h"
#include "Zamo.h"

 
int cst_comp_kor(const void *v1, const void *v2)
{
	ZamoElem *s1 = (ZamoElem *)v1;
	ZamoElem *s2 = (ZamoElem *)v2;
	int r = strncmp(s1->kor, s2->kor, 2);
	if (r!=0)
		return r;	
	return strncmp(s1->sym, s2->sym, 3);
}

int cst_comp_sym(const void *v1, const void *v2)
{
	ZamoElem *s1 = (ZamoElem *)v1;
	ZamoElem *s2 = (ZamoElem *)v2;
	int r = strncmp(s1->sym, s2->sym, 3);
	if (r!=0)
		return r;
	return strncmp(s1->kor, s2->kor, 2);
}

int cst_comp_sym_1(const void *v1, const void *v2)
{
	ZamoElem *s2 = (ZamoElem *)v2;
	return strncmp((char *)v1, s2->sym, 3);
}

int cst_comp_kor_1(const void *v1, const void *v2)
{
	ZamoElem *s2 = (ZamoElem *)v2;
	return strncmp((char *)v1, s2->kor, 2);
}

Zamo::Zamo()
{
}

bool Zamo::create(char *tablefile)
{
	FILE  *fp = fopen(tablefile, "rb");
	if (fp==NULL)
		return true;
	fseek(fp, 0,2);
	m_len = ftell(fp);
	fseek(fp, 0, 0);
	m_pTable = (ZamoElem *)malloc(m_len);
	m_pTable_1 = (ZamoElem *)malloc(m_len);
	fread(m_pTable, 1, m_len, fp);
	memcpy(m_pTable_1, m_pTable, m_len);
	fclose(fp);
	m_len = m_len /5;
	qsort(m_pTable, m_len, 5, cst_comp_kor);
	qsort(m_pTable_1, m_len, 5, cst_comp_sym);
	return true;
}

Zamo::~Zamo()
{
	free(m_pTable);
	free(m_pTable_1);

}
bool Zamo::compose(char *symbols, char *letter)
{
	char *po = symbols;
	char *res = letter;
	if (*po == 0)
		return false;
	while(*po != 0)
	{
		ZamoElem *p = (ZamoElem *)bsearch(po, m_pTable_1, m_len, 5, cst_comp_sym_1);
		if(p != NULL)
		{	strncpy(res, p->kor, 2);
			res +=2;
		}
		po +=3;
	}
	*res = 0;
	return  true;
}
bool Zamo::decompose(char *letter, char *symbols,int count)
{	
	char *res = symbols;
	unsigned char *po = (unsigned char *)letter;
	int cnt=0;
	while(*po != 0)
	{
		if(*po == 0xa4 && *(po+1) >= 0xa1 && *(po+1) <= 0xb3)
		{	*res = *(po +1) - 0xa1 + 'a';
			res++;
		}
		else
		{	ZamoElem *p = (ZamoElem *)bsearch(po, m_pTable, m_len, 5, cst_comp_kor_1);
			if(p != NULL)
			{	strncpy(res, p->sym, 3);
				res +=3;
			}
			else return false;
		}
		po+=2;
		cnt++;
		if(cnt==count)
			break;
	}
	*res = 0;
	return  true;
}
