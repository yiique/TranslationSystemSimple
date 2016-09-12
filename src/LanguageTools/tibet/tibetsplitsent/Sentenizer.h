/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#ifndef _SENTENZIER_H_
#define _SENTENZIER_H_

#include "Common.h"
#include "TibetSentParse.h"
#include "MaxEntModel.h"
#include "CreatFeat.h"
#include "Segmenter.h"

namespace tibetsentenizer
{
	class Sentenizer
	{
	public:
		Sentenizer(){}
		~Sentenizer(void){}
		bool init(const std::string& config_filename);
		int sentenize(const std::string& line, VecOfStr& res);

	private:
		iLines		m_cleanline;
		iLines		m_punctLine, m_digitLine;
		DictSet     m_segDict, m_unsegDict;
		MaxEntModel m_meModel;
		std::map<std::string, bool> m_featActive;

		int __getFeatTemplate(VecFeatTemplate& featTemplate, TibetSentParse& parser);
		bool __loadDictFile(DictSet& dict, const std::string& dictFile);
		bool __getFeatConf(const std::string& featConfigFile);
		int __parseDefinitSent(const std::string& line);
	};
}

#endif //_SENTENZIER_H_
