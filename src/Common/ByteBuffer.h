#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <string>
using namespace std;

class ByteBuffer
{
public:
	ByteBuffer(void);
	~ByteBuffer(void);


	void Resize(const size_t length);
	string & String(string & str) const;
	string String() const;
	char * GetPtr() { return mp_data; }
	size_t GetLength() { return m_length; }


private:
	char * mp_data;
	size_t m_length;
};



#endif //BYTE_BUFFER_H
