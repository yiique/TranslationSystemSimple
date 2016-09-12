#include "PuncResource.h"

using namespace ssplite;

ChPuncResource ChPuncResource::ms_ch_punc_resource;
EnPuncResource EnPuncResource::ms_en_punc_resource;
UyPuncResource UyPuncResource::ms_uy_punc_resource;


// PuncResource public
bool PuncResource::FindPuncAttr(const string &punc_name, int &attr) const
{
	map<string, int>::const_iterator iter = m_punc_map.find(punc_name);

	if (iter != m_punc_map.end())
	{
		attr = iter->second;
		return true;
	}

	attr = TYPE_WORD;
	return false;
}

string & PuncResource::Conv2StdPunc(string &punc_name) const
{
	map<string, string>::const_iterator iter = m_std_conv_map.find(punc_name);

	if (iter != m_std_conv_map.end())
		punc_name = iter->second;

	return punc_name;
}

bool PuncResource::IsLeftRightMatch(const string &left_name, const string &right_name) const
{
	set<pair<string, string>>::const_iterator iter = m_left_right_set.find(make_pair(left_name, right_name));

	if (iter != m_left_right_set.end())
		return true;
	else
		return false;
}

bool PuncResource::IsRightSpeakQuot(const string &punc_name) const
{
	set<string>::const_iterator iter = m_right_speak_quot_set.find(punc_name);

	if (iter != m_right_speak_quot_set.end())
		return true;
	else
		return false;
}

// PuncResource private
PuncResource::PuncResource(void)
{
}


// ChPuncResource
const ChPuncResource::GetInstrance()
{
	return ms_ch_punc_resource;
}

string & ChPuncResource::Conv2StdPunc(string &punc_name) const
{
	return PuncResource::Conv2StdPunc(punc_name);
}

