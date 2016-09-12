#include "LanguageProcess.h"
#include "LanguageTools/SentSpliter.h"
#include "Common/f_utility.h"
#include "DictLibrary/DictManager.h"
#include "TemplateLibrary/UsrTemplateManager.h"
#include "LanguageTools/Utf8ZhHant2CN.h"
#include "SipRPC/SipRPC.h"
#include "Configurator/Configurator.h"

#ifdef ALIGMENT_OUTPUT

size_t LanguageProcess::count_char(const string & src, const size_t offset, const size_t len)
{
	Utf8Stream utf8_ss(src.substr(offset, len));

	string utf8_char;
	size_t cnt = 0;
	while(utf8_ss.ReadOneCharacter(utf8_char))
	{
		cnt++;
	}

	return cnt;
}

void LanguageProcess::parse_align_from_token(const string & src, const string & tgt, vector<BaseAlign> & align_vec)
{
	align_vec.clear();
	vector<string> tgt_vec;
	split_string_by_blank(tgt.c_str(), tgt_vec);

	size_t src_offset = 0;
	size_t char_offset = 0;
	for(size_t i=0; i<tgt_vec.size(); ++i)
	{
		const string & tgt_word = tgt_vec[i];

		size_t pos = src.find(tgt_word, src_offset);

		if(pos != string::npos)
		{
			if(pos > src_offset)
				char_offset += count_char(src, src_offset, pos-src_offset);

			size_t char_size = count_char(src, pos, tgt_word.size());

			align_vec.push_back(BaseAlign(char_offset, char_size, i, 1));
			src_offset = pos + tgt_word.size();
			char_offset += char_size;
	
		}else
		{
			//fix it
		}
	}
}

void LanguageProcess::parse_align_from_splite(const vector<string> & sent_vec, vector<BaseAlign> & align_vec)
{
	size_t src_offset = 0;
	align_vec.clear();
	for(size_t i=0; i<sent_vec.size(); ++i)
	{
		size_t word_cnt = count_words(sent_vec[i]);
		align_vec.push_back(BaseAlign(src_offset, word_cnt, i, 1));
		src_offset += word_cnt;
	}
}

bool LanguageProcess::ChPreTextProcess(string & src, TransAlignment & align)
{
    //·±¼ò×ª»» fix it
    //utf8zhhant2cn::Utf8ZhHant2CN(src);

	CLanguage::Utf8A2B(src, align.src_a2b_vec);

	//·Ö´Ê
	string seg_tgt;
	CLanguage::ChSegment(src, seg_tgt);
	parse_align_from_token(src, seg_tgt, align.a2b_seg_vec);

	src = seg_tgt;

	return true;
}

bool LanguageProcess::ChSpliteSent(const string & src, vector<string> & sent_vec, TransAlignment & align)
{
	ssplite::ChSentSpliter spliter(src, false);

	//lout << "splite src1 = [" << src << "]" << endl;
	sent_vec = spliter.GetSpliteResult();
	//lout << "splite src2 = [" << sent_vec[0] << "]" << endl;
	parse_align_from_splite(sent_vec, align.seg_splite_vec);

	return true;
}

bool LanguageProcess::ChPreSentProcess(SentProcCell & sent_cell)
{
	

#ifdef CH_SEG_WITH_TAG
	//È¥µô´ÊÐÔ±ê×¢
	string sent_src = sent_cell.trans_src._src_str; 
	CLanguage::RemoveChPosTag(no_tag_src);
#else
	const string & sent_src = sent_cell.trans_src._src_str; 
#endif

	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_src, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_src, sent_cell.trans_src._dict_result_vec);

	return true;
}

bool LanguageProcess::EnPreTextProcess(string & src, TransAlignment & align)
{
	//a2b
	CLanguage::Utf8A2B(src, align.src_a2b_vec);

	//Token
	string token_src;
	CLanguage::EnToken(src, token_src);
	parse_align_from_token(src, token_src, align.a2b_seg_vec);

	src = token_src;

	return true;
}


bool LanguageProcess::EnSpliteSent(const string & src, vector<string> & sent_vec, TransAlignment & align)
{
	ssplite::EnSentSpliter spliter(src, false);

	sent_vec = spliter.GetSpliteResult();

	parse_align_from_splite(sent_vec, align.seg_splite_vec);

	return true;
}

