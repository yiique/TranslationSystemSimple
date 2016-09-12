// SymbolProcessor.cpp: implementation of the CSymbolProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx_engine.h"
#include "SymbolProcessor.h"
#include "Tag.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSymbolProcessor::CSymbolProcessor()
{

}

CSymbolProcessor::~CSymbolProcessor()
{

}

int CSymbolProcessor::get_mophem_token(const char *input, char* token)
{
	int len = strlen(input);
	if(!input || !len)
		return -1;
	char word[300]; 
	int symbol = 0; // 0 then general, else its symbol number 
	int cd = 0;
	strcpy(word, input);
	bool stat_digit = false;
	char* pc = word;
	int flg = 0;
	if(*(unsigned char*)pc < 0x80)
	{	cd = discriminate_char(pc); pc++;
		if(cd == 1) stat_digit = true;
//		while(/*cd!=0 &&*/ cd ==0 || cd==discriminate_char(pc) || !possible_sep(pc-1,true)&& !stat_digit//
//			|| ((pc[0] == ',' || pc[0] == '.'  || pc[0] == '/' ) && !possible_sep(pc-1,true)))
		while(/*cd!=0 &&*/ cd ==0 || cd==discriminate_char(pc) || !possible_sep(pc-1,true) && !stat_digit//
			|| ((pc[0] == ',' || pc[0] == '.'  || pc[0] == '/' ) && !possible_sep(pc-1,true) ))
		{
			flg = 0;
			if(*pc == ')' || *pc == '(')
				break;

			if(*(unsigned char*)pc < 0x80)
				pc++;
			else
				pc+=2;
			if(*pc == 0)
				break;
		}
		*pc = 0;
		symbol = cd;//1;
	}
	else
	{	
		cd = discriminate_char(pc); pc += 2;
		while(cd!=0 && (cd==discriminate_char(pc) || !possible_sep(pc-1,false))
			|| ((pc[0] == ',' || pc[0] == '.' ) && !possible_sep(pc-1,false)))
			pc += 2;
		*pc = 0;
		symbol = cd;//2;
	}
	strcpy(token,word);
	return symbol ;
}

bool CSymbolProcessor::possible_sep(char *pInput, bool byte1)
{
// 	if(pInput[0] == '(' || pInput[0] == ')'
// 		return true;
	char cp[3], *pos;
	pos = pInput;
	if(pos[0] < 0x80)
	{	strncpy(cp, pos,1);
		cp[1] = 0;
	}
	else
	{	strncpy(cp, pos,2);
		cp[2] = 0;
	}
// 	strncpy(cp, pos,1); cp[1] = 0;
	int sf = discriminate_char(cp);
//	if((sf==0 && byte1)||sf>=2||sf<=15)
	if((sf==0 && byte1)||sf==1||sf==2||sf==3||sf==13)
	{	if(pInput[1] < 0x80)
		{	pos = pInput+1;
			strncpy(cp, pos,1);
			cp[1] = 0;
		}
		else
		{	pos = pInput+1;
			strncpy(cp, pos,2);
			cp[2] = 0;
		}
		sf = discriminate_char(cp);
		if((byte1 && (cp[0] == '(' && discriminate_char(pos+1) == 17 || cp[0] == ')' && discriminate_char(pos+1) == 17)) // SVM(�����˶�)�溷 ( ������
			|| !byte1 && pInput[0] == '(' || pInput[0] == ')' && discriminate_char(pInput+2) != 17
			)
			return true;
		// by hhj-29, hhj-42
		if(pInput[0] == '(' || pInput[0] == ')' || pInput[0] == '.' && pInput[1] == ' ' || pInput[0] == ',' && pInput[1] == ' '|| pInput[0] == '!' && pInput[1] == ' ')
			return true;
		// end
		if(sf>=1&&sf<=8||sf==11||sf==13)//(sf==3)
		{	if(pos[0] < 0x80 && !(pos[0] == ',' && (pos[1] == 0 || pos[1] == ' ')))
				return false;
			else if(pos[0] > 0x80 /*&& pos[2] != 0 && pos[2] != ' '*/)
				return false;
		}
	}
	if(sf==1 && (!strncmp(pInput+1,",",1) || !strncmp(pInput+1,".",1) || !strncmp(pInput+1,"/",1)))
	{	strncpy(cp, pInput+2,1); cp[1] = 0;
		sf = discriminate_char(cp);
		if(sf==1)//(sf>=1&&sf<=8)
			return false;
	}
	if((sf==3) && !strncmp(pInput+1,"/",1))
	{	strncpy(cp, pInput+2,1); cp[1] = 0;
		sf = discriminate_char(cp);
		if(sf==3)//(sf>=1&&sf<=8)
			return false;
	}
	if(!strncmp(pInput+1,".",1))
	{	
		sf = discriminate_char(pInput+2);
		if(sf==16 || (sf==9 && !strncmp(pInput+2,"\xa4\xa3",2)))/*��*/
			return false;
	}
	return true;
}

