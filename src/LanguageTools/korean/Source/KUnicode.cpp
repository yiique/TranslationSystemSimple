//////////////////////////////////////////////////////////////////////
//	File KUnicode.CPP
//	Description:
//		KUnicode Class Implementation
//	History::
//		Created	by  Pak Son Il
//////////////////////////////////////////////////////////////////////


#include "Stdafx_engine.h"
#include "KUnicode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUnicode::KUnicode()
{
	IsUnicodeTableOpened = false;
}

KUnicode::~KUnicode()
{

}
bool KUnicode::SetCodeTableFile(char *TableFileName)
{
	FILE  *fp;
	fp=fopen(TableFileName,"rb");
	if(!fp)
		return false;

	unsigned int size = sizeof(LUTable);

	if(fread(&m_LocalOrderedUnicode,1,size,fp) < size)
		IsUnicodeTableOpened = false;
	else
	{
		//AfxMessageBox("Local Ordered Unicode Table Reading Error!");
		fclose(fp);
		IsUnicodeTableOpened = true;
	}
	return IsUnicodeTableOpened;
}
unsigned short KUnicode::KorUnicode(unsigned short localCode, int arealNum)
{
	unsigned short	result = localCode;
	
	unsigned short local = localCode;
	local = local<<8;
	int highByte	= local>>8;
	int lowByte	= localCode>>8;

	if(highByte >=	K_START_HIGH && 
		highByte <= K_START_HIGH + K_HIGH_RANGE &&
		lowByte	>=	K_START_LOW &&
		lowByte	<=	K_START_LOW  + K_LOW_RANGE)
	{
		switch(arealNum)
		{
			case L_K_UNI:	result = m_LocalOrderedUnicode.KUNITable[highByte - K_START_HIGH][lowByte - K_START_LOW]; break;
			case L_K_KS:	result = m_LocalOrderedUnicode.K_KSTable[highByte - K_START_HIGH][lowByte - K_START_LOW]; break;
		}
	}

	return result;

}

