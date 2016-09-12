#ifndef DICTIONARY_H
#define DICTIONARY_H

//MAIN
#include "BaseWord.h"
#include "DictDef.h"

//BOOST
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

//STL
#include <map>
using namespace std;

namespace dictionary
{


class Dictionary
	: public boost::enable_shared_from_this<Dictionary>
{
public:
	Dictionary(const DictInfo & _dict_info) : dict_info(_dict_info) {}
	~Dictionary() {}

	string DebugInfo();

public:

	DictInfo dict_info;

	map<WordID, word_ptr> word_map;

	string weight;
};

typedef boost::shared_ptr<Dictionary> dict_ptr;

}

#endif //DICTIONARY_H