bool LanguageProcess::EnPreSentProcess(SentProcCell & sent_cell)
{
	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}


	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}

bool LanguageProcess::UyPreSentProcess(SentProcCell & sent_cell)
{
	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}


	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}

bool LanguageProcess::ChMenuPreSentProcess(SentProcCell & sent_cell)
{
	//²Ëµ¥ÁìÓò²»ÐèÒª´ÊÐÔ±ê×¢£¬²»ÐèÒª´ÊµäºÍÄ£°åÆ¥Åä£¬ÐèÒªºÏ²¢·Ö´Ê½á¹û
	string & str = sent_cell.trans_src._src_str; 

	vector<string> word_vec;
	filter_head_tail(str);
	split_string_by_blank(str.c_str(), word_vec);
	
	str.clear();
	
	for(size_t i=0; i<word_vec.size(); ++i)
	{
	    //È¥µô´ÊÐÔ±ê¼Ç
	    size_t pos = word_vec[i].rfind("/");
	
	    if(string::npos != pos)
	        str += word_vec[i].substr(0, pos);
	    else
	        str += word_vec[i];
	}
	
	return true;
}

bool LanguageProcess::UyTradPreTextProcess(string & src, TransAlignment & align)
{

	//×ª»»ÎªladingÎ¬ÎÄ
	CLanguage::UyghurOld2Latin(src);

	//a2b
	CLanguage::Utf8A2B(src);

	//uyToken
	string token_src;
	CLanguage::UyToken(src, token_src);
    
    src = token_src;
	return true;

}

bool LanguageProcess::UyLatinPreTextProcess(string & src, TransAlignment & align)
{
	//a2b
	CLanguage::Utf8A2B(src);

	//uyToken
	string token_src;
	CLanguage::UyToken(src, token_src);
    
    src = token_src;
	return true;

}

bool LanguageProcess::UySpliteSent(const string & src, vector<string> & sent_vec, TransAlignment & align)
{
	ssplite::UySentSpliter spliter(src, false);

	sent_vec = spliter.GetSpliteResult();

	return true;
}


bool LanguageProcess::TibetPreTextProcess(string & src, TransAlignment & align)
{
	//·Ö´Ê
	siprpc::rpc_data_ptr sp_data(new siprpc::SipRpcData());

	sp_data->data_vec.push_back(src);
	
	siprpc::rpc_data_ptr sp_res = siprpc::SipRPC::Request(Configurator::_s_ti_rpc_ip, Configurator::_s_ti_rpc_port, "tibetrec.do", sp_data);

	if(sp_res)
	{
		if(sp_res->data_vec.size() == 4)
		{
			lout << "Tibet rec result : " << sp_res->data_vec[0] << "  " << sp_res->data_vec[1] << "  " << sp_res->data_vec[2] << endl;
			
			if(sp_res->data_vec[0] == "TRUE")
				src = sp_res->data_vec[3];
			else
				return false;

		}else
		{
			lerr << "Error kr sip rpc failed. result size != 4" << endl;
			return false;
		}
	}else
	{
		lerr << "Error kr sip rpc failed." << endl;
		return false;
	}


	//A2B
	CLanguage::Utf8A2B(src);

	return true;
}

bool LanguageProcess::TibetSpliteSent(const string & src, vector<string> & sent_vec, TransAlignment & align)
{
	CLanguage::TibetSplitSent(src, sent_vec);

	return true;
}

bool LanguageProcess::TibetPreSentProcess(SentProcCell & sent_cell)
{
	//·Ö´Ê
	string seg_tgt;
	CLanguage::TibetSegment(sent_cell.trans_src._src_str, seg_tgt);
	sent_cell.trans_src._src_str = seg_tgt;

	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}


#else //ALIGMENT_OUTPUT

bool LanguageProcess::ChPreTextProcess(string & src)
{
    //·±¼ò×ª»»
    utf8zhhant2cn::Utf8ZhHant2CN(src);

	//a2b
	//lout << "a src = " << src << endl;
	CLanguage::Utf8A2B(src);
	//lout << "b src = " << src << endl;

	return true;
}

