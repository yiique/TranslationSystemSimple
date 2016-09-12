#include "CSequence.h"

CSequence & CSequence::operator << (const int val)
{

	m_data += int_tag;

	m_data += num_2_str(val);

	return *this;
}

CSequence & CSequence::operator >> (int & val)
{

	val = 0;

	if(next_seq_tag() != int_tag)
		return *this;

	string s;

	if(0 == get_int_segment(s))
	{
		//cerr << "get int seg = 0" << endl;
		return *this;
	}

	//cout << "get s = [" << s << "]" << endl;
	val = str_2_num(s);

	return *this;
}


CSequence & CSequence::operator << (const string & val)
{
	
	m_data += str_tag;

	m_data += num_2_str(val.size());

	m_data += str_tag;

	m_data += val;
	return *this;
}

CSequence & CSequence::operator >> (bool & val)
{
	int i_val;
	(*this) >> i_val;

	//cout << "i_val = " << i_val << endl;
	val = (bool) i_val;

	return *this;
}

CSequence & CSequence::operator >> (size_t & val)
{
	int i_val;
	(*this) >> i_val;

	val = (size_t) i_val;
	return *this;
}

CSequence & CSequence::operator >> (string & val)
{
	
	val.clear();

	if(next_seq_tag() != str_tag)
		return *this;

	string size_val;
	const size_t size_offset = get_int_segment(size_val);
	if(size_offset == 0)
		return *this;

	size_t str_len = str_2_num(size_val);

	if(0 == get_str_segment(val, str_len) )
	{
		m_out_idx -= size_offset;
		return *this;
	}

	return *this;
}

const char CSequence::next_seq_tag()
{
	if(m_data.size() <= m_out_idx)
		return err_tag;

	const char curr_tag = m_data[m_out_idx];

	if(curr_tag == int_tag
		|| curr_tag == str_tag)
		return curr_tag;
	else
		return err_tag;
}

size_t CSequence::get_int_segment(string & val)
{
	const size_t old_idx = m_out_idx;
	bool check = false;

	//cout << "out_idx = " << m_out_idx << endl;
	//cout << "curr val = " << m_data[m_out_idx] << endl;
	//跳过自己的tag
	++m_out_idx;

	while(m_out_idx < m_data.size())
	{
		if(m_data[m_out_idx] == int_tag || m_data[m_out_idx] == str_tag)
		{
			check = true;
			break;
		}

		++m_out_idx;
	}

	if(true == check || m_out_idx > old_idx+1)
	{
		val = m_data.substr(old_idx+1, m_out_idx - old_idx - 1); 
	}else
	{
		
		m_out_idx = old_idx;
	}

	return m_out_idx - old_idx;
}

size_t CSequence::get_str_segment(string & val, const size_t len)
{

	const size_t old_idx = m_out_idx;

	//跳过自己的tag和len
	m_out_idx += len + 1;

	if(m_out_idx <= m_data.size())
	{
		val = m_data.substr(old_idx+1, m_out_idx - old_idx - 1); 
	}else
	{
		m_out_idx = old_idx;
	}

	return m_out_idx - old_idx;
}

