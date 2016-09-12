#include "LanguageProcess.h"

bool LanguageProcess::StandardPreTextProcess(string &src)
{
	CLanguage::Utf8A2B(src);

	return true;
}

bool LanguageProcess::StandardSplitSent(const string &src, vector<string> &sent_vec)
{
	string token_src(src);

	CLanguage::SplitStrByChar(token_src);
	// 按照中文方式切分句子(包含的标点更多)
	ssplite::ChSentSpliter spliter(token_src, false);
	// ssplite::StandardSentSpliter spliter(token_src, false);
	sent_vec = spliter.GetSplitResult();

	return true;
}

bool LanguageProcess::StandardPreSentProcess(SentProcCell &sent_cell)
{
	// 过滤句子中的多个空格
	CLanguage::StandardSentFilter(sent_cell);

	// 匹配词典
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSentByMaxSequence(sent_cell.usr_id, sent_cell.domain, true,
																   sent_cell.trans_src._src_str, match_result_list);

	for (list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	// 匹配模版,应该可以做成空接口,以便后期添加模板
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain,
													sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);
	return true;
}