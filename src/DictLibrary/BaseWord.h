#ifndef BASE_WORD_H
#define BASE_WORD_H

//MAIN
#include "DictDef.h"

//BOOST
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

//STL
#include <string>
#include <set>
#include <vector>
#include <map>
using namespace std;

namespace dictionary
{


class Dictionary;
typedef boost::shared_ptr<Dictionary> dict_ptr;

class UWord
{
public:
	size_t word_id;
	int is_active;
	int is_checked;
};

class BaseWord
	: public boost::enable_shared_from_this<BaseWord>
{
public:

	string key;
	string src;
	string tgt;

	string DebugInfo(dict_ptr sp_dict) const;

	map<dict_ptr, UWord> dict_map; //所属词典的集合

	vector<dict_ptr> CheckOwner(const UsrID & usrid, const DomainType & domain_info);
};

typedef boost::shared_ptr<BaseWord> word_ptr;
}

#endif //BASE_WORD_H
