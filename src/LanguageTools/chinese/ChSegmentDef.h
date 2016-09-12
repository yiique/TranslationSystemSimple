#ifndef CH_SEGMENT_DEF_H
#define CH_SEGMENT_DEF_H

//BOOST
#include <boost/shared_ptr.hpp>


//MAIN
#include "Common/f_utility.h"

//STL
#include <string>
using namespace std;

class ChSegWord
{
public:
	ChSegWord(const string & _word, const int _is_active = 1): word_id(-1), key(_word), word(_word), is_active(_is_active)
	{
		filter_head_tail(key);
	}

	~ChSegWord(void) {}

	void ResetWord(const string & _word)
	{
		word = _word;
		key = word;
		filter_head_tail(key);
	}

	int word_id;
	string key;
	string word;
	int    is_active; //1为启用 0为不启用
};

typedef boost::shared_ptr<ChSegWord> seg_word_share_ptr;

#endif //CH_SEGMENT_DEF_H
