#ifndef CSEQUENCE_H
#define CSEQUENCE_H

#include "f_utility.h"

#include <iostream>
#include <string>
using namespace std;


class CSequence
{
public:
	CSequence(void) : m_out_idx(0) {};
	CSequence(const string & src) : m_data(src), m_out_idx(0) {};
	~CSequence(void) {};

	//标识位 \r
	CSequence & operator << (const int val);
	CSequence & operator >> (int & val);
	CSequence & operator >> (bool & val);
	CSequence & operator >> (size_t & val);

	//标识位 \t
	CSequence & operator << (const string & val);
	CSequence & operator >> (string & val);
	


	void ResetOutIdx()
	{
		m_out_idx = 0;
	};

	string & GetData(string & data)
	{
		data = m_data;
		return data;
	};

	const string & GetData() const
	{
		return m_data;
	}

	const char * GetCStrData() const
	{
		return m_data.c_str();
	}

private:

	string m_data;

	size_t m_out_idx;

	static const char int_tag = 'I';
	static const char str_tag = 'S';
	static const char err_tag = 'E';

	size_t get_int_segment(string & val);
	size_t get_str_segment(string & val, const size_t len);
	const char next_seq_tag();
};

#endif //CSEQUENCE_H
