/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#ifndef _ILINES_H_
#define _ILINES_H_

#include "File.h"
#include "Common.h"
#include <vector>
#include <string>

namespace tibetsentenizer
{
	class iLines:public std::vector<std::string> 
	{
	public:
		iLines(void){}
		~iLines(void){}

		bool __readFrom(std::string fileName);
		bool __writeTo(std::string fileName);
	};
}

#endif //_ILINES_H_
