#ifndef _CY2LOWER_H_
#define _CY2LOWER_H_
#include <string>
#include "../../CodeConver.h"

namespace Cy2Lower
{
class CCy2Lower
{
public:
	bool Init(const string& _path)
	{
		return m_Conver.InitMap(_path);
	}

	string Conver(const string& _line)
	{
		return m_Conver.ConverMap(_line);
	}
private:
	CodeConver m_Conver;
};

}

#endif