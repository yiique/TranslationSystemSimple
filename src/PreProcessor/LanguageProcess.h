//
// Created by ������ on 16/9/8.
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
	// �������ı�����Ԥ����,utf8ת��
	static bool StandardPreTextProcess(string & src);
	// ��utf8�ַ��з��ı�,�������з��ַ���
	static bool StandardSplitSent(const string & src, vector<string> & sent_vec);
	// �Ե������ӽ���Ԥ����,���˶���ո�
	// ƥ��ʵ� & ģ��
	static bool StandardPreSentProcess(SentProcCell & sent_cell);
};

#endif //TRANSMASTER_LANGUAGEPROCESS_H
