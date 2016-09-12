#ifndef WORD_INDEX_H
#define WORD_INDEX_H

//MAIN
#include "BaseWord.h"

//BOOST
#include <boost/unordered_map.hpp>

//STL
#include <string>
#include <set>
#include <map>
#include <list>
using namespace std;

namespace dictionary 
{


class WordIndex
{
public:

	bool Search(const string & key, list<word_ptr> & word_list);

	word_ptr Insert(word_ptr & sp_word);
	void Erase(word_ptr & sp_word);

	int Size();

private:

	typedef boost::unordered_map<string, set<word_ptr> > word_map_t; 
    //typedef map<string, set<word_ptr> > word_map_t; 
	word_map_t m_word_map;
	
};

}
#endif //WORD_INDEX_H