unsigned char* KUnicode::KorUTF8(unsigned short localCode)
{
	m_UTF8code[0] = 0;
	char*	pTemp;
	
	unsigned short local = localCode;
	local = local<<8;
	int highByte	= local>>8;
	int lowByte	= localCode>>8;

	if(highByte >=	K_START_HIGH && 
		highByte <= K_START_HIGH + K1_HIGH_RANGE &&
		lowByte	>=	K_START_LOW &&
		lowByte	<=	K_START_LOW  + K_LOW_RANGE)
	{
		pTemp = (char*)&m_LocalOrderedUnicode.KUTF8Table[highByte - K_START_HIGH][lowByte - K_START_LOW][0];
		strncpy((char*)m_UTF8code, pTemp, 3);
		m_UTF8code[3] = 0;
	}

	return m_UTF8code;
}
//	convert  form DPRK  to UTF8
int KUnicode::GetKorUTF8CodeString(const char *localString, unsigned char *unicodeString, int nMode)
{
	int len = strlen(localString);
	unsigned char* plocalString = (unsigned char*)localString;

	int count = 0;

	if (nMode == 1) // for output to file
	{
		unicodeString[count]	 = 0xEF;
		unicodeString[count + 1] = 0xBB;
		unicodeString[count + 2] = 0xBF;
		count += 3;
	}
	unsigned short* pTemp;

	while (len > 0)
	{
		if(*plocalString < 0x80)
		{
			unicodeString[count] = *plocalString;
			plocalString++;
			count++;
			len--;
		}
		else
		{
			pTemp = (unsigned short*)plocalString;
			strncpy((char*)unicodeString + count, (char*)KorUTF8(*pTemp), 3);
			plocalString += 2;

			//	in 2byte utf-8 (russion code, greece,...)
			if(unicodeString[count + 2] == 0x00)
				count += 2;
			else // in 3byte utf-8 (korea code)
				count +=3;
			len -= 2;
		}
	}

	unicodeString[count] = 0;

	return count;
}
//  convert from KS, DPRK, TANJ  to DPRK, KS, TANJ, MAC
int KUnicode::GetKorConvString(const char *sourceString, char *targetString, int arealNum)
{
	int len = strlen(sourceString);
	unsigned char* plocalString = (unsigned char*)sourceString;

	int count = 0;
	int pos = 0;
	
	unsigned short*	pTemp;
	unsigned short	oneWord;

	while (len > 0)
	{
		if(*plocalString < 0x80 || len == 1)
		{
			targetString[pos] = *plocalString;
			plocalString++;
			count++;
			pos++;
			len--;
		}
		else
		{
			pTemp = (unsigned short*)plocalString;
			switch(arealNum)
			{
				case L_K_UNI:
				case L_K_KS:
					oneWord = KorUnicode(*pTemp, arealNum);
					break;
				case L_KS_K:
				case L_UNI_K:
					oneWord = UnicodeKor(*pTemp, arealNum);
					break;
			}
			unsigned short one = oneWord;
			one = one<<8;
			targetString[pos]	= one>>8;
			targetString[pos+1]	= oneWord>>8;

			plocalString += 2;
			count++;
			pos +=2;
			len -= 2;
		}

	}

	targetString[pos] = 0;

	return count;
}
//	convert one word from unicode to local code and ks to dprk
unsigned short KUnicode::UnicodeKor(unsigned short sourceCode, int arealNum)
{
	unsigned short result = sourceCode;

	unsigned short sou = sourceCode;
	sou = sou<<8;
	int lowByte	= sou>>8;
	int highByte	= sourceCode>>8;
	unsigned short* CodeTable;
	int bLow;
	int bHigh;

	switch(arealNum)
	{
		case L_UNI_K:
			CodeTable = &m_LocalOrderedUnicode.KUNITable[0][0]; break;
		case L_KS_K:
			CodeTable = &m_LocalOrderedUnicode.K_KSTable[0][0]; break;
	}
	for(bHigh = 0; bHigh < K_HIGH_RANGE; bHigh++)
		for(bLow = 0; bLow < K_LOW_RANGE; )
		{
			if (sourceCode == (unsigned short)CodeTable[bHigh * K_LOW_RANGE + bLow])
				goto bb;
			bLow ++;
		}
bb:	if(arealNum == L_UNI_K) // in unicode low, high
	{
	//	result = MAKEWORD( bLow + K_START_LOW, bHigh + K_START_HIGH );
		unsigned short h = bHigh + K_START_HIGH;
		unsigned short l = bLow + K_START_LOW;
		l = l*0xff+l;
		result = h +l;
	}
	else					// ¼ÁÂÜ½å¶®
	{
		if(highByte == 0xE8 && lowByte ==0xB1 )//highByte == 0xB1 && lowByte == 0xE8)
		{
			bHigh = 0xB1 - K_START_HIGH;
			bLow  = 0xAE - K_START_LOW;
		}
		else if(highByte == 0xCF && lowByte ==0xC0 )
		{
			bHigh = 0xCB - K_START_HIGH;
			bLow  = 0xCE - K_START_LOW;
		}
		else if(highByte == 0xBA && lowByte ==0xBC )
		{
			bHigh = 0xBA - K_START_HIGH;
			bLow  = 0xC2 - K_START_LOW;
		}
		else if(highByte == 0xA4 && lowByte ==0xC1 )
		{
			bHigh = 0xBC - K_START_HIGH;
			bLow  = 0xB3 - K_START_LOW;
		}
							// ks->k      high, low
//		result = MAKEWORD( bHigh + K_START_HIGH, bLow + K_START_LOW );
		unsigned short h = bHigh + K_START_HIGH;
		unsigned short l = bLow + K_START_LOW;
		l = l*0xff+l;
		result = h +l;
	}
	return result;
}
//	convert one word from utf8 to dprk
unsigned short KUnicode::UTF8Kor(char* sourceCode)
{
	unsigned short	result = 0x2020;
		
	int bLow;
	int bHigh;

	char*	CodeTable;
	char*	pTemp;

	CodeTable = &m_LocalOrderedUnicode.KUTF8Table[0][0][0];

	for(bHigh = 0; bHigh < K1_HIGH_RANGE; bHigh++)
		for(bLow = 0; bLow < K_LOW_RANGE; )
		{
			pTemp = (char*)CodeTable[(bHigh * K_LOW_RANGE + bLow)*3];
			if (strncmp(sourceCode, (char*)&m_LocalOrderedUnicode.KUTF8Table[bHigh][bLow][0], 3) == 0)
				goto bb;
			bLow ++;
		}
//bb:	result = MAKEWORD(bLow + K_START_LOW, bHigh + K_START_HIGH);
bb:		unsigned short h = bLow + K_START_LOW;
		unsigned short l = bHigh + K_START_HIGH;
		l = l*0xff+l;
		if(h > 0xff)
			result = 0x20;
		else
			result = h +l;

	return result;
}
//	convert from UTF8 to DPRK
int KUnicode::GetUTF8_KorCodeString(unsigned char* sourceString, unsigned char* targetString)
{
	int len = strlen((char*)sourceString);
	unsigned char* pUTF8String = sourceString;

	int count = 0;
	int pos = 0;
	
	unsigned short	oneWord;
	char	szTemp[3];
	szTemp[0] = (char)0xEF;
	szTemp[1] = (char)0xBB;
	szTemp[2] = (char)0xBF;

	if(strncmp((const char*)pUTF8String, (const char*)szTemp, 3) == 0)
	{
		pUTF8String += 3;
		len -= 3;
	}
//printf("len:%d\n", len);
	while (len > 0)
	{
		if(pUTF8String[0] < 0x80)
		{
			targetString[pos] = pUTF8String[0];
			pos++;
			count++;
			len --;
//printf("a\t%d\t%d\t%d\n", len,pos,count);
			pUTF8String ++;
		}
		else
		{
			oneWord = UTF8Kor((char*)pUTF8String);

			unsigned short pp = oneWord;
			if(pp != 0x20)
			{
				pp =pp<<8;
				targetString[pos+1]   = pp>>8;
				targetString[pos] = oneWord>>8;

				pos   += 2;
				count += 2;
				len -= 3;
//printf("b\t%d\t%d\t%d\n", len,pos,count);
				pUTF8String += 3;
			}
			else
			{
				targetString[pos]   = pp;
				pos   ++;
				count ++;
				len --;
//printf("c\t%d\t%d\t%d\n", len,pos,count);
				pUTF8String ++;
			}

		}
	}
	targetString[pos] = 0;
//printf("targetString%s\n",targetString);
//printf("count%d\n",count);
	return count;
}
