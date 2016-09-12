#include "ByteBuffer.h"

ByteBuffer::ByteBuffer(void): mp_data(NULL), m_length(0)
{
}

ByteBuffer::~ByteBuffer(void)
{
	if(mp_data)
		delete [] mp_data;
}

void ByteBuffer::Resize(const size_t length)
{
	if(length == 0 )
	{
		if(mp_data) 
			delete [] mp_data;

		mp_data = NULL;
		m_length = 0;

		return;
	}

	size_t copy_length = (m_length < length) ? m_length : length;

	char * p_new = new char[length];
	memcpy(p_new, mp_data, copy_length);

	if(mp_data)
		delete [] mp_data;
	mp_data = p_new;
	m_length = length;

	return;
}

string & ByteBuffer::String(string & str) const
{
	return str.assign(mp_data, m_length);
}

string ByteBuffer::String() const
{
	string str(mp_data, m_length);
	return str;
}
