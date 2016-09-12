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
	static void UyToken(const string & src, string & tgt); // 维语token部分
	static void UyghurOld2Latin(string & src); // 老维文转拉丁维文部分
	static void UyNetransLatin(const string & src, vector<string> & result_vec); // 维文数词时间词识别，输出为result_vec里面的规则

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

	static MutexLock ms_en_token_lock; //英文token器的锁
	static ZTokenizer ms_en_token;     //英文token
	static COldLadConv ms_uy_old2latin;  //维文传统转拉丁
	static CCodeRec    ms_uy_identify;   //维文传统拉丁识别
	static titoken::CSegmentor ms_tibet_segmentor; //藏语分词
	static tibetsentenizer::Sentenizer ms_tibet_sentsplitor; //藏语断句

	static MutexLock ms_uy_ne_lock;
	static NewUyNERec ms_uy_netrans; //维文命名实体识别
	static string uy_netrans_prob; //概率

	static MutexLock ms_tibet_number_lock;
	static TiNumber::NumberPair ms_tibet_number; //藏语数字识别 

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

	//用于中文后处理
	static bool is_ascii(const string & src);

	//用于英文后处理
	static void capitalize(string& str); //首字母大写
	static void capitalize1(string& str); //特定标点符号后的词大写
	static void punc(string& str); //处理标点符号
	static void str_replace(string &str, string _from, string _to);

	//泰语分词
	static thaiSeg::CSegmentor thai_segmentor_;

	// 俄语大小写转换
	static CodeConver ms_russian_lowercase;

	// 日语分词
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
