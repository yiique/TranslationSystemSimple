/*
 *@date: 2011-10-12
 *@author: Li Xiang
 *@copyright: ICT.CAS
*/
#ifndef _TIBETSENTPARSE_H_
#define _TIBETSENTPARSE_H_

#include "Common.h"
#include <cctype>

namespace tibetsentenizer
{
#define SENTSPLIT 0
#define SYLLSPLIT 1

	class TibetSentParse
	{
	public:

		TibetSentParse (const iLines& punctLine, const iLines& digitLine):
		  m_sCharSegSign(getCharSign()), m_sSentSegSign(getSentSign()), m_sCloudSign(getCloudSign()), m_sLeftPunct("{[<("), m_sRightPunct("}]>)")
		  {
			  m_sPunctLine = punctLine;
			  m_sDigitLine = digitLine;

			  std::string char_sign;
			  char temp[3];
			   temp[2] = '\0';

			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xB0;
			  char_sign  = temp;
			  m_sLeftTiPunct.insert(char_sign);
			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xB1;
			  char_sign  = temp;
			  m_sRightTiPunct.insert(char_sign);
			  
			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xB2;
			  char_sign  = temp;
			  m_sLeftTiPunct.insert(char_sign);
			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xB3;
			  char_sign  = temp;
			  m_sRightTiPunct.insert(char_sign);

			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xB4;
			  char_sign  = temp;
			  m_sLeftTiPunct.insert(char_sign);
			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xB5;
			  char_sign  = temp;
			  m_sRightTiPunct.insert(char_sign);

			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xB6;
			  char_sign  = temp;
			  m_sLeftTiPunct.insert(char_sign);
			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xB7;
			  char_sign  = temp;
			  m_sRightTiPunct.insert(char_sign);

			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xBE;
			  char_sign  = temp;
			  m_sLeftTiPunct.insert(char_sign);
			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xBF;
			  char_sign  = temp;
			  m_sRightTiPunct.insert(char_sign);

			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xAE;
			  char_sign  = temp;
			  m_sLeftTiPunct.insert(char_sign);
			  temp[0] = (char)0xA1;
			  temp[1] = (char)0xAF;
			  char_sign  = temp;
			  m_sRightTiPunct.insert(char_sign);

			  temp[0] = (char)0xA3;
			  temp[1] = (char)0xA8;
			  char_sign  = temp;
			  m_sLeftTiPunct.insert(char_sign);
			  temp[0] = (char)0xA3;
			  temp[1] = (char)0xA9;
			  char_sign  = temp;
			  m_sRightTiPunct.insert(char_sign);

			  temp[0] = (char)0xA3;
			  temp[1] = (char)0xFB;
			  char_sign  = temp;
			  m_sLeftTiPunct.insert(char_sign);
			  temp[0] = (char)0xA3;
			  temp[1] = (char)0xFD;
			  char_sign  = temp;
			  m_sRightTiPunct.insert(char_sign);
		  }

		  ~TibetSentParse(void){}

		  int sentParse(VecOfVecOfWordProperty &wordVec, iLines &copurs);

		  const iLines& getPunctLine(void)
		  {
			  return m_sPunctLine;
		  }

	private:
		const std::string& m_sCharSegSign;
		const std::string& m_sSentSegSign;
		const std::string& m_sCloudSign;
		const std::string m_sLeftPunct, m_sRightPunct;
		std::set<std::string> m_sLeftTiPunct, m_sRightTiPunct;

		iLines m_sPunctLine;
		iLines m_sDigitLine;

		int  __parseSyllable(VecOfStr &vec, std::string str);
		int  __split(VecOfStr &vec, std::string str, std::string key, int type);
		int  __token(VecOfVecOfWordProperty &wordVec, std::string sent);
		std::string __replace(std::string str);
	};
}

#endif //_TIBETSENTPARSE_H_
