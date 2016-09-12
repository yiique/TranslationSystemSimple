
#if !defined(_TRANS_DLL_)
#define _TRANS_DLL_
	
//#define _USRDLL

/*
#define _USRDLL
#define _MACOS
#define _CP
#define _NOPICUD
*/

#ifdef _MACOS
#pragma options align = mac68k
#else
#pragma pack(1)
#endif

#ifdef _MACOS
#pragma options align = mac68k
#else
#pragma pack(1)
#endif


#if !defined (_USRDLL)

#define VC_EXTRALEAN		

#include <afxwin.h>         
#include <afxext.h>         
#include <afxdisp.h>        
#include <afxdtctl.h>		
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			
#endif 

#include <afxsock.h>

class CDisplayIC : public CDC
{
public:
	CDisplayIC() { CreateIC(_T("DISPLAY"), NULL, NULL, NULL); }
};


#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#if defined (_WIN32)
#include <windows.h>
#endif

#ifdef _MACOS
extern long cst_g_Architecture;
#endif

#if !defined (_WIN32)
#define MAX_PATH 256
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define WORD    unsigned short
#define BYTE       unsigned char
#define CHAR     char

inline char *itoa(int i, char *res, int mode)
{
    if(mode == 10)
	{	sprintf(res, "%d", i);
		return res;
	}
	else if(mode == 16)
	{	sprintf(res, "%x", i);
		return res;
	}
	return NULL;
}

inline int strnicmp(const char *str1, const char *str2, int len)
{
	char *s1;
	char *s2;
	if(!len)
	{	s1 = (char *)malloc(strlen(str1) +1);
		strcpy(s1, str1);		
		s2 = (char *)malloc(strlen(str2) +1);
		strcpy(s2, str2);
	}
	else
	{	s1 = (char *)malloc(len +1);
		strncpy(s1, str1, len);		
		s2 = (char *)malloc(len +1);
		strncpy(s2, str2, len); 
	}
	int i;
	for(i=0; i<strlen(str1); i++)
	{	if(s1[i] >= 'A' && s1[i] <= 'Z')
			s1[i] += 0x20;
	}
	for(i=0; i<strlen(str2); i++)
	{	if(s2[i] >= 'A' && s2[i] <= 'Z')
			s2[i] += 0x20;
	}
	int n=strcmp(s1, s2);
	free(s1);
	free(s2);
	return n;
}

inline int stricmp(const char *str1, const char *str2)
{
	return strnicmp(str1,str2,0);
}

#endif

#endif

#ifdef _MACOS
#endif

inline void cst_reverse_int(int *val)
{
#ifdef _MACOS
	if (cst_g_Architecture == gestaltIntel)
		return;
#endif
	int n1 =(*val & 0xff) << 24;
    int n2 =(*val & 0xff00) << 8;
    int n3 =(*val & 0xff0000) >> 8;
    int n4 =((*val & 0xff000000) >> 24) & 0xff;
    *val = n1 | n2 | n3 | n4;


}

inline void cst_reverse_short(short *val)
{
#ifdef _MACOS
	if (cst_g_Architecture == gestaltIntel)
		return;
#endif
	int n1 =((int)*val & 0xff) << 8;
    int n2 =(((int)*val & 0xff00) >> 8) & 0xff;
    *val = (short)(n1 | n2);


}

inline void cst_reverse_float(float *val)
{
#ifdef _MACOS
	if (cst_g_Architecture == gestaltIntel)
		return;
#endif
	char nn[4];
	memcpy(nn, val, 4);
	char c;
	c = nn[3];
	nn[3] = nn[0]; nn[0] = c;
	c = nn[2]; 
	nn[2] = nn[1]; nn[1] = c;
    *val = *(float *)nn;

}


#endif 
