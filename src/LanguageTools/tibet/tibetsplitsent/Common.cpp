/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#include "Common.h"

namespace tibetsentenizer
{
	std::string trim(const std::string& str)
	{
		if (str == "")
		{
			return str;
		}
		std::string blank = " \n\r\t";
		size_t beginPos   = str.find_first_not_of(blank);
		size_t endPos     = str.find_last_not_of(blank);

		return str.substr(beginPos, endPos - beginPos + 1);
	}

	std::string num2str(int num)
	{
		std::stringstream ss;
		ss << num;
		std::string numString = ss.str();

		return numString;
	}

	const std::string& getCharSign(void)
	{
		return char_sign;
	}

	const std::string& getSentSign(void)
	{
		return sent_sign;
	}

	const std::string& getCloudSign(void)
	{
		return cloud_sign;
	}

	const std::string& getSign1(void)
	{
		return sign1;
	}

	const std::string& getSign2(void)
	{
		return sign2;
	}

	void setSign()
	{
		char temp[10];
		temp[0] = (char)0xE0;
		temp[1] = (char)0xBC;
		temp[2] = (char)0x8B;
		temp[3] = '\0';
		char_sign  = temp;

		temp[0] = (char)0xE0;
		temp[1] = (char)0xBC;
		temp[2] = (char)0x8D;
		temp[3] = '\0';
		sent_sign  = temp;

		temp[0] = (char)0xE0;
		temp[1] = (char)0xBC;
		temp[2] = (char)0x84;
		temp[3] = (char)0xE0;
		temp[4] = (char)0xBC;
		temp[5] = (char)0x85;
		temp[6] = '\0';
		cloud_sign = temp;

		temp[0] = (char)0xE0;
		temp[1] = (char)0xBD;
		temp[2] = (char)0x82;
		temp[3] = '\0';
		sign1 = temp;

		temp[0] = (char)0xE0;
		temp[1] = (char)0xBD;
		temp[2] = (char)0x82;
		temp[3] = (char)0xE0;
		temp[4] = (char)0xBD;
		temp[5] = (char)0xBC;
		temp[6] = '\0';
		sign2 = temp;
	}

}
