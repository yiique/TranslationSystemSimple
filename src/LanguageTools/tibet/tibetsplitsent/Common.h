/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#include "iLines.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <utility>

namespace tibetsentenizer
{
	#define NPOS std::string::npos

	#define SEGSYSMBOL    1
	#define UNSEGSYSMBOL -1
	#define WORDSYSMBOL   0

	class iLines;

	typedef std::vector<std::string>                VecOfStr;
	typedef std::vector<size_t>                     VecOfInt;
	typedef std::vector<iLines>                     VecFeatTemplate;
	typedef std::pair<std::string, int>             WordProperty;
	typedef std::pair<std::string, std::string>     RuleProperty;
	typedef std::vector<WordProperty >              VecOfWordProperty;
	typedef std::vector<VecOfWordProperty >         VecOfVecOfWordProperty;
	typedef std::vector<RuleProperty >              VecOfRule;
	typedef std::set<std::string>                   DictSet;
	typedef VecOfWordProperty::const_iterator	    VecOfWordPropertyConstIter;
	typedef VecOfVecOfWordProperty::const_iterator  VecOfVecOfWordPropertyConstIter;

	std::string trim(const std::string& str);
	std::string num2str(const int num);

	const std::string& getCharSign(void);
	const std::string& getSentSign(void);
	const std::string& getCloudSign(void);
	const std::string& getSign1(void);
	const std::string& getSign2(void);
	extern void setSign();

	static std::string char_sign, sent_sign, cloud_sign, sign1, sign2;

	class CharCmpLess
	{
	public:
		bool operator() (const char* l, const char* r) const
		{
			return std::strcmp(l, r) < 0;
		}
	};
}

#endif //_COMMON_H_
