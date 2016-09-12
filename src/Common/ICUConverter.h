#ifndef ICU_CONVERTER_H
#define ICU_CONVERTER_H

//ICU
#include <unicode/ucsdet.h>
#include <unicode/ucnv.h>

//SYS
#include <assert.h>

//MAIN
#include "Log/LogStream.h"

//STL
#include <iostream>
#include <string>
using namespace std;

#define BELIEVABLE_DETECT 70
#define DEF_BUF_RATIO     2

class ICUConverter
{
public:
	ICUConverter(void);
	~ICUConverter(void);

	static int Detect(const char * src, size_t size, string & charset);
	static int Detect(const string & src, string & charset);

	static bool Convert( const string & src_charset,
						   const char * src, 
						   size_t size,
						   const string & tgt_charset,
						   string & tgt); 
	static bool Convert( const string & src_charset,
						   const string & src, 
						   const string & tgt_charset,
						   string & tgt);
	static void PrintAllCharset();

	static string & Convert( const string & src_charset,
						   const string & tgt_charset,
						   string & src);

};



#endif //ICU_CONVERTOR_H

