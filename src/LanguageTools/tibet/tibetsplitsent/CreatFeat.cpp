/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#include "CreatFeat.h"
using namespace tibetsentenizer;

bool CreatFeat::isActive(const std::string& feat)
{
	if(m_featActive.find(feat) != m_featActive.end())
	{
		return true;
	}

	return false;
}

void CreatFeat::getFeat(const VecOfVecOfWordProperty& wordVec, VecFeatTemplate& featTemplate)
{	
	for(VecOfVecOfWordPropertyConstIter wpIter = wordVec.begin(); wpIter != wordVec.end(); ++wpIter)
	{
		const VecOfWordProperty &wordSegVector = *wpIter;

		iLines iFeatTemplate;

		size_t wordSegSize = wordSegVector.size();

		for (size_t i = 0; i < wordSegSize; ++i)
		{
			if (wordSegVector[i].second == WORDSYSMBOL)
			{
				continue;
			}

			std::string feat;

			if(isActive("L2L1"))
			{
				feat += "L2L1=" + wordSegVector[(i - 2 + wordSegSize) % wordSegSize].first + "-" + 
					              wordSegVector[(i - 1 + wordSegSize) % wordSegSize].first + " ";
			}
			if(isActive("L2"))
			{
				feat += "L2=" + wordSegVector[(i - 2 + wordSegSize) % wordSegSize].first + " ";
			}
			if(isActive("L1Syl"))
			{
				std::string word = wordSegVector[(i - 1 + wordSegSize) % wordSegSize].first;
				Utf8Stream u8_stream(word);
				std::vector<std::string> v_u8_char;
				std::string u8_char;

				while (u8_stream.ReadOneCharacter(u8_char))
				{
					v_u8_char.push_back(u8_char);
				}

				std::string syl = "";
				
				if (v_u8_char.size() != 0)
				{
					syl = v_u8_char[v_u8_char.size() - 1];
				}

				feat += "L1Syl=" + syl + " ";
			}
			if(isActive("L1"))
			{
				feat += "L1=" + wordSegVector[(i - 1 + wordSegSize) % wordSegSize].first + " ";
			}	
			if(isActive("LLen"))
			{
				feat += "LLen=" + num2str(wordSegVector[(i - 1 + wordSegSize) % wordSegSize].first.size()) + " ";
			}
			if( isActive("R1"))
			{
				feat += "R1=" + wordSegVector[(i + 1) % wordSegSize].first + " ";
			}
			if(isActive("RLen"))
			{	
				feat += "RLen=" + num2str(wordSegVector[(i + 1) % wordSegSize].first.size()) + " ";
			}
			if(isActive("R2"))
			{
				feat += "R2=" + wordSegVector[(i + 2) % wordSegSize].first + " ";
			}
			if(isActive("R1R2"))
			{
				feat += "R1R2=" + wordSegVector[(i + 1 ) % wordSegSize].first + "-" +
					              wordSegVector[(i + 2 ) % wordSegSize].first;
			}

			iFeatTemplate.push_back(feat);	
		}

		featTemplate.push_back(iFeatTemplate);
	}
}
