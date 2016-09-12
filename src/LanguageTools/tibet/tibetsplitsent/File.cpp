/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#include "File.h"
using namespace tibetsentenizer;

bool File::openInputFile(std::ifstream& ifs, std::string fileName)
{
    ifs.clear();
    ifs.open(fileName.c_str(), std::ios::in);
   
	if(!ifs.good())
    {
        std::cerr << "Can't open the file " << fileName << " to read!" <<std::endl;
        return false;
    }

    return true;
}

bool File::openOutputFile(std::ofstream& ofs, std::string fileName)
{
    ofs.clear();
    ofs.open(fileName.c_str(), std::ios::out);
   
	if(!ofs.good())
    {
        std::cerr << "Can't open the file " << fileName << " to write!"  << std::endl;
        return false;
    }

    return true;
}
