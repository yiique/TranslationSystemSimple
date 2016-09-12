#ifndef LANGUAGE_PROCESS_H
#define LANGUAGE_PROCESS_H

//MAIN
#include "LanguageTools/CLanguage.h"
#include "TransText/TransText.h"

//STL
#include <string>
#include <vector>
using namespace std;

#ifdef ALIGMENT_OUTPUT


class LanguageProcess
{
public:


	//预处理
	static bool ChPreTextProcess(string & src, TransAlignment & align);
	static bool ChSpliteSent(const string & src, vector<string> & sent_vec, TransAlignment & align);
	static bool ChPreSentProcess(SentProcCell & sent_cell);
	static bool ChMenuPreSentProcess(SentProcCell & sent_cell);

	static bool EnPreTextProcess(string & src, TransAlignment & align);
	static bool EnSpliteSent(const string & src, vector<string> & sent_vec, TransAlignment & align);
	static bool EnPreSentProcess(SentProcCell & sent_cell);

	static bool UyTradPreTextProcess(string & src, TransAlignment & align);
	static bool UyLatinPreTextProcess(string & src, TransAlignment & align);
	static bool UySpliteSent(const string & src, vector<string> & sent_vec, TransAlignment & align);
	static bool UyPreSentProcess(SentProcCell & sent_cell);

	static bool TibetPreTextProcess(string & src, TransAlignment & align);
	static bool TibetSpliteSent(const string & src, vector<string> & sent_vec, TransAlignment & align);
	static bool TibetPreSentProcess(SentProcCell & sent_cell);

private:

	static void parse_align_from_token(const string & src, const string & tgt,vector<BaseAlign> & align_vec);
	static void parse_align_from_splite(const vector<string> & sent_vec, vector<BaseAlign> & align_vec);
	static size_t count_char(const string & src, const size_t offset, const size_t len);

};

#else //ALIGMENT_OUTPUT

class LanguageProcess
{
public:


	//预处理
	static bool ChPreTextProcess(string & src);
	static bool ChSpliteSent(const string & src, vector<string> & sent_vec);
	static bool ChPreSentProcess(SentProcCell & sent_cell);
	static bool ChMenuPreSentProcess(SentProcCell & sent_cell);

	static bool EnPreTextProcess(string & src);
	static bool EnSpliteSent(const string & src, vector<string> & sent_vec);
	static bool EnPreSentProcess(SentProcCell & sent_cell);

	static bool UyTradPreTextProcess(string & src);
	static bool UyLatinPreTextProcess(string & src);
	static bool UySpliteSent(const string & src, vector<string> & sent_vec);
	static bool UyPreSentProcess(SentProcCell & sent_cell);

	static bool TibetPreTextProcess(string & src);
	static bool TibetSpliteSent(const string & src, vector<string> & sent_vec);
	static bool TibetPreSentProcess(SentProcCell & sent_cell);

	static bool MongolianPreTextProcess(string & src);
	static bool MongolianSpliteSent(const string & src, vector<string> & sent_vec);
	static bool MongolianPreSentProcess(SentProcCell & sent_cell);

	static bool NEWMongolianPreTextProcess(string & src);
	static bool NEWMongolianSpliteSent(const string & src, vector<string> & sent_vec);
	static bool NEWMongolianPreSentProcess(SentProcCell & sent_cell);

	static bool KoreanPreTextProcess(string & src);
	static bool KoreanSpliteSent(const string & src, vector<string> & sent_vec);
	static bool KoreanPreSentProcess(SentProcCell & sent_cell);

	static bool VietnamPreTextProcess(string & src);
	static bool VietnamSpliteSent(const string & src, vector<string> & sent_vec);
	static bool VietnamPreSentProcess(SentProcCell & sent_cell);

	static bool ThaiPreTextProcess(string & src);
	static bool ThaiSpliteSent(const string & src, vector<string> & sent_vec);
	static bool ThaiPreSentProcess(SentProcCell & sent_cell);

	static bool RussianPreTextProcess(string& src);

	static bool JapanesePreTextProcess(string& src);
	static bool JapaneseSpliteSent(const string& src, vector<string>& sent_vec);
	static bool JapanesePreSentProcess(SentProcCell& sent_cell);

	static bool OthPreTextProcess(string& src);
	static bool OthCharSpliteSent(const string& src, vector<string>& sent_vec);
	static bool OthWordSpliteSent(const string& src, vector<string>& sent_vec);
	static bool OthCharPreSentProcess(SentProcCell& sent_cell);
};

#endif //ALIGMENT_OUTPUT

#endif //LANGUAGE_PROCESS
