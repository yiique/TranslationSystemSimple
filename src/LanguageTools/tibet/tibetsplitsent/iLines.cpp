/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#include "iLines.h"
using namespace tibetsentenizer;

bool iLines::__readFrom(std::string fileName)
{
    std::ifstream ifs;
    bool open_file = File::openInputFile(ifs, fileName);

	if (!open_file)
	{
		return false;
	}

    std::string line;
    
	while (getline(ifs, line))
    {
		if(!line.empty())
		{
			this->push_back(trim(line));
		}
    }

    ifs.close();

    return true;
}

bool iLines::__writeTo(std::string fileName)
{
    std::ofstream ofs;
	bool open_file = File::openOutputFile(ofs, fileName);

	if (!open_file)
	{
		return false;
	}

    for (size_t i = 0; i < this->size(); ++i)
    {
        ofs << this->at(i) << std::endl;
    }

    ofs.close();

    return true;
}
