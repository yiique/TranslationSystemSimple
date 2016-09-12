
#include "CLanguage.h"
#include "SentSpliter.h"

#include "Common/ICUConverter.h"
#include "LanguageTools/chinese/ChSegmentManager.h"
#include "LanguageTools/NewTools.h"

#include "boost/asio.hpp"

#define UY_OLD2LATIN_RESOURCE_PATH "mdata/uyghur/uyconv/convert_resource_std.txt" 
#define UY_IDENTIFY_RESOURCE_PATH  "mdata/uyghur/uyidentify/"
#define EN_TOKEN_RESOURCE_RE_PATH "mdata/english/token/no_abbr_no_BreakLine_RE.txt"
#define EN_TOKEN_RESOURCE_ABBR_PATH "mdata/english/token/BrevWords.txt"
#define TIBET_SEGMENT_RESOURCE_PATH "mdata/tibet/segment/"
#define TIBET_SENT_SPLITOR_RESOURCE_PATH "mdata/tibet/splitsent/test_conf.ini" 
#define UY_NETRANS_RESOURCE_PATH "mdata/uyghur/uynetrans/"
#define TIBET_NUMBER_RESOURCE_PATH "mdata/tibet/tibetnumber/"
#define MONGOLIAN_CONV_RESOURCE_PATH "mdata/mongolian/mo2latin/menki_utf82latin.txt"
#define MONGOLIAN_NUMDATE_RESOURCE_PATH "mdata/mongolian/monumdate/"
#define NEW_MONGOLIAN_LOWER_RESOURCE_PATH "mdata/mongolian/newlowercase/cy-tolower.txt"
#define VIETNAM_LOWER_RESOURCE_PATH "mdata/vietnam/vietnam_lower.txt"
#define MONGOLIAN_TOKEN_PATH "mdata/mongolian/latin-mongolian-tokenizer.cfg"
#define THAI_SEGMENTOR_RESOURCE_PATH "mdata/thai/"
#define RUSSIAN_LOWER_CASE_TABLE_PATH "mdata/russian/lowercase.txt"
#define JAPANESE_SEG_PATH "mdata/japanese/setting.ini"

MutexLock CLanguage::ms_en_token_lock; 
ZTokenizer CLanguage::ms_en_token;     
COldLadConv CLanguage::ms_uy_old2latin;
CCodeRec    CLanguage::ms_uy_identify; 
titoken::CSegmentor CLanguage::ms_tibet_segmentor;
tibetsentenizer::Sentenizer CLanguage::ms_tibet_sentsplitor;
thaiSeg::CSegmentor CLanguage::thai_segmentor_;

MutexLock CLanguage::ms_mongolian_token_lock;
RulesTable CLanguage::ms_mongolian_token(MONGOLIAN_TOKEN_PATH);

MutexLock CLanguage::ms_uy_ne_lock;
NewUyNERec CLanguage::ms_uy_netrans("mdata/uyghur/uynetrans/Number_Config.txt", "mdata/uyghur/uynetrans/Time_Config.txt", "mdata/uyghur/uynetrans/UyNeNum.cfg");
string CLanguage::uy_netrans_prob;

MutexLock CLanguage::ms_tibet_number_lock;
TiNumber::NumberPair CLanguage::ms_tibet_number;

MutexLock CLanguage::ms_mo2latin_lock;
CodeConverNameSpace::Mo2Latin CLanguage::ms_mo2latin;

MutexLock CLanguage::ms_monumdate_lock;
MoNumDate::CMoNumDate CLanguage::ms_monumdate;

MutexLock CLanguage::ms_newmo_lowercase_lock;
Cy2Lower::CCy2Lower CLanguage::ms_newmo_lowercase;

CodeConver CLanguage::ms_russian_lowercase;

JPTOKEN::token CLanguage::ms_japanese_seg;


VietnamLower CLanguage::ms_vietnam_lower(VIETNAM_LOWER_RESOURCE_PATH);

//KMA_DLL CLanguage::m_kma;

