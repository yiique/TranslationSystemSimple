/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#ifndef _MAXENTMODEL_H_
#define _MAXENTMODEL_H_

#include "Common.h"
#include "File.h"
#include <queue>
#include <cmath>

namespace tibetsentenizer
{
	typedef std::vector<std::string> MaxEntFeatureVec;
	typedef std::vector<double>      MaxEntClassProbs;

	class MaxEntModel
	{
	public:

		enum ClassType {UNSEG = 0, SEG = 1};

		MaxEntModel(void){};
		~MaxEntModel(void);

		bool load(const std::string & maxEntModelFile);
		int get_MaxEnt_Prob(MaxEntClassProbs & class_probs, const MaxEntFeatureVec & feats);

	private:

		typedef std::vector<char *>                     MaxEntFeatureTable;
		typedef std::map<const char*, int, CharCmpLess> PredicateMap;
		typedef std::vector<ClassType>                  OutcomeMap;
		typedef std::vector<std::pair<int, int> >       Para;
		typedef std::vector<Para >                      Parameters;
		typedef std::vector<double>                     Theta;

		MaxEntFeatureTable  m_me_feat_table;
		PredicateMap        m_pred_map;
		OutcomeMap          m_outcome_map;
		Parameters          m_parameters;
		Theta               m_theta;
	};
}

#endif //_MAXENTMODEL_H_
