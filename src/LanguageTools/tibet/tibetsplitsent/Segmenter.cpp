/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#include "Segmenter.h"
using namespace tibetsentenizer;

int Segmenter::__clearFeat(iLines& featTemplate, std::map<size_t, size_t>& sentSegPos, size_t pos)
{
	size_t deleteFeatPos = sentSegPos[pos];
	
	sentSegPos[pos] = -1;
	featTemplate[deleteFeatPos].clear();

	return 0;
}

int Segmenter::__useDict(const std::string& sent, const DictSet& dict, iLines &featTemplate, VecOfInt &segOrUnsegPos, std::map<size_t, size_t> &sentSegPos)
{
	for(DictSet::const_iterator iter = dict.begin(); iter != dict.end(); ++iter)
	{
		size_t begin = 0, end;

		while((end = sent.find(*iter, begin)) != NPOS)
		{
			size_t nextPos = end + (*iter).size();
			begin = nextPos;

			if(nextPos <= sent.size() - 3 && sent.substr(nextPos, 3) == charSegSign)
			{
				continue;	
			}

			segOrUnsegPos.push_back(nextPos);

			if((*iter).rfind(sentSegSign) != NPOS)
			{
				__clearFeat(featTemplate, sentSegPos, nextPos - 3);
			}
		}
	}

	return 0;
}

int Segmenter::__useME(const std::string& sent, iLines &featTemplate, VecOfInt &segPos, const VecOfInt &unsegPos, const std::map<size_t, size_t> &sentSegPos)
{
	size_t featCounter = 0;

	for(std::map<size_t, size_t>::const_iterator iter = sentSegPos.begin(); iter != sentSegPos.end(); ++iter)
	{
		VecOfInt::const_iterator unsegIter = find(unsegPos.begin(), unsegPos.end(), iter->first + sentSegSign.size());
		VecOfInt::const_iterator segIter   = find(segPos.begin(), segPos.end(), iter->first + sentSegSign.size());

		if(unsegIter == unsegPos.end() && segIter == segPos.end())
		{
			if(iter->first >= sent.size() - 6)
			{
				segPos.push_back(sent.size());
				break;
			}

			std::string feature = featTemplate[featCounter++];

			std::istringstream iss(feature);
			std::string feat;

			MaxEntFeatureVec featVect;

			while (iss >> feat)
			{
				featVect.push_back(feat);
			}

			MaxEntClassProbs classprobs;

			m_pMEModel->get_MaxEnt_Prob(classprobs, featVect);

			if (classprobs[MaxEntModel::SEG] >=  classprobs[MaxEntModel::UNSEG])
			{
				segPos.push_back(iter->first + sentSegSign.size());
			}
		}
	}

	return 0;
}

int Segmenter::__settlePlace(std::string str, VecOfInt &insertPlace)
{
	size_t spaceFirst, spaceLast = 0;
	std::string tempStr;

	while((spaceFirst = str.find_first_of(" ", spaceLast)) != NPOS)
	{
		tempStr += str.substr(spaceLast , spaceFirst - spaceLast);
		spaceLast = str.find_first_not_of(" ", spaceFirst);

		if(spaceFirst >= 3 && (str.substr(spaceFirst - 3, 3) == getSign1() || str.substr(spaceFirst - 3, 3) == getSign2()) && spaceLast <= str.size() - 3 && str.substr(spaceLast, 3) != sentSegSign)
		{
			tempStr += sentSegSign + " ";

			insertPlace.push_back(tempStr.size() - 1);
		}
		else
		{
			tempStr += " "; 
		}
	}

	return 0;
}

