#ifndef _MONUMDATE_H_
#define _MONUMDATE_H_

#include "NewUyNERec.h"

namespace MoNumDate
{
	class CMoNumDate
	{
	public:
		bool Init(const string& path)
		{
			return m_rec.Init(path + "/" + "MoNe.cfg", path + "/" + "MoNeNum.cfg");
		}
		
		vector<string> GetNERules(const string& _strIn)
		{
			vector<string> vNERule;
			m_rec.NeRec(_strIn, "no", true);
			m_rec.OutputNERule(_strIn, vNERule, "no");
			return vNERule;
		}
		
	private:
		NewMoNERec m_rec;
	};
}

#endif