bool CLanguage::InitStaticResource(const Configurator & config)
{
	uy_netrans_prob = config.uy_netrans_prob;

	if(false == thai_segmentor_.Initialize(THAI_SEGMENTOR_RESOURCE_PATH))
	{
		lerr << "Init thai segmentor failed." << endl;
		return false;
	}
	lout << "Init thai segmentor success." << endl;

	if(false == ms_newmo_lowercase.Init(NEW_MONGOLIAN_LOWER_RESOURCE_PATH))
	{
		lerr << "Init new mongolian lowercase failed." << endl;
		return false;
	}
	
	lout << "Init new mongolian lowercase success." << endl;

	if(false == ms_mo2latin.Init(MONGOLIAN_CONV_RESOURCE_PATH))
	{
		lerr << "Init mo2latin failed." << endl;
		return false;
	}
	
	lout << "Init mo2latin success." << endl;

	if(false == ms_monumdate.Init(MONGOLIAN_NUMDATE_RESOURCE_PATH))
	{
		lerr << "Init monumdate failed." << endl;
		return false;
	}

	lout << "Init monumdate success." << endl;


	lout << "Init Uy netrans success." << endl;

	if(false == ms_tibet_number.Initialize(TIBET_NUMBER_RESOURCE_PATH))
	{
		lerr << "Init Tibet number failed." << endl;
		return false;
	}

	lout << "Init Tibet number success." << endl;

	if(false == ChSegmentManager::GetInstance()->Initialize(config._text_result_path))
	{
		lerr << "Init Ch segmentor PBClas failed." << endl;
		return false;
	}

	lout << "Init Ch segmentor PBClas success." << endl;

	if(false == ms_en_token.Initialize(EN_TOKEN_RESOURCE_RE_PATH, EN_TOKEN_RESOURCE_ABBR_PATH))
	{
		lerr << "Init en tokenizer failed." << endl;
		return false;
	}

	lout << "Init en tokenizer success." << endl;

	if(false == ms_uy_old2latin.LoadMapTab(UY_OLD2LATIN_RESOURCE_PATH))
	{
		lerr << "Init uy old 2 latin failed." << endl;
		return false;
	}

	lout << "Init uy old 2 latin success." << endl;

	if(false == ms_uy_identify.Load(UY_IDENTIFY_RESOURCE_PATH) )
	{
		lerr << "Init uy identify failed. " << endl;
		return false;
	}

	lout << "Init uy identify success." << endl;

	if(false == ms_tibet_segmentor.Initialize(TIBET_SEGMENT_RESOURCE_PATH))
	{
		lerr << "Init tibet segment failed." << endl;
		return false;
	}

	lout << "Init tibet segment success." << endl;

	if(false == ms_tibet_sentsplitor.init(TIBET_SENT_SPLITOR_RESOURCE_PATH))
	{
		lerr << "Init tibet sent splitor failed." << endl;
		return false;
	}

	lout << "Init tibet sent splitor success." << endl;
	if (false == ms_russian_lowercase.InitMap(RUSSIAN_LOWER_CASE_TABLE_PATH))
	{
		lerr << "Init Russian lower cased failed" << endl;
		return false;
	}
	lout << "Init Russian lower case success." << endl;

	lout << "Init Japanese segmentor." << endl;
	if (false == ms_japanese_seg.init(JAPANESE_SEG_PATH))
	{
		lerr << "Init Japanese segmentor failed" << endl;
		return false;
	}
	lout << "Init Japanese segmentor sucess" << endl;
	

	/*if (false == m_kma.create("mdata/korean/kma.ini"))
	{
		lerr << "Init korean segmentor failed" << endl;
		return false;
	}
	lout << "Init korean segmentor sucess" << endl;*/

	return true;
}


string & CLanguage::en_token_post(string & str) 
{
	//词条化（201605）
	find_and_replace(str, "'m", " am");
	find_and_replace(str, "'re", " are");
	find_and_replace(str, "'t", " not");
	find_and_replace(str, "cant", "can not");
	find_and_replace(str, "dont", "do not");


	//将“co .”合并为“co.”
	find_and_replace(str, " co . ", " co. ");

	//去掉最后的\r或者\n

	if(str.size() > 0)
	{
		if('\r' == *(str.rbegin()) || '\n' == *(str.rbegin()) )
			str.erase(str.size()-1, 1);
	}

	return str;
}