bool LanguageProcess::ChSpliteSent(const string & src, vector<string> & sent_vec)
{

	string token_src(src);
	CLanguage::SpliteStrByChar(token_src);
	ssplite::ChSentSpliter spliter(token_src, false);

	//lout << "splite src1 = [" << src << "]" << endl;
	sent_vec = spliter.GetSpliteResult();
	//lout << "splite src2 = [" << sent_vec[0] << "]" << endl;

	return true;
}

bool LanguageProcess::ChPreSentProcess(SentProcCell & sent_cell)
{
	//ÏÈÆ¥ÅäÓÃ»§´Êµä£¬ÓÃÓÚÇ¿ÖÆ·Ö´Ê
	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> seg_result_list;
	set<string> force_seg_dict;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, seg_result_list);

	for(list<dictionary::MatchResult>::iterator iter = seg_result_list.begin(); iter != seg_result_list.end(); ++iter)
	{
		//lout << "force seg match : " << iter->src << endl;
		if(iter->type == DICT_TYPE_USR)
			force_seg_dict.insert(iter->src);
	}

	//ºÏ²¢¿Õ¸ñ
	string noblank_sent;
	for(size_t i=0; i<sent_cell.trans_src._src_str.size(); ++i)
		if(sent_cell.trans_src._src_str[i] != ' ')
			noblank_sent += sent_cell.trans_src._src_str[i];

	//·Ö´Ê
	string seg_tgt;
	CLanguage::ChSegment(noblank_sent, seg_tgt, force_seg_dict); //TODO
	sent_cell.trans_src._src_str = seg_tgt;
	sent_cell.trans_src._pre_proc_result = seg_tgt;
	

#ifdef CH_SEG_WITH_TAG
	//È¥µô´ÊÐÔ±ê×¢
	string sent_src = sent_cell.trans_src._src_str; 
	CLanguage::RemoveChPosTag(no_tag_src);
#else
	const string & sent_src = sent_cell.trans_src._src_str; 
#endif

	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_src, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_src, sent_cell.trans_src._dict_result_vec);

	return true;
}

bool LanguageProcess::EnPreTextProcess(string & src)
{
	//a2b
	CLanguage::Utf8A2B(src);

	//Token
	string token_src;
	CLanguage::EnToken(src, token_src);

	src = UyLower::to_lower(token_src);

	return true;
}


bool LanguageProcess::EnSpliteSent(const string & src, vector<string> & sent_vec)
{
	ssplite::EnSentSpliter spliter(src, false);

	sent_vec = spliter.GetSpliteResult();

	return true;
}

bool LanguageProcess::EnPreSentProcess(SentProcCell & sent_cell)
{

	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}


	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}

bool LanguageProcess::UyPreSentProcess(SentProcCell & sent_cell)
{
	//lout << "PreSentProcess" << endl; // LK Debug
	//lout << "NE:" << sent_cell.trans_src._src_str << endl; // LK Debug
	//Æ¥ÅäNE
	//CLanguage::UyNetransLatin(sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);
	//lout << "NE size:" << sent_cell.trans_src._dict_result_vec.size() << endl; // LK Debug
	//for (unsigned int i = 0 ; i < sent_cell.trans_src._dict_result_vec.size(); i++) // LK Debug
	//	lout << sent_cell.trans_src._dict_result_vec[i] << endl;
	//lout << "Match Dict" << endl; // LK Debug
	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);
	//lout << "for push back" << endl; // LK Debug
	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}


	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);
	//lout << "PreSentProcess Done" << endl; // LK Debug
	return true;
}

bool LanguageProcess::ChMenuPreSentProcess(SentProcCell & sent_cell)
{
	//²Ëµ¥ÁìÓò²»ÐèÒª´ÊÐÔ±ê×¢£¬²»ÐèÒª´ÊµäºÍÄ£°åÆ¥Åä£¬ÐèÒªºÏ²¢·Ö´Ê½á¹û
	string & str = sent_cell.trans_src._src_str; 

	vector<string> word_vec;
	filter_head_tail(str);
	split_string_by_blank(str.c_str(), word_vec);
	
	str.clear();
	
	for(size_t i=0; i<word_vec.size(); ++i)
	{
	    //È¥µô´ÊÐÔ±ê¼Ç
	    size_t pos = word_vec[i].rfind("/");
	
	    if(string::npos != pos)
	        str += word_vec[i].substr(0, pos);
	    else
	        str += word_vec[i];
	}
	
	return true;
}

