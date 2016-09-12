/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#include "Sentenizer.h"
using namespace tibetsentenizer;

bool Sentenizer::init(const std::string& config_filename)
{
	setSign();
	iLines config;
	if (!config.__readFrom(config_filename))
	{
		std::cout << "Init Tibet Sentenizer Fails!" << std::endl;
		return false;
	}	
	if (!m_punctLine.__readFrom(config[0]))
	{
		std::cout << "Init Tibet Sentenizer Fails!" << std::endl;
		return false;
	}	
	if (!m_digitLine.__readFrom(config[1]))
	{
		std::cout << "Init Tibet Sentenizer Fails!" << std::endl;
		return false;
	}
	if (!__getFeatConf(config[2]))
	{
		std::cout << "Init Tibet Sentenizer Fails!" << std::endl;
		return false;
	}
	if (!m_meModel.load(config[3]))
	{
		std::cout << "Init Tibet Sentenizer Fails!" << std::endl;
		return false;
	}
	if (!__loadDictFile(m_unsegDict, config[4]))
	{
		std::cout << "Init Tibet Sentenizer Fails!" << std::endl;
		return false;
	}
	if (!__loadDictFile(m_segDict, config[5]))
	{
		std::cout << "Init Tibet Sentenizer Fails!" << std::endl;
		return false;
	}

	return true;
}

bool Sentenizer::__loadDictFile(DictSet& dict, const std::string& dictFile)
{
	std::ifstream ifs;
	if (!File::openInputFile(ifs, dictFile))
	{
		std::cout << "Can't load " << dictFile << " dictionary!" << std::endl;
		return false;
	}

	std::string dictLine;
	std::string sent_sign = getSentSign();

	while (getline(ifs, dictLine))
	{
		if (!dictLine.empty())
		{
			if (dictLine.find(sent_sign) == NPOS)
			{
				dictLine += sent_sign;
			}
			dict.insert(dictLine);
		}
	}

	ifs.close();

	return true;
}

bool Sentenizer::__getFeatConf(const std::string& featConfigFile)
{
	std::ifstream ifs;
	bool open_file = File::openInputFile(ifs, featConfigFile);
	
	if (!open_file)
	{
		std::cout << "Can't get feature configuration!" << std::endl;
		return false;
	}

	std::string featConfig;

	while (getline(ifs, featConfig))
	{
		if(!featConfig.empty())
		{
			std::istringstream iss(featConfig);
			std::string feat;
			bool isActive;

			iss >> feat;
			iss >> isActive;
			
			m_featActive[feat] = isActive;
		}
	}

	ifs.close();

	return true;
}

int Sentenizer::__getFeatTemplate(VecFeatTemplate& featTemplate, TibetSentParse& parser)
{
	VecOfVecOfWordProperty wordVec;
	parser.sentParse(wordVec, m_cleanline);
	
	CreatFeat feat(m_featActive);
	feat.getFeat(wordVec, featTemplate);

	return 0;
}

int Sentenizer::__parseDefinitSent(const std::string& line)
{
	const std::string& sentSegSign       = getSentSign();
	const std::string& doubleSentSegSign = sentSegSign + sentSegSign;
	const std::string& cloudSign         = getCloudSign();
	const std::string& sign1             = getSign1();
	const std::string& sign2             = getSign2();
	
	std::string sent = trim(line);
	//10.27 add
	int begin = 0, end;
	int signcount = 0;
	std::map<int, bool> sign1_pos, sign2_pos;

	while ((end = sent.find(sign1, begin)) != NPOS)
	{
		if (end < sent.size() - sign1.size() && sent[end + sign1.size()] == ' ')
		{
			sign1_pos[signcount++] = true;
		}
		else
		{
			sign1_pos[signcount++] = false;
		}
		begin = end + sign1.size();
	}
	begin = 0;
	signcount = 0;
	while ((end = sent.find(sign2, begin)) != NPOS)
	{
		if (end < sent.size() - sign2.size() && sent[end + sign2.size()] == ' ')
		{
			sign2_pos[signcount++] = true;
		}
		else
		{
			sign2_pos[signcount++] = false;
		}
		begin = end + sign2.size();
	}
	
	std::string temp;
	for (size_t i = 0; i < sent.size(); ++i)
	{
		if (sent[i] != ' ')
		{
			temp += sent[i];
		}
	}
	sent = temp;
	temp = "";
	begin = 0;
	signcount = 0;
	while ((end = sent.find(sign1, begin)) != NPOS)
	{
		temp += sent.substr(begin, end - begin + sign1.size());
		if (sign1_pos[signcount])
		{
			temp += " ";
		}
		++signcount;
		begin = end + sign1.size();
	}
	if (begin < sent.size())
	{
		temp += sent.substr(begin);
	}
	sent = temp;
	temp = "";
	begin = 0;
	signcount = 0;
	while ((end = sent.find(sign2, begin)) != NPOS)
	{
		temp += sent.substr(begin, end - begin + sign2.size());
		if (sign2_pos[signcount])
		{
			temp += " ";
		}
		++signcount;
		begin = end + sign2.size();
	}
	if (begin < sent.size())
	{
		temp += sent.substr(begin);
	}
	sent = temp;
	//10.27 add
	begin = 0;
	bool cloud = false;
	m_cleanline.clear();

	while((end = sent.find(doubleSentSegSign, begin)) != NPOS)
	{
		std::string str = sent.substr(begin, end + doubleSentSegSign.size() - begin);
		size_t cloudPos = str.rfind(cloudSign);
		
		if(cloudPos == NPOS)
		{
			if(cloud == true)
			{
				m_cleanline.push_back(cloudSign + sentSegSign + str);
				cloud = false;
			}
			else
			{
				m_cleanline.push_back(str);
			}
		}
		else
		{
			if(cloudPos != 0)
			{
				m_cleanline.push_back(str.substr(0, cloudPos));
			}
			cloud = true;
		}

		begin = end + doubleSentSegSign.size();
	}

	if(begin != sent.size())
	{
		if(cloud == true)
		{
			m_cleanline.push_back(cloudSign + sentSegSign + sent.substr(begin));
		}
		else
		{
			m_cleanline.push_back(sent.substr(begin));
		}
	}

	if(m_cleanline.size() == 0)
	{
		m_cleanline.push_back(sent);
	}

	return 0;
}

int Sentenizer::sentenize(const std::string& line, VecOfStr &res)
{	
	__parseDefinitSent(line);
	TibetSentParse parser(m_punctLine, m_digitLine);

	VecFeatTemplate featTemplate;
	__getFeatTemplate(featTemplate, parser);

	Segmenter segmenter(&parser, &m_meModel);
	iLines segedSent;
	segmenter.segment(m_cleanline, featTemplate, segedSent, m_unsegDict, m_segDict);
	
	for (size_t i = 0; i < segedSent.size(); ++i)
	{
		res.push_back(segedSent[i]);
	}

	return 0;
}
