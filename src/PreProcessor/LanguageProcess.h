//
// Created by 刘舒曼 on 16/9/8.
//

#ifndef TRANSMASTER_LANGUAGEPROCESS_H
#define TRANSMASTER_LANGUAGEPROCESS_H

#include "DictLibrary/DictManager.h"
#include "TemplateLibrary/UsrTemplateManager.h"
#include "LanguageTools/CLanguage.h"
#include "LanguageTools/SentSpliter.h"
#include "TransText/TransText.h"
#include "SipRPC/SipRPC.h"

#include <string>
#include <vector>

using namespace std;

class LanguageProcess
{
public:
	// 对整个文本进行预处理,utf8转换
	static bool StandardPreTextProcess(string & src);
	// 按utf8字符切分文本,按句子切分字符流
	static bool StandardSplitSent(const string & src, vector<string> & sent_vec);
	// 对单个句子进行预处理,过滤多个空格
	// 匹配词典 & 模版
	static bool StandardPreSentProcess(SentProcCell & sent_cell);
};

#endif //TRANSMASTER_LANGUAGEPROCESS_H
