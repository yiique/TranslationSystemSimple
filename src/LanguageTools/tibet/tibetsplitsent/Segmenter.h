/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#ifndef _SEGMENTER_H_
#define _SEGMENTER_H_

#include "TibetSentParse.h"
#include "MaxEntModel.h"

namespace tibetsentenizer
{
	class Segmenter
	{
	public:

		Segmenter(TibetSentParse* parser, MaxEntModel* pMEModel):charSegSign(getCharSign()), sentSegSign(getSentSign())
		{
			m_parser   = parser;
			m_pMEModel = pMEModel;
		}

		~Segmenter(){}

		int segment(const iLines& testSent, VecFeatTemplate& vecFeatTemplate, iLines& segedSent, DictSet& unsegDict, DictSet& segDict);

	private:

		int __useDict(const std::string& sent, const DictSet& dict, iLines& featTemplate, VecOfInt& segOrUnsegPos, std::map<size_t, size_t > &sentSegPos);
		int __useME(const std::string& sent, iLines &featTemplate, VecOfInt &segPos, const VecOfInt &unsegPos, const std::map<size_t, size_t> &sentSegPos);
		int __clearFeat(iLines &featTemplate, std::map<size_t, size_t > &sentSegPos, size_t pos);
		int __settlePlace(std::string str, VecOfInt &place);

		TibetSentParse *m_parser;
		MaxEntModel    *m_pMEModel;
		const std::string&  sentSegSign;
		const std::string&  charSegSign;
	};

}

#endif //_SEGMENTER_H_
