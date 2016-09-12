// OptionSet.cpp: implementation of the COptionSet class.
//
//////////////////////////////////////////////////////////////////////
#include "Stdafx_engine.h"
#include "OptionSet.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COptionSet::COptionSet()
{
	strcpy(m_dicPath, "./mdata/korean/data/");
	m_inputMode = 0;
	m_outputMode = 0;
	m_compNounRes = 1;
	m_ambigRes = 0;
	m_code_input = 1;
	m_code_output = 1;
	m_numberConverse = 0;
	m_change_letter1 = 0;
	m_change_letter2 = 0;
	m_delete_letter = 0;
	m_change_letter3 = 0;
	m_add_letter = 0;
	m_syn_letter = 0;
	m_tag_mode=1;
	m_sym_mode = 1;
}

COptionSet::~COptionSet()
{
}
void trim(char *str);

bool COptionSet::load(const char *fname)
{
	FILE *fp_ini = fopen(fname, "rt");
	if(fp_ini == NULL)
		return false;
	char buff[500];
	while (fgets(buff, 500, fp_ini))
	{
		if(buff[0]==';')
			continue;
		if(buff[strlen(buff)-1] == 0x0a)
			buff[strlen(buff)-1] = 0;
		char *p1 = strstr(buff, "=");
		if(p1==NULL)
			continue;
		char str_option[50];
		char str_val[10];
		p1 ++;
		trim(p1);
		if(*p1==0)
			continue;
		strcpy(str_val, p1);
		*(p1-1)=0;
		strcpy(str_option, buff);
		int kind = get_option_kind(str_option);
		if(kind==-1)
			continue;
		switch(kind) 
		{
		case dic_path:
			strcpy(m_dicPath, str_val);
			break;
		case input_mod:
			m_inputMode = atoi(str_val);
			break;
		case output_mod:
			m_outputMode = atoi(str_val);
			break;
		case ambi_res:
			m_ambigRes = atoi(str_val);
			break;
		case compnoun_res:
			m_compNounRes = atoi(str_val);
			break;
		case code_input:
			m_code_input = atoi(str_val);
			break;
		case code_output:
			m_code_output = atoi(str_val);
			break;
		case num_converse:
			m_numberConverse = atoi(str_val);
			break;
		case change_letter1:
			m_change_letter1 = atoi(str_val);
			break;
		case change_letter2:
			m_change_letter2 = atoi(str_val);
			break;
		case delete_letter:
			m_delete_letter = atoi(str_val);
		case change_letter3:
			m_change_letter3 = atoi(str_val);
			break;
		case add_letter:
			m_add_letter = atoi(str_val);
			break;
		case syn_letter:
			m_syn_letter = atoi(str_val);
			break;
		case tag_mode:
			m_tag_mode = atoi(str_val);
			break;
		case sym_mode:
			m_sym_mode = atoi(str_val);
			break;
		}
	}
	if(m_ambigRes == 1)
		m_outputMode = 3;
	fclose(fp_ini);
	return true;
}

int COptionSet::get_option_kind(const char* str)
{
	int n;
	for (n=0; n<OPTION_NUM; n++)
	{
		if(strcmp(str, option_str[n])==0)
			return n; 
	}
	return -1;
}
