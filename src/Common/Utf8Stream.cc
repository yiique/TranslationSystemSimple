#include "Utf8Stream.h"


Utf8Stream::Utf8Stream(const string & utf8_src): m_utf8_src(utf8_src),
												 m_byte_idx(0)
{
}

Utf8Stream::~Utf8Stream(void)
{
}


bool Utf8Stream::ReadOneCharacter(string & utf8_char_str)
{
	// return true ��ʾ��ȡ�ɹ���
	// return false ��ʾ�Ѿ���ȡ����ĩβ

	if(m_utf8_src.size() == m_byte_idx)
		return false;

	size_t utf8_char_len = get_utf8_char_len(m_utf8_src[m_byte_idx]);

	if( 0 == utf8_char_len )
	{
		cerr << "Get a illegal utf-8 character." << endl;
		utf8_char_str = m_utf8_src[m_byte_idx++];
		return true;
	}

	size_t next_idx = m_byte_idx + utf8_char_len;
	if( m_utf8_src.size() < next_idx )
	{
		cerr << "Get utf8 first byte out of input src string." << endl;
		next_idx = m_utf8_src.size();
	}

	//���UTF-8��һ���ַ�
	utf8_char_str = m_utf8_src.substr(m_byte_idx, next_idx - m_byte_idx);

	//����ƫ����
	m_byte_idx = next_idx;

	return true;
}


size_t Utf8Stream::get_utf8_char_len(char & byte)
{
	// return 0 ��ʾ����
	// return 1-6 ��ʾ��ȷֵ
	// ���� return ����ֵ 

	//UTF8 �����ʽ��
	//     U-00000000 - U-0000007F: 0xxxxxxx  
    //     U-00000080 - U-000007FF: 110xxxxx 10xxxxxx  
    //     U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx  
    //     U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
    //     U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
    //     U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  

	size_t len = 0;
	unsigned char mask = 0x80;

	while( byte & mask )
	{
		len++;
		if( len > 6 )
		{
			cerr << "The mask get len is over 6." << endl;
			return 0;
		}

		mask >>= 1;
	}

	if( 0 == len )
	{
		return 1;
	}

	return len;

}