void CLanguage::UyNetransLatin(const string & src, vector<string> & result_vec)
{
	MutexLockGuard guard(ms_uy_ne_lock);
	//vector<string> ne_vec = ms_uy_netrans.process(src);

	//lout << "UyNetrans result size : " << ne_vec.size() << endl;

	//for(size_t i=0; i<ne_vec.size(); ++i)
	//{
	//	lout << "UyNe : " << ne_vec[i] << endl;
	//	result_vec.push_back(ne_vec[i]);
	//}

	//new netrans
	string oriLine = src;
	while(oriLine.find("  ") != string::npos)
		Replace_all(oriLine, "  ", " ");
	//lout<<"ms_uy_netrans.NeRec start"<<endl;
	ms_uy_netrans.NeRec(oriLine);
	//lout<<"ms_uy_netrans.NeRec done"<<endl;
	vector<string> ne_vec = ms_uy_netrans.ne_trans_table(oriLine, uy_netrans_prob);
	//lout<<"ne_trans_table done"<<endl;
	//lout << "UyNetrans result size : " << ne_vec.size() << endl;

	for(size_t i=0; i<ne_vec.size(); ++i)
	{
		//lout << "UyNe : " << ne_vec[i] << endl;
		result_vec.push_back(ne_vec[i]);
	}
}

void CLanguage::TibetNumberTrans(const string & src, vector<string> & result_vec)
{
	MutexLockGuard guard(ms_tibet_number_lock);
	vector<string> ne_vec;
	string _s = src;
	ms_tibet_number.GetTransPair(_s, ne_vec);

	//lout << "TibetNumber result size : " << ne_vec.size() << endl;

	for(size_t i=0; i<ne_vec.size(); ++i)
	{
		//lout << "TibetNumber : " << ne_vec[i] << endl;
		result_vec.push_back(ne_vec[i]);
	}
}

string CLanguage::IdentifyChEn(const string & src)
{
	Utf8Stream utf8_ss(src);

	size_t en_cnt = 0;
	size_t ch_cnt = 0;

	string utf8_char;
	while(utf8_ss.ReadOneCharacter(utf8_char))
	{
		if(utf8_char.size() <= 1)
			en_cnt++;
		else
			ch_cnt++;
	}

	if(en_cnt >= ch_cnt)
		return LANGUAGE_ENGLISH;
	else
		return LANGUAGE_CHINESE;
}




void CLanguage::RemoveChPosTag(string & src)
{
	vector<string> src_vec;

	split_string_by_blank(src.c_str(), src_vec);

	string tgt;

	for(size_t i=0; i<src_vec.size(); ++i)
	{

		size_t pos = src_vec[i].find("/");

		if(string::npos != pos)
		{
			tgt += src_vec[i].substr(0, pos);
		}else
		{
			tgt += src_vec[i];
		}

		if(i+1 < src_vec.size())
			tgt += " ";
	}

	src = tgt;
}

void CLanguage::ChSegment(const string & src, string & tgt)
{
	ChSegmentManager::GetInstance()->Segment(src, tgt);
}

void CLanguage::ChSegment(const string & src, string & tgt, const set<string> & force_dict)
{
	ChSegmentManager::GetInstance()->Segment(src, tgt, force_dict);
}

void CLanguage::EnToken(const string & src, string & tgt)
{
	MutexLockGuard guard(ms_en_token_lock);

	ms_en_token.Token(src, tgt);

	en_token_post(tgt); //英文token后处理
}

bool CLanguage::IsUyghurLatin(const string & src)
{
	if(ms_uy_identify.RecCodeStr2(src) == "latin")
		return true;
	else 
		return false;
}

void CLanguage::UyToken(const string & src, string & tgt)
{
	UigurTokenizer token;
	token.TokenLatin(src, tgt);
}

void CLanguage::UyghurOld2Latin(string & src)
{
	string latin = ms_uy_old2latin.Old2Lad(src);

	src = latin;
}

