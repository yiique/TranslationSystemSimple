/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#ifndef _CREATFEAT_H_
#define _CREATFEAT_H_

#include "Common.h"
#include "TibetSentParse.h"
#include "Common/Utf8Stream.h"

namespace tibetsentenizer
{
	class CreatFeat 
	{
	public:
		CreatFeat (void){}
		CreatFeat (const std::map<std::string, bool> featActive) : m_featActive(featActive){}
		~CreatFeat(void){}

		bool isActive(const std::string& feat);
		void getFeat(const VecOfVecOfWordProperty& wordVec, VecFeatTemplate& featTemplate);

	private:
		const std::map<std::string, bool> m_featActive;
	};
}

#endif //_CREATFEAT_H_