void ChPuncResource::init_punc_resource()
{
#ifdef CH_SEG_WITH_TAG
	static const char CH_UTF8_PUNC_0[] = {0xe3, 0x80, 0x82, 0x2f, 0x77, 0}; // PUN = 。/w
#else
	static const char CH_UTF8_PUNC_0[] = {0xe3, 0x80, 0x82, 0x00}; // PUN = 。
#endif
	static const char CH_UTF8_PUNC_1[] = {0xe3, 0x80, 0x82, 0x00 }; // PUN = 。
	static const char CH_UTF8_PUNC_2[] = {0x2c, 0x00 }; // PUN = ,
	static const char CH_UTF8_PUNC_3[] = {0xef, 0xbc, 0x8c, 0x00 }; // PUN = ，
	static const char CH_UTF8_PUNC_4[] = {0xe3, 0x80, 0x81, 0x00 }; // PUN = 、
	static const char CH_UTF8_PUNC_5[] = {0xef, 0xbc, 0x9b, 0x00 }; // PUN = ；
	static const char CH_UTF8_PUNC_6[] = {0x3b, 0x00 }; // PUN = ;
	static const char CH_UTF8_PUNC_7[] = {0xef, 0xbc, 0x9a, 0x00 }; // PUN = ：
	static const char CH_UTF8_PUNC_8[] = {0xef, 0xbc, 0x9f, 0x00 }; // PUN = ？
	static const char CH_UTF8_PUNC_9[] = {0xef, 0xbc, 0x81, 0x00 }; // PUN = ！
	static const char CH_UTF8_PUNC_10[] = {0x3f, 0x00 }; // PUN = ?
	static const char CH_UTF8_PUNC_11[] = {0x21, 0x00 }; // PUN = !
	static const char CH_UTF8_PUNC_12[] = {0xe2, 0x80, 0xa6, 0x00 }; // PUN = …
	static const char CH_UTF8_PUNC_13[] = {0xe2, 0x80, 0x94, 0x00 }; // PUN = —
	static const char CH_UTF8_PUNC_14[] = {0xe2, 0x80, 0x94, 0xe2, 0x80, 0x94, 0x00 }; // PUN = ——
	static const char CH_UTF8_PUNC_15[] = {0x2e, 0x00 }; // PUN = .
	static const char CH_UTF8_PUNC_16[] = {0x2e, 0x2e, 0x2e, 0x00 }; // PUN = ...
	static const char CH_UTF8_PUNC_17[] = {0x27, 0x00 }; // PUN = '
	static const char CH_UTF8_PUNC_18[] = {0x22, 0x00 }; // PUN = "
	static const char CH_UTF8_PUNC_19[] = {0xe2, 0x80, 0xa6, 0xe2, 0x80, 0xa6, 0x00 }; // PUN = ……
	static const char CH_UTF8_PUNC_20[] = {0xe2, 0x80, 0x9c, 0x00 }; // PUN = “
	static const char CH_UTF8_PUNC_21[] = {0xe2, 0x80, 0x9d, 0x00 }; // PUN = ”
	static const char CH_UTF8_PUNC_22[] = {0xe2, 0x80, 0x98, 0x00 }; // PUN = ‘
	static const char CH_UTF8_PUNC_23[] = {0xe2, 0x80, 0x99, 0x00 }; // PUN = ’
	static const char CH_UTF8_PUNC_24[] = {0xe2, 0x95, 0x9a, 0x00 }; // PUN = ╚
	static const char CH_UTF8_PUNC_25[] = {0xe2, 0x95, 0x97, 0x00 }; // PUN = ╗
	static const char CH_UTF8_PUNC_26[] = {0xe2, 0x94, 0x94, 0x00 }; // PUN = └
	static const char CH_UTF8_PUNC_27[] = {0xe2, 0x94, 0x90, 0x00 }; // PUN = ┐
	static const char CH_UTF8_PUNC_28[] = {0xef, 0xbc, 0x88, 0x00 }; // PUN = （
	static const char CH_UTF8_PUNC_29[] = {0xef, 0xbc, 0x89, 0x00 }; // PUN = ）
	static const char CH_UTF8_PUNC_30[] = {0x28, 0x00 }; // PUN = (
	static const char CH_UTF8_PUNC_31[] = {0x29, 0x00 }; // PUN = )
	static const char CH_UTF8_PUNC_32[] = {0xe3, 0x80, 0x8a, 0x00 }; // PUN = 《
	static const char CH_UTF8_PUNC_33[] = {0xe3, 0x80, 0x8b, 0x00 }; // PUN = 》
	static const char CH_UTF8_PUNC_34[] = {0x3c, 0x20, 0x00 }; // PUN = <
	static const char CH_UTF8_PUNC_35[] = {0x3e, 0x20, 0x00 }; // PUN = >
	static const char CH_UTF8_PUNC_36[] = {0x5b, 0x00 }; // PUN = [
	static const char CH_UTF8_PUNC_37[] = {0x5d, 0x00 }; // PUN = ]
	static const char CH_UTF8_PUNC_38[] = {0x7b, 0x00 }; // PUN = {
	static const char CH_UTF8_PUNC_39[] = {0x7d, 0x00 }; // PUN = }
	static const char CH_UTF8_PUNC_40[] = {0x28, 0x00 }; // PUN = (
	static const char CH_UTF8_PUNC_41[] = {0x29, 0x00 }; // PUN = )
	static const char CH_UTF8_PUNC_42[] = {0xe3, 0x80, 0x8e, 0x00 }; // PUN = 『
	static const char CH_UTF8_PUNC_43[] = {0xe3, 0x80, 0x8f, 0x00 }; // PUN = 』

	//可加在句子后面的结束标点符号
	m_sent_end_punc = CH_UTF8_PUNC_0; //"。/w";
	m_sent_end_punc_name = CH_UTF8_PUNC_1; //"。";

	//标点类型表
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_1, TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_2,  TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_3,  TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_4,  TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_5,  TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_6,  TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_7,  TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_8, TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_9, TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_10, TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_11, TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_12,  TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_13,   TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_14,  TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_15,  TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_16, TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_17,  TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_18, TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_19,  TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_20, TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_21, TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_22,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_23,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_24,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_25,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_26,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_27,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_28,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_29,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_30,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_31,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_32,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_33,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_34,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_35,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_36,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_37,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_38,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_39,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_40,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_41,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_42,TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(CH_UTF8_PUNC_43,TYPE_PUNC_QUOT));
	//标准转换表

	//左右配对表
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_20 , CH_UTF8_PUNC_21));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_18 , CH_UTF8_PUNC_18));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_17 , CH_UTF8_PUNC_17));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_22 , CH_UTF8_PUNC_23));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_24 , CH_UTF8_PUNC_25));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_26 , CH_UTF8_PUNC_27));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_32 , CH_UTF8_PUNC_33));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_34 , CH_UTF8_PUNC_35));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_36 , CH_UTF8_PUNC_37));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_38 , CH_UTF8_PUNC_39));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_40 , CH_UTF8_PUNC_41));
	m_left_right_set.insert(make_pair(CH_UTF8_PUNC_42 , CH_UTF8_PUNC_43));

	//可用于对话的右引号表，用来处理： 我说：“你好！”这种断句的情况
	m_right_speak_quot_set.insert(CH_UTF8_PUNC_21);
	m_right_speak_quot_set.insert(CH_UTF8_PUNC_18);
	m_right_speak_quot_set.insert(CH_UTF8_PUNC_43);
}

// EnPuncResource
const EnPuncResource & EnPuncResource::GetInstrance()
{
	return ms_en_punc_resource;
}

void EnPuncResource::init_punc_resource()
{
	//可加在句子后面的结束标点符号
	m_sent_end_punc = ".";
	m_sent_end_punc_name = ".";

	//标点类型表
	m_punc_map.insert(make_pair(",", TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair(":", TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair("--", TYPE_PUNC_NORMAL));
	m_punc_map.insert(make_pair(".", TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair("?", TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair("!", TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair(";", TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair("...", TYPE_PUNC_TERMINAL));
	m_punc_map.insert(make_pair("'", TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair("\"", TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair("< ",TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair("> ",TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair("[",TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair("]",TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair("{",TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair("}",TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair("(",TYPE_PUNC_QUOT));
	m_punc_map.insert(make_pair(")",TYPE_PUNC_QUOT));

	//标准转换表
	//左右配对表
	m_left_right_set.insert(make_pair("<" , ">"));
	m_left_right_set.insert(make_pair("[" , "]"));
	m_left_right_set.insert(make_pair("{" , "}"));
	m_left_right_set.insert(make_pair("(" , ")"));
	m_left_right_set.insert(make_pair("\"", "\""));
	m_left_right_set.insert(make_pair("\'", "\'"));

	//可用于对话的右引号表，用来处理： 我说：“你好！”这种断句的情况
	m_right_speak_quot_set.insert("\"");
}
