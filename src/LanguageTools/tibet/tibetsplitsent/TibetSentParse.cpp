/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#include "TibetSentParse.h"
using namespace tibetsentenizer;

int TibetSentParse::__parseSyllable(VecOfStr &vec, std::string syllable)
{
	if (syllable.empty())
	{
		return 0;
	}

	std::string word;
	std::string num;

	bool  arabNumFlag   =  false;
	bool  tibetNumFlag  =  false;
	bool  symbolFlag    =  false;
	bool  wordFlag      =  false;
	bool  alphaFlag     =  false;

	for (size_t i = 0; i < syllable.size(); ++i)
	{
		if (syllable[i] >= 33 && syllable[i] <= 136)
		{
			if (wordFlag)
			{
				vec.push_back(word);
				wordFlag = false;
			}

			if (isdigit(syllable[i]))
			{
				if ( alphaFlag)
				{
					vec.push_back("alpha");
					alphaFlag = false;
				}

				arabNumFlag = true;
			} 
			else if (ispunct(syllable[i]))
			{
				if (arabNumFlag)
				{
					vec.push_back("digit");
					arabNumFlag = false;
				}
				else if (alphaFlag)
				{
					vec.push_back("alpha");
					alphaFlag = false;
				}

				std::string asciiPunct;
				asciiPunct = syllable[i];

				if(m_sLeftPunct.find(asciiPunct) != NPOS)
				{
					vec.push_back("lp");
				}
				else if (m_sRightPunct.find(asciiPunct) != NPOS)
				{
					vec.push_back("rp");
				}
				else
				{
					vec.push_back(asciiPunct);
				}
			}
			else if (isalpha(syllable[i]))
			{	
				if (arabNumFlag)
				{
					vec.push_back("digit");
					arabNumFlag = false;
				}

				alphaFlag = true;
			}
		} 
		else
		{
			if (arabNumFlag)
			{
				vec.push_back("digit");
				arabNumFlag = false;
			}
			else if (alphaFlag)
			{
				vec.push_back("alpha");
				alphaFlag = false;
			}

			if (i < syllable.size() - 1)
			{
				const std::string tibetWord = syllable.substr(i, 3);
				iLines::iterator iterPunc  = find(m_sPunctLine.begin(), m_sPunctLine.end(), tibetWord); 
				iLines::iterator iterDigit = find(m_sDigitLine.begin(), m_sDigitLine.end(), tibetWord); 

				if (iterPunc != m_sPunctLine.end())
				{
					if (wordFlag)
					{
						vec.push_back(word);
						wordFlag = false;
					}
					else if(tibetNumFlag )
					{
						vec.push_back("digit");
						tibetNumFlag = false;
					}

					if(m_sLeftTiPunct.find(tibetWord) != m_sLeftTiPunct.end())      
					{
						vec.push_back("lp");
					}
					else if (m_sRightTiPunct.find(tibetWord) != m_sRightTiPunct.end())
					{
						vec.push_back("rp");
					}
					else
					{
						vec.push_back(tibetWord);
					}
				}
				else if (iterDigit != m_sDigitLine.end())
				{
					if (wordFlag)
					{
						vec.push_back(word);
						wordFlag = false;
					}
					else if (alphaFlag)
					{
						vec.push_back("alpha");
						alphaFlag = false;
					}

					tibetNumFlag = true;
				}
				else if (!wordFlag)
				{
					if (tibetNumFlag)
					{
						vec.push_back("digit");
						tibetNumFlag = false;
					}
					else if (alphaFlag)
					{
						vec.push_back("alpha");
						alphaFlag  = false;
					}
					wordFlag = true;
					word.clear();
					word += tibetWord;
				}
				else
				{
					word += tibetWord;
				}

				++i;
			}	
		}
	}

	if (wordFlag)
	{
		vec.push_back(word);
	}

	return 1 ;
}