string & CLanguage::utf8_char_a2b(string & utf8_char)
{
	static const char UTF8_SBC_SET[][4] = { {0xe3, 0x80, 0x80, 0x00 },  {0xef, 0xbc, 0x81, 0x00 },  {0xef, 0xbc, 0x82, 0x00 },  {0xef, 0xbc, 0x83, 0x00 },  
											  {0xef, 0xbc, 0x84, 0x00 },  {0xef, 0xbc, 0x85, 0x00 },  {0xef, 0xbc, 0x86, 0x00 },  {0xef, 0xbc, 0x87, 0x00 },  
											  {0xef, 0xbc, 0x88, 0x00 },  {0xef, 0xbc, 0x89, 0x00 },  {0xef, 0xbc, 0x8a, 0x00 },  {0xef, 0xbc, 0x8b, 0x00 },  
											  {0x00, 0x00, 0x00, 0x00 },  {0xef, 0xbc, 0x8d, 0x00 },  {0xef, 0xbc, 0x8e, 0x00 },  {0xef, 0xbc, 0x8f, 0x00 },  
											  {0xef, 0xbc, 0x90, 0x00 },  {0xef, 0xbc, 0x91, 0x00 },  {0xef, 0xbc, 0x92, 0x00 },  {0xef, 0xbc, 0x93, 0x00 },  
											  {0xef, 0xbc, 0x94, 0x00 },  {0xef, 0xbc, 0x95, 0x00 },  {0xef, 0xbc, 0x96, 0x00 },  {0xef, 0xbc, 0x97, 0x00 },  
											  {0xef, 0xbc, 0x98, 0x00 },  {0xef, 0xbc, 0x99, 0x00 },  {0xef, 0xbc, 0x9a, 0x00 },  {0xef, 0xbc, 0x9b, 0x00 },  
											  {0xef, 0xbc, 0x9c, 0x00 },  {0xef, 0xbc, 0x9d, 0x00 },  {0xef, 0xbc, 0x9e, 0x00 },  {0xef, 0xbc, 0x9f, 0x00 },  
											  {0xef, 0xbc, 0xa0, 0x00 },  {0xef, 0xbc, 0xa1, 0x00 },  {0xef, 0xbc, 0xa2, 0x00 },  {0xef, 0xbc, 0xa3, 0x00 },  
											  {0xef, 0xbc, 0xa4, 0x00 },  {0xef, 0xbc, 0xa5, 0x00 },  {0xef, 0xbc, 0xa6, 0x00 },  {0xef, 0xbc, 0xa7, 0x00 },  
											  {0xef, 0xbc, 0xa8, 0x00 },  {0xef, 0xbc, 0xa9, 0x00 },  {0xef, 0xbc, 0xaa, 0x00 },  {0xef, 0xbc, 0xab, 0x00 },  
											  {0xef, 0xbc, 0xac, 0x00 },  {0xef, 0xbc, 0xad, 0x00 },  {0xef, 0xbc, 0xae, 0x00 },  {0xef, 0xbc, 0xaf, 0x00 },  
											  {0xef, 0xbc, 0xb0, 0x00 },  {0xef, 0xbc, 0xb1, 0x00 },  {0xef, 0xbc, 0xb2, 0x00 },  {0xef, 0xbc, 0xb3, 0x00 },  
											  {0xef, 0xbc, 0xb4, 0x00 },  {0xef, 0xbc, 0xb5, 0x00 },  {0xef, 0xbc, 0xb6, 0x00 },  {0xef, 0xbc, 0xb7, 0x00 },  
											  {0xef, 0xbc, 0xb8, 0x00 },  {0xef, 0xbc, 0xb9, 0x00 },  {0xef, 0xbc, 0xba, 0x00 },  {0xef, 0xbc, 0xbb, 0x00 },  
											  {0xef, 0xbc, 0xbc, 0x00 },  {0xef, 0xbc, 0xbd, 0x00 },  {0xef, 0xbc, 0xbe, 0x00 },  {0xef, 0xbc, 0xbf, 0x00 },  
											  {0xef, 0xbd, 0x80, 0x00 },  {0xef, 0xbd, 0x81, 0x00 },  {0xef, 0xbd, 0x82, 0x00 },  {0xef, 0xbd, 0x83, 0x00 },  
											  {0xef, 0xbd, 0x84, 0x00 },  {0xef, 0xbd, 0x85, 0x00 },  {0xef, 0xbd, 0x86, 0x00 },  {0xef, 0xbd, 0x87, 0x00 },  
											  {0xef, 0xbd, 0x88, 0x00 },  {0xef, 0xbd, 0x89, 0x00 },  {0xef, 0xbd, 0x8a, 0x00 },  {0xef, 0xbd, 0x8b, 0x00 },  
											  {0xef, 0xbd, 0x8c, 0x00 },  {0xef, 0xbd, 0x8d, 0x00 },  {0xef, 0xbd, 0x8e, 0x00 },  {0xef, 0xbd, 0x8f, 0x00 },  
											  {0xef, 0xbd, 0x90, 0x00 },  {0xef, 0xbd, 0x91, 0x00 },  {0xef, 0xbd, 0x92, 0x00 },  {0xef, 0xbd, 0x93, 0x00 },  
											  {0xef, 0xbd, 0x94, 0x00 },  {0xef, 0xbd, 0x95, 0x00 },  {0xef, 0xbd, 0x96, 0x00 },  {0xef, 0xbd, 0x97, 0x00 },  
											  {0xef, 0xbd, 0x98, 0x00 },  {0xef, 0xbd, 0x99, 0x00 },  {0xef, 0xbd, 0x9a, 0x00 },  {0xef, 0xbd, 0x9b, 0x00 },  
											  {0xef, 0xbd, 0x9c, 0x00 },  {0xef, 0xbd, 0x9d, 0x00 },  {0xef, 0xbd, 0x9e, 0x00 },  0};
	
	 static const char UTF8_DBC_SET[][2] = {  {0x20, 0x00 },  {0x21, 0x00 },  {0x22, 0x00 },  {0x23, 0x00 },  
											{0x24, 0x00 },  {0x25, 0x00 },  {0x26, 0x00 },  {0x27, 0x00 },  
											{0x28, 0x00 },  {0x29, 0x00 },  {0x2a, 0x00 },  {0x2b, 0x00 },  
											{0x00, 0x00 },  {0x00, 0x00 },  {0x2e, 0x00 },  {0x2f, 0x00 },  
											{0x30, 0x00 },  {0x31, 0x00 },  {0x32, 0x00 },  {0x33, 0x00 },  
											{0x34, 0x00 },  {0x35, 0x00 },  {0x36, 0x00 },  {0x37, 0x00 },  
											{0x38, 0x00 },  {0x39, 0x00 },  {0x3a, 0x00 },  {0x3b, 0x00 },  
											{0x3c, 0x00 },  {0x3d, 0x00 },  {0x3e, 0x00 },  {0x3f, 0x00 },  
											{0x40, 0x00 },  {0x41, 0x00 },  {0x42, 0x00 },  {0x43, 0x00 },  
											{0x44, 0x00 },  {0x45, 0x00 },  {0x46, 0x00 },  {0x47, 0x00 },  
											{0x48, 0x00 },  {0x49, 0x00 },  {0x4a, 0x00 },  {0x4b, 0x00 },  
											{0x4c, 0x00 },  {0x4d, 0x00 },  {0x4e, 0x00 },  {0x4f, 0x00 },  
											{0x50, 0x00 },  {0x51, 0x00 },  {0x52, 0x00 },  {0x53, 0x00 },  
											{0x54, 0x00 },  {0x55, 0x00 },  {0x56, 0x00 },  {0x57, 0x00 },  
											{0x58, 0x00 },  {0x59, 0x00 },  {0x5a, 0x00 },  {0x5b, 0x00 },  
											{0x5c, 0x00 },  {0x5d, 0x00 },  {0x5e, 0x00 },  {0x5f, 0x00 },  
											{0x60, 0x00 },  {0x61, 0x00 },  {0x62, 0x00 },  {0x63, 0x00 },  
											{0x64, 0x00 },  {0x65, 0x00 },  {0x66, 0x00 },  {0x67, 0x00 },  
											{0x68, 0x00 },  {0x69, 0x00 },  {0x6a, 0x00 },  {0x6b, 0x00 },  
											{0x6c, 0x00 },  {0x6d, 0x00 },  {0x6e, 0x00 },  {0x6f, 0x00 },  
											{0x70, 0x00 },  {0x71, 0x00 },  {0x72, 0x00 },  {0x73, 0x00 },  
											{0x74, 0x00 },  {0x75, 0x00 },  {0x76, 0x00 },  {0x77, 0x00 },  
											{0x78, 0x00 },  {0x79, 0x00 },  {0x7a, 0x00 },  {0x7b, 0x00 },  
											{0x7c, 0x00 },  {0x7d, 0x00 },  {0x7e, 0x00 },  0};

	size_t i=0;
	const char * p_sbc = UTF8_SBC_SET[i];
	while(i<95)
	{
		if(utf8_char == p_sbc)
		{
			utf8_char = UTF8_DBC_SET[i];
			return utf8_char;
		}

		++i;
		p_sbc = UTF8_SBC_SET[i];
	}

	return utf8_char;
}

