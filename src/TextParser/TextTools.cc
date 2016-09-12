#include "TextTools.h"


bool IsEnglishText(const string & src)
{
	for(size_t i=0; i<src.size(); ++i)
	{

		if( 0 > (int) src[i] )
			return false;
	}

	return true;
}


bool RevertTextWithPos(const string & src,
					   const vector<string> & tgt_vec, 
					   const vector<pair<size_t,size_t> > & pos_vec,
					   string & tgt)
{

	if(tgt_vec.size() != pos_vec.size())
	{
		cerr << "Error Target Vector's size is not equal with Posvec.size." << endl;
		return false;
	}

	tgt.clear();
	size_t para_idx = 0;
	size_t offset = 0;
	while(offset < src.size())
	{
		if(para_idx < tgt_vec.size())
		{
			size_t len = pos_vec[para_idx].first - offset;

			//Êä³ö·Ç·­Òë¶ÎÂä
			if(len > 0)
			{
				tgt += src.substr(offset, len);
			}

			string tmp = tgt_vec[para_idx];

			//Êä³ö·­Òë¶ÎÂä
			tgt += tgt_vec[para_idx];

			//¸üÐÂÆ«ÒÆÁ¿
			offset = pos_vec[para_idx].first + pos_vec[para_idx].second;
			++para_idx;

		}else
		{
			size_t len = src.size() - offset;

			//Êä³ö·Ç·­Òë¶ÎÂä
			if(len > 0)
			{
				tgt += src.substr(offset, len);
			}

			break;
		}
	}

	return true;


}