int TibetSentParse::__split(VecOfStr & vec, std::string sent, std::string key, int type)
{
	if (sent.empty())
	{
		return 0;
	}

	size_t  beginPos = 0, endPos = 0;

	while((beginPos = sent.find(key, endPos)) != NPOS)
	{
		if (beginPos != endPos)	 
		{
			std::string temp = sent.substr(endPos, beginPos - endPos);
			vec.push_back(temp);

			if (temp != m_sCloudSign && type == SENTSPLIT)
			{
				vec.push_back(m_sSentSegSign);
			}
		}
		else
		{	
			if (beginPos >= 6 && sent.substr(beginPos - 6, 6) == m_sCloudSign)
			{
				vec.clear();
				vec.push_back(m_sCloudSign);
				vec.push_back(m_sSentSegSign);
			} 	
		}

		endPos = beginPos + key.size();
	}

	if ( endPos != sent.size() )
	{
		std::string str = sent.substr( endPos );
		vec.push_back( str );

		if ( type == SENTSPLIT )
		{
			vec.push_back( m_sSentSegSign );
		}
	}

	return 1;
}

std::string TibetSentParse::__replace(std::string str)
{
	size_t spaceFirst, spaceLast = 0;
	std::string temp;

	while((spaceFirst = str.find_first_of(" ", spaceLast)) != NPOS)
	{
		temp += str.substr(spaceLast , spaceFirst - spaceLast);

		spaceLast = str.find_first_not_of(" ", spaceFirst);

		if(spaceFirst >= 3 && (str.substr(spaceFirst - 3, 3) == getSign1() || str.substr(spaceFirst - 3, 3) == getSign2()) && spaceLast  <= str.size() - 3 && str.substr(spaceLast, 3) != getCharSign())
		{
			temp += m_sSentSegSign;	
		}
	}

	if( spaceFirst == NPOS )
	{
		temp += str.substr(spaceLast);
	}

	return temp;
}


int TibetSentParse::__token(VecOfVecOfWordProperty &wordVec, std::string sent)
{
	std::string trimSent = sent;

	if(trimSent.size() >= 3 && trimSent.substr(trimSent.size() - 3, 3) != m_sSentSegSign)
	{
		trimSent += m_sSentSegSign;
	}

	if(trimSent.find(" ") != NPOS)
	{
		trimSent = __replace(trimSent);
	}

	VecOfStr sentSeg__splitedVec;	
	__split(sentSeg__splitedVec, trimSent, m_sSentSegSign, SENTSPLIT);

	VecOfWordProperty wVec;

	for (size_t i = 0; i < sentSeg__splitedVec.size(); ++i)
	{
		if (sentSeg__splitedVec[i].find(m_sSentSegSign) == NPOS)  
		{
			VecOfStr charSeg__splitedVec;
			__split(charSeg__splitedVec, sentSeg__splitedVec[i], m_sCharSegSign, SYLLSPLIT);

			for (size_t j = 0; j < charSeg__splitedVec.size(); ++j)
			{
				if (!charSeg__splitedVec[j].empty())
				{
					VecOfStr vecSyll;

					__parseSyllable (vecSyll, charSeg__splitedVec[j]);

					for (size_t k = 0; k < vecSyll.size(); ++k)
					{
						wVec.push_back(std::make_pair<std::string, int>(trim(vecSyll[k]), WORDSYSMBOL));
					}
				}
			}
		}
		else if(sentSeg__splitedVec[i] != m_sCloudSign) 
		{
			if (i != sentSeg__splitedVec.size() - 1)
			{
				wVec.push_back(std::make_pair<std::string, int>(sentSeg__splitedVec[i], UNSEGSYSMBOL));
			}
			else
			{
				wVec.push_back(std::make_pair<std::string, int>(sentSeg__splitedVec[i], SEGSYSMBOL));
			}
		}
		else
		{
			wVec.push_back( std::make_pair<std::string, int>(m_sCloudSign, WORDSYSMBOL));
		}
	}

	if (!sentSeg__splitedVec.empty())
	{
		wordVec.push_back(wVec);
	}

	return 1;
}

int TibetSentParse::sentParse(VecOfVecOfWordProperty &wordVec, iLines &copurs)
{
	for (size_t i = 0; i < copurs.size(); ++i)
	{
		__token(wordVec, copurs[i]);
	}

	return 1;
}