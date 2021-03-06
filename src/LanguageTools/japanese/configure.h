#pragma once

#include <iostream>
using namespace std;
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <algorithm>

//

//string BASIC_FORM;


#define			DEF_RENGO	"連語"

#define         MAX_SENTENCE_LEN        100000
//#define			BASIC_FORM		"基本形"
#define         MIDASI_MAX      129
#define         YOMI_MAX        129
#define         STOP_MRPH_WEIGHT        255
#define         MAX_PATHES_WK   5000
//#define         INT_MAX 0x7ffffff
#define         DEFAULT_C_WEIGHT        10

#define         NILSYMBOL       "NIL"
#define         HAVE_MMAP       1
#define         GRAMMARFILE     "dic.grammar"
#define         CLASSIFY_MAX       128

#define         CONJUGATIONFILE     "dic.conj" //活用
#define         TYPE_NO         128
#define         FORM_NO         128

#define         CONNECTFILE     "dic.connect"
#define         KANKEIFILE      "dic.kankei"

#define         TABLEFILE       "dic.tab"
#define         MATRIXFILE      "dic.mat"

//rc文件的default权重
#define         POS_COST_DEFAULT   10
#define         CONNECT_WEIGHT_DEFAULT  100
#define         WORD_WEIGHT_DEFAULT  1
#define         COST_WIDTH_DEFAULT      20
#define         UNDEF_WORD_DEFAULT      10000
#define         MRPH_DEFAULT_WEIGHT     10

#define         RENGO_ID        "999"
#define         RENGO_DEFAULT_WEIGHT    0.5

#define			SPECIAL_DIC			"SPECIAL_DIC" //特殊词典
#define			DEF_SOURCE_FILE		"SOURCE"	//资源文件集合路径
#define         DEF_POS_C             "POS_COST"    //词类权重
#define         DEF_CONNECT_W          "CONNECTION_PROP"        //链接花费比重
#define         DEF_WORD_W          "WORD_PROP"      //形态素花费比重
#define         DEF_COST_MARGIN           "MARGIN"      //权重幅度
#define         DEF_DIC_FILE            "DICTIONARY"  //词典文件
#define         DEF_GRAMMAR_FILE           "GRAMMAR"  //文法文件
#define         USER_DIC_FILE   "USER_DIC_FILE" //用户词典文件
#define         USER_DIC_WEIGHT        "USER_DIC_WEIGHT"       //用户词典权重


//词典信息
#define         MAX_DIC_NUMBER  10
//#define         O_RDONLY        00
#define         DEFAULT_DIC_WEIGHT      20
/*
//未登录词信息
#define         DEF_UNDEF               "未定義語"      //未登陆词
#define         DEF_UNDEF_KATA          "カタカナ"      //片假名
#define         DEF_UNDEF_ALPH          "アルファベット"        //字母
#define         DEF_UNDEF_ETC           "その他"        //其他

//数词信息
#define         DEF_SUUSI_HINSI         "名詞"
#define         DEF_SUUSI_BUNRUI        "数詞"

//透过处理信息
#define         DEF_KAKKO_HINSI         "特殊"
#define         DEF_KAKKO_BUNRUI1       "括弧始"
#define         DEF_KAKKO_BUNRUI2       "括弧終"
#define         DEF_KUUHAKU_HINSI       "特殊"
#define         DEF_KUUHAKU_BUNRUI      "空白"
*/
#define         CONNECT_MATRIX_MAX      1000
#define         LENMAX  50000
#define         BYTES4CHAR       3

#define		KUUHAKU            	0x20    //空格
#define		HANKAKU            	0x80    //ascii表分解符
#define		PRIOD            	0xa1a5
#define		CHOON            	0xa1bc  //扩折号"—"
#define		KIGOU            	0xa3b0  //记号
#define		SUJI           	        0xa3c0  //数字0-9
#define		ALPH            	0xa4a0  //阿拉伯字母
#define		HIRAGANA                0xa5a0  //平假名
#define		KATAKANA                0xa6a0  //片假名
#define		GR                      0xb0a0   /* α, β, ... */

#define		KANJI                   0xffff      //日语汉字与"々"

