#include "Stdafx_engine.h"
#include "StringTokenizer.h"

void cst_trim(char *str, short * offsets);
void cst_trim(char *str)
{
	cst_trim(str, NULL);
}

void cst_trim(char *str, short * offsets)
{
	if(strlen(str) == 0)
		return;
	char *po1, *po2;
	po1 = str;
	while(*po1 > 0 && *po1 <=' ')
		po1++;
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
	{	memmove(str, po1, po2-po1+1);
		if(offsets != NULL)
			memmove(offsets, offsets + (po1 - str), (po2-po1) * 2);
	}
}

StringTokenizer::StringTokenizer(const char *str0)
{

	strcpy(this->delimit, " ");
    length = strlen(str0);
	str = (char *)malloc(length + 1);
	strcpy(str, str0);
	index = 0;
    while(index < length && strchr(delimit, str[index]) != NULL)
      index++;
	kind = 0;
}

StringTokenizer::StringTokenizer(const char *str0, const char c)
{

	delimit[0] = c;
	delimit[1] =  0;
    length = strlen(str0);
	str = (char *)malloc(length + 1);
	strcpy(str, str0);
	index = 0;
	kind =  1;

}

StringTokenizer::StringTokenizer(const char *str0,  const char *syms)
{
	strcpy(delimit, syms);
    length = strlen(str0);
	str = (char *)malloc(length + 1);
	strcpy(str, str0);
	index = 0;
    while(index < length && strchr(delimit, str[index]) != NULL)
      index++;
	kind = 0;
}

StringTokenizer::StringTokenizer(const char *str0,  const char *syms, const char *exc)
{
	strcpy(delimit, syms);
	strcpy(except,exc);
    length = strlen(str0);
	str = (char *)malloc(length + 1);
	strcpy(str, str0);
	index = 0;
    while(index < length && strchr(delimit, str[index]) != NULL)
      index++;
	kind = 2;
}

StringTokenizer::~StringTokenizer()
{
	if(str != NULL)
		free(str);
}

const char *StringTokenizer::nextToken()
{	
	if(kind == 0 && index >=length)
		return  NULL;
	else if(kind == 1 && index >length)
		return NULL;
	else if(kind == 1 && index == length)
	{	index++;
		return str + index -1;
	}

    int i=index;
	bool bexcept = false;
	while(i < length && strchr(delimit, str[i]) == NULL)
	{
		if(kind==2&&i>index&&except[0]==str[i])
		{	bexcept = true;break;}
		i++;
	}
    if(bexcept)
	{
		while (i<length && except[1]!=str[i])
			i++;
		while(i<length && strchr(delimit,str[i])==NULL)
			i++;
	}
	if(i > length) return NULL;
	str[i] = 0;

	char *po = str + index;
	cst_trim(po);
    index = i+1;
	if(kind == 0)
	{	while(index < length && strchr(delimit, str[index]) != NULL)
		 index++;
	}
    return po;

}

StringTokenizerWith2Bracket::StringTokenizerWith2Bracket(const char *str0)
{
    length = strlen(str0);
	str = (char *)malloc(length + 1);
	strcpy(str, str0);
	index = 0;
    while(index < length-1 && !(str[index] == '{' && str[index+1] == '{'))
      index++;
	index+=2;
}
StringTokenizerWith2Bracket::~StringTokenizerWith2Bracket()
{
	if(str != NULL)
		free(str);
}

const char *StringTokenizerWith2Bracket::nextToken()
{
	if(index >length)
		return  NULL;
    int i=index;
    int co = 0;
    while(i < length-1)
    { 
      if(str[i] == '}' && str[i+1] == '}')
      { 
			break;
      }
      i++;
    }
    if(i >= length-1) return NULL;
    str[i] = 0;
	char *po = str + index;
	cst_trim(po);
    index = i+1;
    while(index < length-1 && !(str[index] == '{' && str[index+1] == '{'))
      index++;
	index+=2;
    return po;
}
/*
void trim(char  *str)
{	int i=0;
	while(str[i] != 0 && str[i] == ' ')
		i++;
	if(str[i]==0)
	{	str[0]=0;	return;  }
	int j = strlen(str)-1;
	while(j >=0 && str[j] == ' ')
		j--;
	if(i>0)
	{	memmove(str, str+i, j-i+1);
		str[j-i+1]=0;
	}
	else
		str[j+1]=0;

}
*/
StringTokenizerWithBracket::StringTokenizerWithBracket(const char *str0, char c_open, char c_close)
{	m_open = c_open;
	m_close = c_close;

    length = strlen(str0);
	str = (char *)malloc(length + 1);
	strcpy(str, str0);
	index = 0;
    while(index < length && str[index] != m_open)
      index++;
    index++;
}
StringTokenizerWithBracket::~StringTokenizerWithBracket()
{
	if(str != NULL)
		free(str);
}

const char *StringTokenizerWithBracket::nextToken()
{
	if(index >length)
		return  NULL;
    int i=index;
    int co = 0;
    while(i < length)
    { if(str[i] == m_open)
        co++;
      if(str[i] == m_close)
      { if(co >0)
          co--;  

        else
          break;
      }
      i++;
    }
    if(i >= length) return NULL;
    str[i] = 0;
	char *po = str + index;
	cst_trim(po);
    index = i+1;
    while(index < length && str[index] != m_open)
      index++;
    index++;
    return po;
}

StringTokenizerByStr::StringTokenizerByStr(const char *str0, const char *tokenstr)
{	

	length = strlen(str0);
	index = 0;

	m_oristr = str0;	
	str = NULL;
	m_tokenstr = NULL;

	if(strstr(str0, tokenstr) == 0)
		return;
	
	m_tokenstr = (char *)malloc(strlen(tokenstr)+1);
	strcpy(m_tokenstr, tokenstr);
	m_tokenlen = strlen(tokenstr);
	
	str = (char *)malloc(length + 1);
	strcpy(str, str0);
}
StringTokenizerByStr::~StringTokenizerByStr()
{	if(m_tokenstr != NULL)
		free(m_tokenstr);
	if(str != NULL)
		free(str);
}

const char *StringTokenizerByStr::nextToken()
{
	if(index >=length)
		return  NULL;
	if(!m_tokenstr)
	{	index = length;
		return m_oristr;
	}
    int i=index;
    while(i < length && strncmp(str + i, m_tokenstr, m_tokenlen) != 0)
      i++;
    str[i] = 0;
	char *po = str + index;
	cst_trim(po);
    index = i+m_tokenlen;
    return po;
}