void CLanguage::Utf8A2B(string & src)
{

	Utf8Stream utf8_ss(src);

	string tgt;
	string utf8_char;
	while(utf8_ss.ReadOneCharacter(utf8_char))
	{
		tgt += utf8_char_a2b(utf8_char);
	}

	src = tgt;
}

string & CLanguage::SpliteStrByChar(string & str)
{
	Utf8Stream utf8_ss(str);
	str.clear();

	string utf8_char;
	while(utf8_ss.ReadOneCharacter(utf8_char))
	{
		str += utf8_char + " ";
	}

	return str;
}

void CLanguage::TibetSegment(const string & src, string & tgt)
{
	ms_tibet_segmentor.token(src, tgt);
}

void CLanguage::TibetSplitSent(const string & src, vector<string> & sent_vec)
{
	ms_tibet_sentsplitor.sentenize(src, sent_vec);
}


string & CLanguage::En2ChPost(string & str)
{
	if(str.size() == 0) return str;

	string tgt = "";

	
	vector<string> words_vec;

	split_string_by_tag(str.c_str(), words_vec, ' ');
	
	bool is_last_ascii = false;
	
	
	for(size_t i=0; i<words_vec.size(); ++i)
	{
		string & word = words_vec[i];
		if(true == is_ascii(word))
		{
			if(false == is_last_ascii)
				tgt += " ";

			is_last_ascii = true;
			tgt += word + " ";
		}else
		{
			if(word == ",," || word == "，，")
			{
				word = ",";
			}

			tgt += word;
			is_last_ascii = false;
		
		}
	}

	//尾部不加空格
	/*if(!is_last_ascii)
		tgt += " ";*/

    str = tgt;

	//lout << "PostChi End : [" << str << "]" << endl;

	return str;
}

