//////////////////////////////////////////////////////////////////////
//	File : KUnicode.H
//	Description:
//		KUnicode Class Description Header
//	History:
//		Updated	by Pak Son Il
//////////////////////////////////////////////////////////////////////

//
	/***********************************
	Korean Charset range
	************************************
	A1A0 A1A1 ............ A1FE A1FF
	A2A0 A2A1 ............ A2FE A2FF
	................................
	CCA0 CCA1 ............ CCFE CCFF
    -------------------------------- Chinese
	CDA0 CDA1 ............ CDFE CDFF
    ................................
	FEA0 FEA1 ............ FEFE FEFF

	Chinese Charset range
	************************************
	1) General
	A1A0 A1A1 ............ A1FE A1FF
	A2A0 A2A1 ............ A2FE A2FF
	................................
	A9A0 A9A1 ............ A9FE A9FF
  
	2) Special
	B0A0 B0A1 ............ B0FE B0FF
	B1A0 B1A1 ............ B1FE B1FF
	................................
	F7A0 F7A1 ............ F7FE F7FF

	Japanese Charset range
	************************************
	1) General
	8140 8141 ............ 81FE 81FF
	8240 8241 ............ 82FE 82FF
	................................
	8440 8441 ............ 84FE 84FF
  
	2) Special
	8740 8741 ............ 87FE 87FF

	3) 1 Lebel Japanese Charset
	8840 8841 ............ 88FE 88FF
	8940 8941 ............ 89FE 89FF
	................................
	9F40 9F41 ............ 9FFE 9FFF

 	4) 2 Lebel Japanese Charset
	E040 E041 ............ E0FE E0FF
	E140 E141 ............ E1FE E1FF
	................................
	EA40 EA41 ............ EAFE EAFF

	5) Expanded Japanese Charset
	ED40 ED41 ............ EDFE EDFF
	EE40 EE41 ............ EEFE EEFF
	................................
	EE40 EE41 ............ EEFE EEFF

	************************************/
//////////////////////////////////////////////////////////////////////

#if !defined(__KUNICODE_H__)
#define __KUNICODE_H__

// KR Common Include File
//#include "KDBFC.h"

#define K_LOW_RANGE		0x60
#define K_HIGH_RANGE	0x5E
#define K1_HIGH_RANGE	0x2C

#define K_START_HIGH	0xA1
#define K_START_LOW		0xA0

#define C_LOW_RANGE		96
#define C1_HIGH_RANGE	9
#define C2_HIGH_RANGE	72

#define C1_START_HIGH	0xA1
#define C1_START_LOW	0xA0
#define C2_START_HIGH	0xB0
#define C2_START_LOW	0xA0

#define J_LOW_RANGE		192
#define J1_HIGH_RANGE	4
#define J2_HIGH_RANGE	1
#define J3_HIGH_RANGE	24
#define J4_HIGH_RANGE	11
#define J5_HIGH_RANGE	2

#define J1_START_HIGH	0x81
#define J1_START_LOW	0x40
#define J2_START_HIGH	0x87
#define J2_START_LOW	0x40
#define J3_START_HIGH	0x88
#define J3_START_LOW	0x40
#define J4_START_HIGH	0xE0
#define J4_START_LOW	0x40
#define J5_START_HIGH	0xED
#define J5_START_LOW	0x40

#define LU_SIZE	K_LOW_RANGE * K_HIGH_RANGE + C_LOW_RANGE * (C1_HIGH_RANGE + C2_HIGH_RANGE) + J_LOW_RANGE * (J1_HIGH_RANGE + J2_HIGH_RANGE + J3_HIGH_RANGE + J4_HIGH_RANGE + J5_HIGH_RANGE) 

///// for  kor code convert
	// kukyu to tangun and to mac
#define KT_LOW_RANGE	96
#define KT1_HIGH_RANGE	8
#define KT2_HIGH_RANGE	30   // 29 + 1 (1: for mac symbol)
#define KT3_HIGH_RANGE	10   // for mac hanja (10 * 2 * 96 = 1920)
							 // ¹¾°äÂ×²÷ ÂÙ»ôºã = 480
							 // ¿¸´ÅÁì = °ê°öÂÙ»ô + ´ÝËÄÂÙ mac ÂÙ»ô = 480 + 480 = 960 (Âï¼õ 11 ±¡»ô ÊÞË±ËØËÁ)
							 // ¿¸´ÅÁì byte = 960 * 2 = 1920(byte)   (2 : 1word=2byte)
