#include "ICUConverter.h"


ICUConverter::ICUConverter(void)
{
}


ICUConverter::~ICUConverter(void)
{
}



int ICUConverter::Detect(const char * src, size_t size, string & charset)
{
	if(!src) return 0;

	UErrorCode status = U_ZERO_ERROR;

	UCharsetDetector * p_csd = ucsdet_open(&status);

	if(!p_csd)
	{
		cerr << "ICUConverter::Detect() open Detector failed. err = " << status << endl;
		return 0;
	}

	ucsdet_setText(p_csd, src, size, &status);

	if(U_ZERO_ERROR != status)
	{
		cerr << "ICUConverter::Detect() set src text error. err = " << status << endl;
		ucsdet_close(p_csd);
		return 0;
	}

	const UCharsetMatch * p_ucm = ucsdet_detect(p_csd, &status); //The returned UCharsetMatch object is owned by the UCharsetDetector.

	if(U_ZERO_ERROR != status)
	{
		cerr << "ICUConverter::Detect() detect error. err = " << status << endl;
		ucsdet_close(p_csd);
		return 0;
	}

	charset = ucsdet_getName(p_ucm, &status);

	if(U_ZERO_ERROR != status)
	{
		cerr << "ICUConverter::Detect() get charset name error. err = " << status << endl;
		ucsdet_close(p_csd);
		return 0;
	}

	int score = ucsdet_getConfidence(p_ucm, &status);

	if(U_ZERO_ERROR != status)
	{
		cerr << "ICUConverter::Detect() get detect score error. err = " << status << endl;
		ucsdet_close(p_csd);
		return 0;
	}


	//清理资源
	ucsdet_close(p_csd);

	return score;


}

int ICUConverter::Detect(const string & src, string & charset)
{
	return Detect(src.c_str(), src.size(), charset);
}
	

bool ICUConverter::Convert( const string & src_charset,
						   const char * src, 
						   size_t size,
						   const string & tgt_charset,
						   string & tgt)
{
	if(!src) return false;

	if(src_charset == tgt_charset)
	{
		tgt = src;
		return true;
	}

	bool res = true;
	UErrorCode status = U_ZERO_ERROR;
	UChar * uni_buf = NULL;
	char  * tgt_buf = NULL;
	UConverter * p_conv_mid = NULL;
	UConverter * p_conv_tgt = NULL;

	try
	{
		//Step 1. 先转换到UNICODE
		p_conv_mid = ucnv_open(src_charset.c_str(), &status);

		if(U_ZERO_ERROR != status)
		{
			cerr << "ICUConverter::Convert() create mid converter failed. err = " << status << endl;
			throw -1;
		}

		//unicode 缓冲区
		size_t uni_bufsize = size + 1;
		uni_buf = new UChar[uni_bufsize];
		assert(uni_buf);
		memset(uni_buf, 0, uni_bufsize);

		int uni_len = ucnv_toUChars(p_conv_mid, uni_buf, uni_bufsize, src, size, &status);
		
		if(U_ZERO_ERROR != status)
		{
			cerr << "ICUConverter::Convert() convert to unicode first failed. err = " << status << endl;
			throw -1;
		}

		//缓冲区大小不够 根据返回的实际len重新生成缓冲区转换
		if(uni_len > uni_bufsize-1 )
		{
			//buffer is not enough do it again.
			delete[] uni_buf;
			uni_bufsize = uni_len+1;
			uni_buf = new UChar[uni_bufsize];
			assert(uni_buf);
			memset(uni_buf, 0, uni_bufsize);
			
			uni_len = ucnv_toUChars(p_conv_mid, uni_buf, uni_bufsize, src, size, &status);
		
			if(U_ZERO_ERROR != status)
			{
				cerr << "ICUConverter::Convert() convert to unicode second failed. err = " << status << endl;
				throw -1;
			}

		}


		//Step 2. 再转换到目标编码
		p_conv_tgt = ucnv_open(tgt_charset.c_str(), &status);

		if(U_ZERO_ERROR != status)
		{
			cerr << "ICUConverter::Convert() create tgt converter failed. err = " << status << endl;
			throw -1;
		}

		//目标 缓冲区
		size_t tgt_bufsize = uni_bufsize * DEF_BUF_RATIO + 1;
		tgt_buf = new char[tgt_bufsize];
		assert(tgt_buf);
		memset(tgt_buf, 0, tgt_bufsize);

		//转换到目标编码
		int tgt_len = ucnv_fromUChars(p_conv_tgt, tgt_buf, tgt_bufsize, uni_buf, uni_len, &status);
		
		if(U_ZERO_ERROR != status)
		{
			cerr << "ICUConverter::Convert() convert to tgt_charset first failed. err = " << status << endl;
			throw -1;
		}

		//缓冲区大小不够 根据返回的实际len重新生成缓冲区转换
		if(tgt_len > tgt_bufsize-1 )
		{
			//buffer is not enough do it again.
			delete[] tgt_buf;
			tgt_bufsize = tgt_len+1;
			tgt_buf = new char[tgt_bufsize];
			assert(tgt_buf);
			memset(tgt_buf, 0, tgt_bufsize);
			
			tgt_len = ucnv_fromUChars(p_conv_tgt, tgt_buf, tgt_bufsize, uni_buf, uni_len, &status);
		
			if(U_ZERO_ERROR != status)
			{
				cerr << "ICUConverter::Convert() convert to tgt_charset second failed. err = " << status << endl;
				throw -1;
			}

		}


		//赋值到目标串
		tgt = tgt_buf;
		res = true;

	}catch(...)
	{
		cerr << "Error In ICUConverter::Convert()." << endl;
		res = false;
	}

	//清理资源
	delete[] uni_buf;
	delete[] tgt_buf;
	if(p_conv_mid) ucnv_close(p_conv_mid);
	if(p_conv_tgt) ucnv_close(p_conv_tgt);

	return res;
}

bool ICUConverter::Convert(const string & src_charset,
						   const string & src, 
						   const string & tgt_charset,
						   string & tgt)
{
	return Convert(src_charset, src.c_str(), src.size(), tgt_charset, tgt);
}


void ICUConverter::PrintAllCharset()
{
	UErrorCode status = U_ZERO_ERROR;
    UCharsetDetector *csd = ucsdet_open(&status);
    UEnumeration *e = ucsdet_getAllDetectableCharsets(csd, &status);
    int32_t count = uenum_count(e, &status);


    for(int32_t i = 0; i < count; i += 1) 
	{
        int32_t length;
        const char *name = uenum_next(e, &length, &status);

        if(name == NULL || length <= 0) {
            lerr << "ucsdet_getAllDetectableCharsets() returned a null or empty name!" << endl;
        }

        cout << name << endl;

    }

    uenum_close(e);
    ucsdet_close(csd);
}


string & ICUConverter::Convert( const string & src_charset,
								const string & tgt_charset,
								string & src)
{
	string old(src);

	if(old.size() == 0)
		return src;

	Convert(src_charset, old, tgt_charset, src);

	return src;
}

