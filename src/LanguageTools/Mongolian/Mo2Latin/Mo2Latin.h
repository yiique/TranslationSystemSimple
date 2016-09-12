#ifndef _MO2LATIN_H_
#define _MO2LATIN_H_
#include "../../CodeConver.h"

namespace CodeConverNameSpace
{
	class Mo2Latin
	{
	public:
		bool Init(const string& _path)
		{
			return m_Conver.InitTire(_path);
		}

		string Conver(const string& _line)
		{
			return m_Conver.ConverTire(_line);
		}
	private:
		CodeConver m_Conver;
	};

}
#endif