#define KT1_START_HIGH	0xA1
#define KT2_START_HIGH	0xB0
#define KT3_START_HIGH	0xAC // for mac symbol
#define KT4_START_HIGH	0x88 // for hanja    start high byte 
#define KT4_END_HIGH	0x9F // for hanja    end   high byte 
#define KT5_START_HIGH	0xCD // for mac hanja    start high byte 
#define KT5_END_HIGH	0xFE // for mac hanja    end   high byte 
#define KT_START_LOW	0xA0
							// *** content of k_tanj table ***
							//	0    ..  1BBF	TangunJ code
							//	1BC0 ..  1C80	for mac symbol
							//	1C80 + 0x3c0 = 2040  (0x3C0 = 960) |
							//	1C80 + 0x780 = 2400  (0x780 = 1920)|-> for mac hanja
	// tangun to kukyu
#define TK_LOW_RANGE	192
#define TK1_HIGH_RANGE	7
#define TK2_HIGH_RANGE	13

#define TK1_START_HIGH	0x81
#define TK2_START_HIGH	0xF0
#define TK_START_LOW	0x40



/////
#define  U_KOR 1
#define  U_CHI 2
#define  U_JAP 3
#define  U_RUS 4
#define  U_EUR 5
/// code convert mode in korean 
#define	 L_K_UNI			1
#define	 L_UNI_K			2
#define	 L_K_KS				3
#define	 L_KS_K				4
#define	 L_K_TAN			5
#define	 L_TAN_K			6
#define  L_K_MAC			7
#define  L_MAC_K			8
#define  L_K_UTF8			9
#define  L_UTF8_K			10
/// code convert to mac for russian, france, german 
#define	 MAC_ANSI			1
#define	 MAC_RUS			2
#define	 MAC_KOR			3

class	KUnicode
{
public:
	KUnicode();

	bool	SetCodeTableFile(char* TableFileName);

	int		GetKorUTF8CodeString(const char *localString, unsigned char *unicodeString, int nMode = 0);
											// nMode = 1 for output to *.htm file ( in first = 0xEFBBBF)
											// nMode = 0 convert string
	int		GetKorConvString(const char *sourceString, char *targetString, int arealNum);
	int		GetUTF8_KorCodeString(unsigned char* sourceString, unsigned char* targetString);
	unsigned short	KorUnicode(unsigned short localCode, int arealNum);
	unsigned short	UnicodeKor(unsigned short sourceCode, int arealNum);
	unsigned short	UTF8Kor(char* sourceCode);
	unsigned char* KorUTF8(unsigned short localCode);
	
	virtual ~KUnicode();

protected:
	bool	IsUnicodeTableOpened;
	unsigned char m_UTF8code[5];

	struct LUTable
	{
		unsigned short KUNITable[K_HIGH_RANGE][K_LOW_RANGE];
		unsigned short K_KSTable[K_HIGH_RANGE][K_LOW_RANGE];
		unsigned short KTANTable[KT1_HIGH_RANGE + KT2_HIGH_RANGE - 1][KT_LOW_RANGE];
		unsigned short KMACTable[KT1_HIGH_RANGE + KT2_HIGH_RANGE + KT3_HIGH_RANGE][KT_LOW_RANGE];
		unsigned short TANKTable[TK1_HIGH_RANGE + TK2_HIGH_RANGE][TK_LOW_RANGE];
		unsigned short CTable1[C1_HIGH_RANGE][C_LOW_RANGE];
		unsigned short CTable2[C2_HIGH_RANGE][C_LOW_RANGE];
		unsigned short JTable1[J1_HIGH_RANGE][J_LOW_RANGE];
		unsigned short JTable2[J_LOW_RANGE];
		unsigned short JTable3[J3_HIGH_RANGE][J_LOW_RANGE];
		unsigned short JTable4[J4_HIGH_RANGE][J_LOW_RANGE];
		unsigned short JTable5[J5_HIGH_RANGE][J_LOW_RANGE];
		char KUTF8Table[K1_HIGH_RANGE][K_LOW_RANGE][3];
		char CUTF8Table[C1_HIGH_RANGE + C2_HIGH_RANGE][C_LOW_RANGE][3];
	};
	LUTable	m_LocalOrderedUnicode;
};

#endif // !defined(__KUNICODE_H__)