bool LanguageProcess::UyTradPreTextProcess(string & src)
{

	//×ª»»ÎªlatinÎ¬ÎÄ
	CLanguage::UyghurOld2Latin(src);

	//a2b
	CLanguage::Utf8A2B(src);

	//uyToken
	string token_src;
	CLanguage::UyToken(src, token_src);
    
    src = token_src;
	return true;

}

bool LanguageProcess::UyLatinPreTextProcess(string & src)
{
	//lout << "UyOld2Latin" << endl; // LK Debug
	//×ª»»ÎªlatinÎ¬ÎÄ
	CLanguage::UyghurOld2Latin(src);
	//lout << "UyA2B" << endl; // LK Debug
	//a2b
	CLanguage::Utf8A2B(src);
	//lout << "UyToken" << endl; // LK Debug
	//uyToken
	string token_src;
	CLanguage::UyToken(src, token_src);
    
    src = token_src;
	//lout << "PreProcess Done" << endl; // LK Debug
	return true;

}

bool LanguageProcess::UySpliteSent(const string & src, vector<string> & sent_vec)
{
	//lout << "PreSent" << endl; // LK Debug
	ssplite::UySentSpliter spliter(src, false);

	sent_vec = spliter.GetSpliteResult();
	//sent_vec.push_back(src);
	//lout << "PreSent Done" << endl; // LK Debug
	return true;
}


bool LanguageProcess::TibetPreTextProcess(string & src)
{
	//·Ö´Ê
	siprpc::rpc_data_ptr sp_data(new siprpc::SipRpcData());

	sp_data->data_vec.push_back(src);
	
	siprpc::rpc_data_ptr sp_res = siprpc::SipRPC::Request(Configurator::_s_ti_rpc_ip, Configurator::_s_ti_rpc_port, "tibetrec.do", sp_data);

	if(sp_res)
	{
		if(sp_res->data_vec.size() == 4)
		{
			//lout << "Tibet rec result : " << sp_res->data_vec[0] << "  " << sp_res->data_vec[1] << "  " << sp_res->data_vec[2] << endl;
			
			if(sp_res->data_vec[0] == "TRUE")
				src = sp_res->data_vec[3];
			else
				return false;

		}else
		{
			lerr << "Error kr sip rpc failed. result size != 4" << endl;
			return false;
		}
	}else
	{
		lerr << "Error kr sip rpc failed." << endl;
		return false;
	}


	//A2B
	CLanguage::Utf8A2B(src);

	return true;
}

bool LanguageProcess::TibetSpliteSent(const string & src, vector<string> & sent_vec)
{
	CLanguage::TibetSplitSent(src, sent_vec);


	return true;
}