int CSymbolProcessor::get_symbol_tag(char *pInput, int bflag)//  bflag:���Ŵ���flag
{
	if(bflag==1)	{	return T_SNM;}
	else if(bflag==2)
	{
		if(!strcmp(pInput,","))
			return T_SCM;
		else if(!strcmp(pInput,".")||!strcmp(pInput,"?")||!strcmp(pInput,"!"))
			return T_SEN; 
		if(!strcmp(pInput,"[") ||!strcmp(pInput,"{")||!strcmp(pInput,"("))
			return T_SQL;
		else if(!strcmp(pInput,"]") ||!strcmp(pInput,"}")||!strcmp(pInput,")"))
			return T_SQR;
		else if(!strcmp(pInput,"%") ||!strcmp(pInput,"$") ||!strcmp(pInput,"US$"))
			return T_SUT;
		else
			return T_SWT;
	}
	else if(bflag==3)	{	
		if(!strcmp(pInput,"kg")||!strcmp(pInput,"mg")||!strcmp(pInput,"g")||!strcmp(pInput,"t")
			||!strcmp(pInput,"ml")||!strcmp(pInput,"l")||!strcmp(pInput,"t/h")
			||!strcmp(pInput,"km")||!strcmp(pInput,"cm")||!strcmp(pInput,"mm")||!strcmp(pInput,"m")
			||!strcmp(pInput,"cal")||!strcmp(pInput,"kcal")||!strcmp(pInput,"kWh")||!strcmp(pInput,"Wh")
			||!strcmp(pInput,"MPa")||!strcmp(pInput,"Pa")||!strcmp(pInput,"Hz")||!strcmp(pInput,"MHz")
			||!strcmp(pInput,"ppm")||!strcmp(pInput,"kVA")||!strcmp(pInput,"W")||!strcmp(pInput,"kW")
			||!strcmp(pInput,"eV")||!strcmp(pInput,"MeV")||!strcmp(pInput,"V")||!strcmp(pInput,"Gbit/s")
			||!strcmp(pInput,"km/h")||!strcmp(pInput,"m/min"))
			return T_SUT;
		else
			return T_SLG;
	}
	else if(bflag > 4 && bflag < 17)
	{
// 		if(!strcmp(pInput,"��") || !strcmp(pInput,"��"))
		if(!strcmp(pInput,"\xa1\xd4") || !strcmp(pInput,"\xa1\xc8"))
			return T_SQL;
// 		else if(!strcmp(pInput,"��")|| !strcmp(pInput,"��"))
		else if(!strcmp(pInput,"\xa1\xd5")|| !strcmp(pInput,"\xa1\xc9"))
			return T_SQR;
// 		else if(!strcmp(pInput,"��")|| !strcmp(pInput,"��"))
		else if(!strcmp(pInput,"\xa8\xac")|| !strcmp(pInput,"��")||!strcmp(pInput,"��")
			||!strcmp(pInput,"��")||!strcmp(pInput,"��")||!strcmp(pInput,"��"))
			return T_SUT;
		else
			return T_SWT;
	}
	else
		return 0;
}