bool CLanguage::is_ascii(const string & src)
{
	for(size_t i=0; i<src.size(); ++i)
	{
		if(0 > (int) src[i])
			return false;
	}

	return true;
}

string & CLanguage::Ch2EnPost(string & str)
{
	if(str.size() == 0) return str;

	// 特定标点符号后的词大写
	capitalize1(str);

	// 处理标点符号
	punc(str);

	// 首字母大写
	capitalize(str);
	
	return str;
}

void CLanguage::capitalize1(string& str)
{
	istrstream buf(str.c_str());
	vector<string> wordVec;
	string w;

	while (buf >> w)
	{
		wordVec.push_back(w);
	}

	int i;

	for (i = 1; i < wordVec.size(); i++)
	{
		if (wordVec[i - 1] == "." ||
			wordVec[i - 1] == "?" ||
			wordVec[i - 1] == "!")
		{
			capitalize(wordVec[i]);
		}

		if (i >= 2 && wordVec[i - 1] == "\"" &&
			(wordVec[i - 2] == ":" || wordVec[i - 2] == ","))
		{
			capitalize(wordVec[i]);
		}
	}

	string str1;

	for (i = 0; i < wordVec.size(); i++)
	{
		str1 += wordVec[i];

		if (i != wordVec.size() - 1)
		{
			str1 += " ";
		}
	}

	str = str1;
}

void CLanguage::punc(string& str)
{
	//str_replace(str, " 's", "'s");	//modified by xiejun 2010-07-12
	//str_replace(str, "s '", "s'");	//modified by xiejun 2010-07-12

	str_replace(str," '", "'");			//modified by xiejun 2010-07-12
	str_replace(str, " n't", "n't");
	str_replace(str, ", ,", ",");
	str_replace(str, ", .", ".");
	str_replace(str, ". ,", ",");
	str_replace(str, " - ", "-");
	str_replace(str, "cann't", "can't");		//modified by xiejun 2010-07-12
	str_replace(str, "wonn't", "won't");		//modified by xiejun 2010-07-12
	str_replace(str, "``","\"");				//modified by xiejun 2010-07-22
	str_replace(str, "`","'");					//modified by xiejun 2010-07-22
	
	str_replace(str, "」", "\"");
	str_replace(str, "「", "\"");
    str_replace(str, " 'd", "'d");  //add by fuchunlin 2011-04-18
	
}

