/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#include "MaxEntModel.h"
using namespace tibetsentenizer;

MaxEntModel::~MaxEntModel(void)
{	
	for (MaxEntFeatureTable::size_type i = 0 ; i < m_me_feat_table.size(); ++i)
	{
		delete (m_me_feat_table[i]);
	}

	m_me_feat_table.clear();
}

bool MaxEntModel::load(const std::string &maxEntModelFile)
{
	std::ifstream ifs;
	
	bool open_file = File::openInputFile(ifs, maxEntModelFile);
	if (!open_file)
	{
		return false;
	}

	size_t predCount		= 0;
	size_t outcomeCount     = 0;
	size_t parameterCount   = 0;
	size_t thetaIdx		    = 0;
	size_t thetaCount       = 0;

	std::string line;
	
	while (std::getline(ifs, line))
	{
		if (!(line.empty() || line[0] == '#'))
		{
			break;
		}
	}

	{
		predCount = std::atoi(line.c_str());

		for (size_t i = 0; i < predCount; ++i)
		{
			if (std::getline(ifs, line))
			{
				char * p = new char [line.size() + 1];
				std::strcpy(p, line.c_str()); 

				m_me_feat_table.push_back(p);
				m_pred_map.insert(PredicateMap::value_type(p,i));
			}
		}
	}

	{
		std::getline(ifs, line);
		outcomeCount = std::atoi(line.c_str());
		m_outcome_map.resize(outcomeCount);
		
		for (size_t i = 0; i < outcomeCount; ++i)
		{
			if (std::getline(ifs, line))
			{
				if (line == "1")
				{
					m_outcome_map[i] = SEG;
				}
				else if (line == "0")
				{
					m_outcome_map[i] = UNSEG;
				}
				else
				{
					throw "meet error class type: " + line;
				}
			}
		}
	}

	{
		parameterCount = (int)(m_pred_map.size());
		thetaIdx = 0;
		
		for (size_t i = 0; i < parameterCount; ++i)
		{
			if (std::getline(ifs, line))
			{
				std::istringstream iss(line);
				int lable = 0;
				iss >> lable; 
				m_parameters.push_back(Para());
				
				while (iss >> lable)
				{
					m_parameters[m_parameters.size() - 1].push_back(std::make_pair(lable, thetaIdx++));
				}
			}
		}
	}

	{
		std::getline(ifs, line);
		thetaCount = std::atoi(line.c_str());
		
		if ( thetaCount != thetaIdx )
		{
			throw "error in MaxEnt Model file, diff in thetaCount";
		}
		
		m_theta.resize(thetaCount);
		
		for (size_t i = 0; i < thetaCount; ++i)
		{
			if (std::getline(ifs, line))
			{
				m_theta[i] = std::atof(line.c_str());
			}
		}
	}

	ifs.close();
	return true;
}

int MaxEntModel::get_MaxEnt_Prob(MaxEntClassProbs &class_probs, const MaxEntFeatureVec &feats)
{
	class_probs.clear();
	class_probs.resize(m_outcome_map.size(), 0.0); 

	for (MaxEntFeatureVec::size_type i = 0; i < feats.size(); ++i) 
	{
		PredicateMap::iterator pos = m_pred_map.find(feats[i].c_str());
		
		if (m_pred_map.end() != pos)
		{
			const Para & m_para = m_parameters[pos->second];
			for(Para::size_type j = 0; j < m_para.size(); ++j)
			{
				class_probs[m_para[j].first] += m_theta[m_para[j].second];
			}
		}
		else
		{
		}
	}

	double sum = 0.0;
	for (MaxEntClassProbs::size_type i = 0; i < class_probs.size(); ++i) 
	{
		class_probs[i] = std::exp(class_probs[i]);
		sum += class_probs[i];
	}

	double max_prob = (std::numeric_limits<double>::min)();
	MaxEntClassProbs::size_type maxIdx = 0;
	
	for (MaxEntClassProbs::size_type i = 0; i < class_probs.size(); ++i) 
	{
		class_probs[i] /= sum;
		if (max_prob < class_probs[i])
		{
			max_prob = class_probs[i];
			maxIdx = i;
		}
	}

	return maxIdx;
}
