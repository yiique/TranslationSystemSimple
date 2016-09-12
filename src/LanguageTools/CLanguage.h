#ifndef CLANGUAGE_H
#define CLANGUAGE_H

//MAIN
#include "Common/f_utility.h"
#include "TransText/TransText.h"
#include "Common/Utf8Stream.h"
#include "Configurator/Configurator.h"
#include "Log/LogStream.h"
#include "Common/BaseMutex.h"
#include "LanguageTools/uyghur/uytoken/UyToken.h"
#include "ZDQToken.h"
#include "LanguageTools/uyghur/uyconv/COldLadConv.h"
#include "LanguageTools/uyghur/uyidentify/CCodeRec.h"
#include "LanguageTools/tibet/tibetseg/Segmentor.h"
#include "LanguageTools/tibet/tibetsplitsent/Sentenizer.h"
#include "LanguageTools/tibet/tibetnumber/NumberPair.h"
#include "LanguageTools/Mongolian/Mo2Latin/Mo2Latin.h"
#include "LanguageTools/Mongolian/MoNumDate/MoNumDate.h"
#include "LanguageTools/Mongolian/NewLowercase/Cy2Lower.h"
#include "LanguageTools/Vietnam/VietnamLower.h"
#include "LanguageTools/Mongolian/OldMToken/RulesTable.h"
#include "LanguageTools/japanese/token.h"
#include "Configurator/Configurator.h"
#include "thai/Segmentor.h"
#include "LanguageTools/uyghur/uynetrans/NewUyNERec.h"
#include "LanguageTools/CodeConver.h"
#include "KMA_DLL.h"

//STL
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

/*class CLanguage
{
public:
	CLanguage(void) {};
	~CLanguage(void) {};

	static bool InitStaticResource(const Configurator & config);

	static string IdentifyChEn(const string & src);
	static void Utf8A2B(string & src);
	static void Utf8A2B(string & src, vector<BaseAlign> & char_align_vec);
	static void RemoveChPosTag(string & src);
	

	static void ChSegment(const string & src, string & tgt);
	static void ChSegment(const string & src, string & tgt, const set<string> & force_dict);
	static void EnToken(const string & src, string & tgt);
	static string & EnLowercase(string & str);


	static bool IsUyghurLatin(const string & src);
	static void UyToken(const string & src, string & tgt); // ά��token����
	static void UyghurOld2Latin(string & src); // ��ά��ת����ά�Ĳ���
	static void UyNetransLatin(const string & src, vector<string> & result_vec); // ά������ʱ���ʶ�����Ϊresult_vec����Ĺ���

	static string & Mongolian2Latin(string & src);
	static void MongolianNumDate(const string & src, vector<string> & result_vec);
	static string & NEWMongolianLower(string & src);

	static string & VietnamLowercase(string & src);
	static string & ThaiSegment(string & src);

	static void TibetSegment(const string & src, string & tgt);
	static void TibetSplitSent(const string & src, vector<string> & sent_vec);
	static void TibetNumberTrans(const string & src, vector<string> & result_vec);

	static string & En2ChPost(string & str);
	static string & Ch2EnPost(string & str);
	
	static string & SpliteStrByChar(string & str);

	static string MongolianToken(string & str);

	static string RussianLowercase(const string& str);

	static string JapaneseSeg(string& str);

	static string KoreanSeg(string& str);

private:

	static MutexLock ms_en_token_lock; //Ӣ��token������
	static ZTokenizer ms_en_token;     //Ӣ��token
	static COldLadConv ms_uy_old2latin;  //ά�Ĵ�ͳת����
	static CCodeRec    ms_uy_identify;   //ά�Ĵ�ͳ����ʶ��
	static titoken::CSegmentor ms_tibet_segmentor; //����ִ�
	static tibetsentenizer::Sentenizer ms_tibet_sentsplitor; //����Ͼ�

	static MutexLock ms_uy_ne_lock;
	static NewUyNERec ms_uy_netrans; //ά������ʵ��ʶ��
	static string uy_netrans_prob; //����

	static MutexLock ms_tibet_number_lock;
	static TiNumber::NumberPair ms_tibet_number; //��������ʶ�� 

	static MutexLock ms_mo2latin_lock;
	static CodeConverNameSpace::Mo2Latin ms_mo2latin;

	static MutexLock ms_monumdate_lock;
	static MoNumDate::CMoNumDate ms_monumdate;

	static MutexLock ms_newmo_lowercase_lock;
	static Cy2Lower::CCy2Lower ms_newmo_lowercase;

	static VietnamLower ms_vietnam_lower;

	static string & en_token_post(string & src);

	static MutexLock ms_mongolian_token_lock;
	static RulesTable ms_mongolian_token;

	//
	static string & utf8_char_a2b(string & utf8_char);

	//�������ĺ���
	static bool is_ascii(const string & src);

	//����Ӣ�ĺ���
	static void capitalize(string& str); //����ĸ��д
	static void capitalize1(string& str); //�ض������ź�Ĵʴ�д
	static void punc(string& str); //���������
	static void str_replace(string &str, string _from, string _to);

	//̩��ִ�
	static thaiSeg::CSegmentor thai_segmentor_;

	// �����Сдת��
	static CodeConver ms_russian_lowercase;

	// ����ִ�
	static JPTOKEN::token ms_japanese_seg;

	//korean seg
	static KMA_DLL m_kma;
};*/


class CLanguage
{
public:
	static void Utf8A2B(string & src);
	static string & SplitStrByChar(string & str);
	static void StandardSentFilter(SentProcCell & sent_cell);
private:
	static string & utf8_char_a2b(string & utf8_char);
};


#endif //CLANGUAGE_H