int Segmenter::segment(const iLines &testSent, VecFeatTemplate &vecFeatTemplate, iLines &segedSent, DictSet &unsegDict, DictSet &segDict)
{
	for (size_t i = 0; i < testSent.size(); ++i)
	{
		if(testSent[i].size() <= 15)
		{
			segedSent.push_back(testSent[i]);
			continue;
		}

		VecOfInt insertPlace;               

		std::string sent = testSent[i];

		if(sent.find(" ") != NPOS)
		{
			__settlePlace(sent, insertPlace);
		}

		size_t begin = 0, end;  
		if (sent.substr(0, 6) == getCloudSign())
		{
			begin += 12;
		}

		bool findTailSentSign = false;
		if (sent.substr(sent.size() - 6) == sentSegSign + sentSegSign)
		{
			findTailSentSign = true;
			sent = sent.substr(0, sent.size() - 3);
		}

		size_t featSize = 0;
		std::map<size_t, size_t> sentSegPos; 
		while((end = sent.find(sentSegSign, begin)) != NPOS)
		{
			sentSegPos[end] = featSize++;			
			begin = end + 2;
		}

		if(sentSegPos.size() == 0 || (sentSegPos.size() == 1 && sentSegPos.begin()->second == sent.size() - 2))
		{
			segedSent.push_back(sent + sentSegSign);
			continue;
		}

		iLines featTemplate = vecFeatTemplate[i];
		VecOfInt segPos, unsegPos;           
		__useDict(sent, unsegDict, featTemplate, unsegPos, sentSegPos);
		__useDict(sent, segDict,   featTemplate, segPos,   sentSegPos);

		for(std::map<size_t, size_t >::iterator iter = sentSegPos.begin(); iter != sentSegPos.end();)
		{
			if(iter->second == -1 )
			{
				sentSegPos.erase(iter++);
			}
			else
			{
				++iter;
			}
		}

		featTemplate.erase(remove(featTemplate.begin(), featTemplate.end(), ""), featTemplate.end());

		if(featTemplate.size() != 0)
		{
			__useME(sent, featTemplate, segPos, unsegPos, sentSegPos);
		}

		for(size_t i = 0; i < insertPlace.size(); ++i)
		{
			sent.replace(insertPlace[i] - 3, 3, "   ");
		}

		{
			if(segPos.size() == 0)
			{
				segedSent.push_back(sent + sentSegSign);
				continue;
			}

			std::string asciiPunctLine = "}]>);,.?!";
			std::string punctLine      = "¡±¡·£©¡µ¡³¡¿¡¯£ý£¬¡££¿£¡£»";
			std::string temp;
			begin = 0;
			sort(segPos.begin(), segPos.end());

			for(VecOfInt::iterator iter = segPos.begin(); iter != segPos.end(); ++iter)
			{
				if (*iter < sent.size() && asciiPunctLine.find(sent.substr(*iter, 1)) != NPOS)
				{
					if(*iter < sent.size() - 3 && sent.substr(*iter + 1, 3) == sentSegSign)
					{
						temp = trim(sent.substr(begin, *iter - begin + 4));
						segedSent.push_back(temp);
						begin = *iter + 4;
					}
					else
					{
						temp = trim(sent.substr(begin, *iter - begin + 1));
						segedSent.push_back(temp);
						begin = *iter + 1;
					}
				}
				else if (*iter < sent.size() - 1 && punctLine.find(sent.substr(*iter, 2)) != NPOS)
				{
					if(*iter < sent.size() - 3 && sent.substr(*iter + 2, 3) == sentSegSign)
					{
						temp = trim(sent.substr(begin, *iter - begin + 5));
						segedSent.push_back(temp);
						begin = *iter + 5;
					}
					else
					{
						temp = trim(sent.substr(begin, *iter - begin + 2));
						segedSent.push_back(temp);
						begin = *iter + 2;
					}
				}
				else
				{
					temp = trim(sent.substr(begin, *iter - begin));
					segedSent.push_back(temp);
					begin = *iter;
				}	
			}

			if(*(segPos.end() - 1) <= sent.size() - 6)
			{
				temp = trim(sent.substr(*(segPos.end() - 1)));
				segedSent.push_back(temp);
			}

			if (findTailSentSign)
			{
				*(segedSent.end() - 1) += sentSegSign;
			}
		}
	}

	return 0;
}
