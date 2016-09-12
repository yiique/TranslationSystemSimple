#include "Utf8ZhHant2CN.h"

#include <cstring>
#include <string>
#include <iostream>
using namespace std;

//extern "C"{
//#include "cconv.h"
//}

using namespace utf8zhhant2cn;

string & utf8zhhant2cn::Utf8ZhHant2CN(string & src)
{
	/*size_t in_size = src.size() + 1;
	size_t out_size = in_size * 2;
	
	char * in_buf = new char[in_size];
	char * out_buf = new char[out_size];
	
	
	memset(in_buf, 0, in_size);
	memset(out_buf, 0, out_size);
	memcpy(in_buf, src.c_str(), src.size());

	cconv_t conv = cconv_open(CCONV_CODE_UCN, CCONV_CODE_UTF);
	if((cconv_t)-1 == conv)
	{
		return src;
	}
    
	char * p_in_buf = in_buf;
	char * p_out_buf = out_buf;
	size_t ret = cconv(conv, &p_in_buf, &in_size, &p_out_buf, &out_size);
	
	if((int)ret < 0)
	{
		return src;
	}

	out_buf[ret] = 0;

	cconv_close(conv);
	
	src = out_buf;

    delete [] in_buf;
    delete [] out_buf;*/
	
    return src;
}

