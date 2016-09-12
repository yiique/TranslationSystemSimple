#include "TextFormat.h"
#include "Common/f_utility.h"
#include <sstream>

void TextFormat::Clear()
{
	for(size_t i=0; i<m_para_vec.size(); ++i)
	{
		delete m_para_vec[i];
	}

	m_para_vec.clear();
}

void TextFormat::Reset(const vector<string> & para_vec, 
					   const vector<pair<size_t, size_t> > & pos_vec)
{
	assert( para_vec.size() == pos_vec.size() );
	
	vector<Para*> tmp_para_vec;

	for(size_t i=0; i<para_vec.size(); ++i)
	{
		Para * pr = new Para;
		pr->_content = para_vec[i];
		pr->_offset = pos_vec[i].first;
		pr->_len = pos_vec[i].second;

		tmp_para_vec.push_back(pr);
	}

	if(false == check_format_info(tmp_para_vec))
	{
		lerr << "Check format failed. m_para_vec will be zero." << endl;
		return;
	}

	this->Clear();
	m_para_vec = tmp_para_vec;
}

bool TextFormat::check_format_info(vector<Para*> & para_vec)
{
	//验证pos是否冲突，并对其按照offset排序

	//Step.1 先排序
	sort(para_vec.begin(), para_vec.end(), ParaInAhead());

	//debug print
	//for(size_t i=0; i<para_vec.size(); ++i)
	//{
	//	lout << "para_vec.content = [" << para_vec[i]->_content << "]" << endl;
	//	lout << "para_vec.pos = [" << para_vec[i]->_offset << " , " << para_vec[i]->_len << "]" << endl;
	//}

	//Step.2 验证
	size_t ahead_end = 0;
	for(size_t i=0; i<para_vec.size(); ++i)
	{
		if(para_vec[i]->_offset < ahead_end)
		{
			lerr << " para_vec: i = " << i << " _offset = " << para_vec[i]->_offset << " ahead_end = " << ahead_end << endl;
			return false;
		}
		else
			ahead_end = para_vec[i]->_offset + para_vec[i]->_len;
	}

	return true;
}

void TextFormat::Serialization(string & data) const 
{
	stringstream ss;

	for(size_t i=0; i<m_para_vec.size(); ++i)
	{
		ss << m_para_vec[i]->_offset << " " << m_para_vec[i]->_len << endl;
	}

	data = ss.str();
}

bool TextFormat::UnSerialization(const string & data)
{
	this->Clear();

	stringstream ss;
	ss << data;

	string line;
	while(getline(ss, line))
	{
		vector<string> pos_vec;
		split_string_by_blank(line.c_str(), pos_vec);

		if(pos_vec.size() < 2)
			return false;

		Para * p_para = new Para();

		p_para->_offset = str_2_num(pos_vec[0]);
		p_para->_len = str_2_num(pos_vec[1]);

		m_para_vec.push_back(p_para);
	}

	return check_format_info(m_para_vec);
}

