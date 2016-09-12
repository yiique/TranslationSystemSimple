/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#ifndef _FILE_H_
#define _FILE_H_

#include <fstream>
#include <string>
#include <iostream>

namespace tibetsentenizer
{
	class File
	{
	public:
		static bool openInputFile(std::ifstream& ifs, std::string fileName);
		static bool openOutputFile(std::ofstream& ofs, std::string fileName);
	};
}

#endif //_FILE_H_