void CLanguage::str_replace(string &str, string _from, string _to)
{
	int pos = 0;
	int off = 0;
	int len1 = _from.length();
	int len2 = _to.length();
	while( (pos = str.find(_from,off)) != -1)
	{
		str.replace(pos,len1,_to);
		off = pos + len2;
	}
}

void CLanguage::capitalize(string& str)
{
	if(str.size() == 0)
		return;

	if (islower((int)str[0]))
	{
		str[0] = toupper((int)str[0]);
	}
}


string & CLanguage::EnLowercase(string & str)
{
	for(size_t i=0; i<str.size(); ++i)
    {
        char & ch = str[i];
        if(ch >= 65 && ch <= 90)
        {
            ch += 32;
        }
    }

	return str;
}

void CLanguage::Utf8A2B(string & src, vector<BaseAlign> & char_align_vec)
{
	char_align_vec.clear();

	Utf8Stream utf8_ss(src);

	string tgt;
	string utf8_char;
	size_t src_offset = 0;
	while(utf8_ss.ReadOneCharacter(utf8_char))
	{
		BaseAlign align;
		align.src_offset = src_offset;
		align.src_size = utf8_char.size();

		src_offset += utf8_char.size();

		utf8_char_a2b(utf8_char);

		align.tgt_offset = tgt.size();
		align.tgt_size = utf8_char.size();

		tgt += utf8_char;

		char_align_vec.push_back(align);
	}

	src = tgt;
}


string & CLanguage::Mongolian2Latin(string & src)
{
	MutexLockGuard guard(ms_mo2latin_lock);

	string tgt = ms_mo2latin.Conver(src);

	src = tgt;

	return src;
}

string & CLanguage::NEWMongolianLower(string & src)
{
	MutexLockGuard guard(ms_newmo_lowercase_lock);

	string tgt = ms_newmo_lowercase.Conver(src);

	src = tgt;

	return src;

}

void CLanguage::MongolianNumDate(const string & src, vector<string> & result_vec)
{
	MutexLockGuard guard(ms_monumdate_lock);

	result_vec = ms_monumdate.GetNERules(src);
}

string & CLanguage::VietnamLowercase(string & src)
{

	string tgt = ms_vietnam_lower.Tolower(src);

	return (src = tgt);
}

string & CLanguage::ThaiSegment(string & src)
{
	/*size_t reply_length = 0;

	const size_t replay_max_length = 10240;
	char reply[replay_max_length];
	try
	{

		boost::asio::io_service io_service;

		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), Configurator::_s_thai_seg_ip, Configurator::_s_thai_seg_port);
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

		boost::asio::ip::tcp::socket s(io_service);
		boost::asio::connect(s, iterator);

   
		boost::asio::write(s, boost::asio::buffer(src.c_str(), src.size()));

		
		reply_length = s.read_some(boost::asio::buffer(reply, replay_max_length));
		
		
  }
  catch (std::exception& e)
  {
		lerr << "Exception: " << e.what() << "\n";
  }

  string tgt(reply, reply_length);

  src = tgt;*/

	string seg;
	thai_segmentor_.Segment(src, seg);
	
	src = seg;
	return src;
}


string CLanguage::MongolianToken(string & str)
{
	MutexLockGuard guard(ms_mongolian_token_lock);
	return ms_mongolian_token.Translate(str);
}

string CLanguage::RussianLowercase(const string& str)
{
	string ret = ms_russian_lowercase.ConverMap(str);
	return ret;
}

string CLanguage::JapaneseSeg(string& str)
{
	string seg;
	lerr << "JP seg input:" << str << endl;
	ms_japanese_seg.segment(str, seg);
	lerr << "JP seg output:" << seg << endl;
	str = seg;
	return str;
}

string CLanguage::KoreanSeg(string& str)
{
	//return m_kma.proc_sen(str.c_str());
	return "";
}