int CSymbolProcessor::discriminate_char(char* csChar, bool check_EngUppLow)
{

	unsigned char cChar, cBChar;
	int nFlg = 0;

// 	int nLen = strlen(csChar);
	cChar = *csChar;
	
	if(check_EngUppLow)
	{
		if((cChar >= 0x41) && (cChar <= 0x5A))
			nFlg = 1;                                                    ///// ���� �ݷͻ�
		
		else if((cChar >= 0x61) && (cChar <= 0x7A))
			nFlg = 2;                                                    ///// ���� �Ϸͻ�  
		return nFlg;
	}

// 	if(nLen == 1)
	if(cChar < 0x80)
	{
		if((cChar >= 0x30) && (cChar <= 0x39))
			nFlg = 1;                                                     ///// �㰪

		else if((cChar < 0x30) || ((cChar >= 0x3A) && (cChar <= 0x40))
			                   || ((cChar >= 0x5B) && (cChar <= 0x60))
			                   || (cChar >= 0x7B))
			nFlg =  2;                                                    ///// ���� ����

		else if((cChar >= 0x41) && (cChar <= 0x5A))
			nFlg = 3;                                                    ///// ���� �ݷͻ�
		
		else if((cChar >= 0x61) && (cChar <= 0x7A))
			nFlg = 3; //  = 4;										 	 ///// ���� �Ϸͻ�  

	}
	else
	{
		cBChar = *(csChar+1); 

		if((cChar == 0xA3) && (cBChar >= 0xB0) && (cBChar <= 0xB9))
			nFlg = 5;                                                      ///// �������� �㰪

		else if(((cChar < 0xA3) && (cBChar >= 0xA1) && (cBChar <= 0xFE))
				|| ((cChar == 0xAC) && (cBChar >= 0xA1) && (cBChar <= 0xE0)))
			nFlg = 6;                                                      ///// ���� �������ⱨ��

		else if((cChar == 0xA3) 
			   && ((cBChar >= 0xC1) && (cBChar <= 0xDA)))
			nFlg = 7;                                                       ///// ���� ��������ݷͻ�
		
		else if((cChar == 0xA3) 
			   && ((cBChar >= 0xE1) && (cBChar <= 0xFA)))
			nFlg = 8;                                                       ///// ���� ��������Ϸͻ� 
		
		else if((cChar == 0xA4) 
			   && ((cBChar >= 0xA1) && (cBChar <= 0xE1)))
			nFlg = 9;                                                       ///// �������� ����, �ʾ� 
		
		else if((cChar == 0xA4) 
			   && ((cBChar >= 0xE8) && (cBChar <= 0xED)))
			nFlg = 10;                                                       ///// �������� ����
		
		else if(((cChar == 0xA5) && (cBChar >= 0xA1) && (cBChar <= 0xF1))     /////  ��������  ���̻���, 
			    || ((cChar == 0xA6) && (cBChar >= 0xA1) && (cBChar <= 0xD8))	/////   ��������  ����������,	
				||((cChar == 0xA6) && (cBChar >= 0xE1) && (cBChar <= 0xFA))		///	 ��������  �����
				||((cChar == 0xAC) && (cBChar >= 0xA1) && (cBChar <= 0xFA))	)	///	 �������� 
			nFlg = 11;                                                  

		else if((cChar == 0xA7) 
			   && ((cBChar >= 0xA1) && (cBChar <= 0xDE)))
			nFlg = 12;                                                       ///// �������� ��������ʯ̩ ����

		else if((cChar == 0xA7) 
			   && ((cBChar >= 0xE0) && (cBChar <= 0xFE)))
			nFlg = 13;                                                       ///// �������� ���� ˧ʭ�����

		else if((cChar == 0xA8) 
			   && ((cBChar >= 0xA1) && (cBChar <= 0xFE)))
			nFlg = 14;                                                       ///// �������� ���ǳ�̡

		else if(((cChar == 0xAA) && (cBChar >= 0xA1) && (cBChar <= 0xF3))
			   || ((cChar == 0xAB) && (cBChar >= 0xA1) && (cBChar <= 0xF6)))
			nFlg = 15;                                                       ///// �������� �θ��̻���

		else if(((cChar == 0xFF) && ((cBChar >= 0x01) || (cBChar <= 0x1F)))
		       || ((cChar == 0x20) && (cBChar >= 0x1D) )
		       || ((cChar == 0x30) && (cBChar >= 0x0B) ))
			nFlg = 16;                                                       ///// �������� �ͻ��鴸��, �ᱪ����
		else if(((cChar < 0xCC) && (cChar >= 0xB0) && (cBChar >= 0xA1) && (cBChar <= 0xFE))
		       || ((cChar == 0xCC) && (cBChar >= 0xA1) && (cBChar <= 0xCF)))
			nFlg = 17;                                                       ///// �������� ˧������
	}

	return nFlg;
}

bool CSymbolProcessor::check_F_E_symbol(char* pword)
{	
	char* p = pword;
	int sflag = discriminate_char(p);
	if(sflag != 17)
		return true;
	p = pword+strlen(pword)-2;
	sflag = discriminate_char(p);
	if(sflag == 17)
		return false;
	else
		return true;
}

int CSymbolProcessor::separate_sym(char* source, char* target, int *nFlag)
{	
	char onemorp[300] ;
	int sflag = -1, ln = 0, cnt=0;
	sflag = get_mophem_token(source, onemorp);
	target[0] = 0;
	char *p = target;
	while((int)strlen(onemorp) && (int)strlen(source) != ln)
	{
		nFlag[cnt] = sflag;
		strcpy(p,onemorp);
		p += strlen(onemorp)+1;
		ln += strlen(onemorp);
		sflag = get_mophem_token(source+ln, onemorp);
		cnt++;
		if(cnt==20)
			return -1;
	}
	return cnt;
}