bool LanguageProcess::TibetPreSentProcess(SentProcCell & sent_cell)
{
	//·Ö´Ê
	string seg_tgt;
	CLanguage::TibetSegment(sent_cell.trans_src._src_str, seg_tgt);
	sent_cell.trans_src._src_str = seg_tgt;
	sent_cell.trans_src._pre_proc_result = seg_tgt;

	//Êý´ÊÊ¶±ð
	CLanguage::TibetNumberTrans(sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}


bool LanguageProcess::KoreanPreTextProcess(string & src)
{
	//a2b
	CLanguage::Utf8A2B(src);

	//uyToken
	string token_src;
	CLanguage::UyToken(src, token_src);
    
    src = token_src;

	return true;
}

bool LanguageProcess::KoreanSpliteSent(const string & src, vector<string> & sent_vec)
{
	ssplite::UySentSpliter spliter(src, false);

	sent_vec = spliter.GetSpliteResult();

	return true;

}

bool LanguageProcess::KoreanPreSentProcess(SentProcCell & sent_cell)
{
	//cout << "before segment : " << sent_cell.trans_src._src_str << endl;
	string result = CLanguage::KoreanSeg(sent_cell.trans_src._src_str);
	sent_cell.trans_src._src_str = result;
	//cout << "after segment : " << result << endl;
	/*
	//·Ö´Ê
	siprpc::rpc_data_ptr sp_data(new siprpc::SipRpcData());

	sp_data->data_vec.push_back(sent_cell.trans_src._src_str);
	
	siprpc::rpc_data_ptr sp_res = siprpc::SipRPC::Request(Configurator::_s_kr_rpc_ip, Configurator::_s_kr_rpc_port, "kranalysis.do", sp_data);

	if(sp_res)
	{
		if(sp_res->data_vec.size() > 0)
		{
			sent_cell.trans_src._src_str = sp_res->data_vec[0];
		}else
		{
			lerr << "Error kr sip rpc failed. result size = 0" << endl;
			return false;
		}
	}else
	{
		lerr << "Error kr sip rpc failed." << endl;
		return false;
	}
	*/

	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}

bool LanguageProcess::MongolianPreTextProcess(string & src)
{
	CLanguage::Utf8A2B(src);

	//×ª»»Îªlatin
	CLanguage::Mongolian2Latin(src);

	src = CLanguage::MongolianToken(src);

	return true;

}

bool LanguageProcess::MongolianSpliteSent(const string & src, vector<string> & sent_vec)
{
	//Ê¹ÓÃÎ¬ÓïµÄ¶Ï¾ä
	ssplite::UySentSpliter spliter(src, false);

	sent_vec = spliter.GetSpliteResult();

	return true;
}

bool LanguageProcess::MongolianPreSentProcess(SentProcCell & sent_cell)
{
	//Æ¥ÅäNE
	CLanguage::MongolianNumDate(sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}


bool LanguageProcess::NEWMongolianPreTextProcess(string & src)
{
	//uyToken
	string token_src;
	CLanguage::UyToken(src, token_src);

	src = token_src;

	CLanguage::NEWMongolianLower(src);


	return true;
}

bool LanguageProcess::NEWMongolianSpliteSent(const string & src, vector<string> & sent_vec)
{
	//Ê¹ÓÃÎ¬ÓïµÄ¶Ï¾ä
	ssplite::UySentSpliter spliter(src, false);

	sent_vec = spliter.GetSpliteResult();

	return true;
}

bool LanguageProcess::NEWMongolianPreSentProcess(SentProcCell & sent_cell)
{
	
	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}


bool LanguageProcess::VietnamPreTextProcess(string & src)
{
	//uyToken
	string token_src;
	CLanguage::UyToken(src, token_src);

	src = token_src;

	CLanguage::VietnamLowercase(src);


	return true;
}

bool LanguageProcess::VietnamSpliteSent(const string & src, vector<string> & sent_vec)
{
	//Ê¹ÓÃÎ¬ÓïµÄ¶Ï¾ä
	ssplite::UySentSpliter spliter(src, false);

	sent_vec = spliter.GetSpliteResult();

	return true;
}

bool LanguageProcess::VietnamPreSentProcess(SentProcCell & sent_cell)
{
	
	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}

bool LanguageProcess::ThaiPreTextProcess(string & src)
{
	return true;
}

bool LanguageProcess::ThaiSpliteSent(const std::string &input, std::vector<std::string> &splits)
{

	//cout << "ThaiSpliteSent start. " << endl;
	splits.push_back(input);
	//if (input != "")
	//{	
	//	std::istringstream iss(input);
	//	std::string split;
	//	splits.clear();
	//	std::vector<std::string> tmp_split;

	//	while (iss >> split)
	//	{
	//		tmp_split.push_back(split);
	//	}
	//	
	//	size_t split_size = tmp_split.size();
	//	if (split_size > 0)
	//	{
	//		std::string sent = tmp_split[0];

	//		for (size_t i = 1; i < split_size; ++i)
	//		{
	//			if (sent.size() > 30)
	//			{
	//				splits.push_back(sent);
	//				sent = tmp_split[i];
	//			}
	//			else
	//				sent += tmp_split[i] + " ";
	//		}
	//		if (sent != "")
	//			splits.push_back(sent);
	//	}
	//}

	//cout << "ThaiSpliteSent end. " << endl;
	return true;
}

bool LanguageProcess::ThaiPreSentProcess(SentProcCell & sent_cell)
{
	//cout << "ThaiSegment start. " << endl;
	CLanguage::ThaiSegment(sent_cell.trans_src._src_str);
	//cout << "ThaiSegment end. " << endl;


	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	//cout << "ThaiPreSentProcess end. " << endl;

	return true;
}

bool LanguageProcess::RussianPreTextProcess(string& src)
{
	//¶íÓï×ªÐ¡Ð´
	src = CLanguage::RussianLowercase(src);

	//a2b
	CLanguage::Utf8A2B(src);

	//uyToken
	string token_src;
	CLanguage::UyToken(src, token_src);
    
    src = token_src;
	return true;
}

bool LanguageProcess::JapanesePreTextProcess(string & src)
{
	//lout << "JPA2B" << endl; // LK Debug
	//a2b
	CLanguage::Utf8A2B(src);
	//lout << "JPToken" << endl; // LK Debug
	//uyToken
	string token_src;
	CLanguage::UyToken(src, token_src);
    
    src = token_src;
	//lout << "JP TextPreProcess Done" << endl; // LK Debug
	return true;
}

bool LanguageProcess::JapaneseSpliteSent(const string& src, vector<string>& sent_vec)
{
	//lout << "JP PreSent" << endl; // LK Debug
	string token_src(src);
	CLanguage::SpliteStrByChar(token_src);
	ssplite::ChSentSpliter spliter(token_src, false);
	sent_vec = spliter.GetSpliteResult();
	//lout << "JP sentence size:" << sent_vec.size() << endl;
	//lout << "JP PreSent Done" << endl; // LK Debug
	return true;
}

bool LanguageProcess::JapanesePreSentProcess(SentProcCell& sent_cell)
{
	//cout << "JP Segment start. " << endl;
	// to be added
	string noblank_sent;
	for(size_t i=0; i<sent_cell.trans_src._src_str.size(); ++i)
		if(sent_cell.trans_src._src_str[i] != ' ')
			noblank_sent += sent_cell.trans_src._src_str[i];

	sent_cell.trans_src._src_str = CLanguage::JapaneseSeg(noblank_sent);
	sent_cell.trans_src._pre_proc_result = sent_cell.trans_src._src_str;
	//CLanguage::ThaiSegment(sent_cell.trans_src._src_str);
	//cout << "JP Segment end. " << endl;


	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}

	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	//cout << "JP PreSentProcess end. " << endl;

	return true;

}

bool LanguageProcess::OthPreTextProcess(string & src)
{
	//a2b
	CLanguage::Utf8A2B(src);
	//lout << "Other Token" << endl;
	//Token
	string token_src;
	CLanguage::UyToken(src, token_src);

	src = token_src;
	return true;
}

bool LanguageProcess::OthCharSpliteSent(const string& src, vector<string>& sent_vec)
{
	string token_src(src);
	CLanguage::SpliteStrByChar(token_src);
	ssplite::ChSentSpliter spliter(token_src, false);
	sent_vec = spliter.GetSpliteResult();
	//lout << "OthChar sentence size:" << sent_vec.size() << endl;
	return true;
}

bool LanguageProcess::OthWordSpliteSent(const string & src, vector<string> & sent_vec)
{
	ssplite::EnSentSpliter spliter(src, false);
	sent_vec = spliter.GetSpliteResult();
	return true;
}

bool LanguageProcess::OthCharPreSentProcess(SentProcCell & sent_cell)
{
	//Æ¥Åä´Êµä
	list<dictionary::MatchResult> match_result_list;
	dictionary::DictManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, false, sent_cell.trans_src._src_str, match_result_list);

	for(list<dictionary::MatchResult>::iterator iter = match_result_list.begin(); iter != match_result_list.end(); ++iter)
	{
		sent_cell.trans_src._dict_result_vec.push_back(iter->serialize());
	}


	//Æ¥ÅäÄ£°å
	UsrTemplateLibManager::GetInstance()->MatchSent(sent_cell.usr_id, sent_cell.domain, sent_cell.trans_src._src_str, sent_cell.trans_src._dict_result_vec);

	return true;
}


#endif //ALIGMENT_OUTPUT