void CSymbolProcessor::insert_space(char* source)
{	
	char *pp;
	if((pp = strstr(source, "��")) && *(pp+2) != 0)
	{
		memmove(pp+3, pp+2, strlen(pp+2)+1);
		*(pp+2) = 0x20;
	}	
	pp = source+strlen(source)-1;
	if(!strcmp(pp,".")||!strcmp(pp,"!")||!strcmp(pp,"?")||!strcmp(pp,","))
	{	memmove(pp+1, pp,strlen(pp-1));
		pp[0] = ' ';}
// 	pp = source+strlen(source)-2;
// 	if(!strcmp(pp,"��"))
// 	{	memmove(pp+1, pp,strlen(pp-1));
// 		pp[0] = ' ';}

	char* find_str1[15] = {",", ".","!","?",/*"(",")", "[", "]", "[", "]",*/ NULL};
	int i = 0;
	while(find_str1[i])
	{
		pp = strstr(source,find_str1[i]);
		while(pp)
		{
			// by hhj-29
//			if(possible_sep(pp-1)/* && *(pp+1) != ' '*/)
			if(possible_sep(pp)/* && *(pp+1) != ' '*/)
			// end
			{	if(*(pp+1) != ' '){
					memmove(pp+2, pp+1,strlen(pp));
					pp[1] = ' ';
				}
				memmove(pp+1, pp,strlen(pp-1));
				pp[0] = ' ';
			}
			pp+=2;
			pp = strstr(pp+1,find_str1[i]);
		}
		i++;
	}
/*
	char* find_str2[10] = {"��", "��", NULL};
	i = 0;
	while(find_str2[i])
	{//	char* sqr = strstr(pp,"��");	char* spc = strstr(pp," ");		if(spc && spc < sqr)	{
		pp = strstr(source,find_str2[i]);
		while(pp)
		{
			memmove(pp+1, pp, strlen(pp)+1);
			pp[0] = ' ';	pp++;
			if(*(pp+2) == 0)
				break;
			if(*(pp+2) != ' '){
				memmove(pp+3, pp+2,strlen(pp)+1);
				pp[2] = ' ';
			}
			pp+=3; 
			pp = strstr(pp,find_str2[i]);
		}
		i++;
	}
*/
	return;
}

void CSymbolProcessor::proc_string(char* source)
{	
	char* replace_ori[] = {"��޷���", "��밡",   "����","���ΰ�","���μ�", "ʿ�ʳ�",/*"��˺",*/"�ֻ�", "++", " �� ", " �� ", " ���׳�", " �㰡 ", /*" ���˳�",*/ " �ݵ� ", " ���� ", NULL};
	char* replace_new[] = {"��� ����", "����Ͱ�","����ͼ�","�����Ͱ�","�����ͼ�","ʿ �ױʳ�", /*"��", */"�� ��", "����", "�� ", "�� ", "���׳�", "�㰡 ", /*"���˳�", */"�ݵ� ", "���� ", NULL};
	for(int i=0; replace_ori[i]; i++)
	{
		replace_str(source,replace_ori[i], replace_new[i]);
	}
	char *p=strstr(source,"˺");
	if(p && p!=source)
	{
		char* hI_str1[10] = {"˺��","˺��",/*"˺��","˺��",*/"˺���",NULL};
		char* hI_str2[10] = { "��",   "��",  /*"��",  "��", */ "���", NULL};
		p-=2;
		if(strncmp(p,"��",2))
		{
			for(int j=0; hI_str1[j]; j++)
				replace_str(source, hI_str1[j], hI_str2[j]);
		}
	}
	return;
}

void CSymbolProcessor::replace_str(char *src, char *old_str, char *new_str)
{
	char *po = src;
	while(*po)
	{	char *po1 = strstr(po, old_str);
		if(po1 == 0)
			break;
		// by hhj-41
		if(strcmp(old_str, " �㰡 ") == 0 && strncmp(po1-2, "̩", 2) == 0)
		{
			memcpy(po1, old_str, strlen(old_str));
			po = po1 + strlen(old_str);
		}
		else
		{
		// end
			if(strlen(old_str) != strlen(new_str))
				memmove(po1+strlen(new_str), po1+strlen(old_str), strlen(po1) + 1 - strlen(old_str));
			memcpy(po1, new_str, strlen(new_str));
			po = po1 + strlen(new_str);
		}
	}
}

bool CSymbolProcessor::check_symbol(char *source, int* tagnum, int* len)
{
	char onemorp[300] ;
	int sflag = -1;
	sflag = get_mophem_token(source, onemorp);
	if((sflag == 0||sflag == 17) && strlen(source) == strlen(onemorp))
		return false;
	*tagnum = get_symbol_tag(onemorp,sflag);
	*len = strlen(onemorp);
	return true;
}
