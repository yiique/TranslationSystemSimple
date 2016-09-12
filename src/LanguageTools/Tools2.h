#ifndef TOOLS2_H
#define TOOLS2_H

#include <string>
#include <vector>
//#include <sstream>
//#include <stack>
#undef max
#undef min
#include<sstream>


using namespace std;

namespace Tools
{
	class StrTool
	{
	public:	
		template<class T>
		static T string2X(const std::string str, T& x)
		{
			std::stringstream sstream(str);
			sstream >> x;
			return x;
		}

		template<class T>
		static std::string X2string(const T& x)
		{
			std::stringstream ssteam;
			ssteam << x;
			return ssteam.str();
		}

		template<class T>
		static void vString2vX(const std::vector<std::string>& vStr, std::vector<T>& vResult)
		{
			T temp;
			for (unsigned int i = 0; i < vStr.size(); i++)
			{
				string2X(vStr[i], temp);
				vResult.push_back(temp);
			}
		}

		static void parseWord(const std::string& strLine, const std::string& tag, std::vector<std::string>& vecStr)
		{
			std::string::size_type l = 0;
			std::string::size_type r = 0;

			do 
			{
				l = strLine.find_first_not_of( tag.c_str(), r );
				if (l == std::string::npos)
					break;
				r = strLine.find_first_of( tag.c_str(), l );
				vecStr.push_back( strLine.substr(l, r-l));

			} while ( r < strLine.length() - 1 || r != std::string::npos );
		}

		static void parseWord(const std::string& strLine, const std::string& tag, std::vector<std::pair<std::string::size_type, std::string::size_type> >& vecTag)
		{
			std::string::size_type l = 0;
			std::string::size_type r = 0;

			do 
			{
				l = strLine.find_first_not_of( tag.c_str(), r );
				if (l == std::string::npos)
					break;
				r = strLine.find_first_of( tag.c_str(), l );
				vecTag.push_back(std::make_pair(l, r));

			} while ( r < strLine.length() - 1 || r != std::string::npos );
		}
		// return word number
		static unsigned int parseWord(const std::string& strLine, const std::string& tag)
		{
			std::string::size_type l = 0;
			std::string::size_type r = 0;
			unsigned int num = 0;
			do 
			{
				l = strLine.find_first_not_of( tag.c_str(), r );
				if (l == std::string::npos)
					break;
				r = strLine.find_first_of( tag.c_str(), l );
				num++;

			} while ( r < strLine.length() - 1 || r != std::string::npos );
			
			return num;
		}

		static std::string getStr(const std::vector<std::string>& vecStr, unsigned int l = 0, unsigned int r = (unsigned int) - 1)
		{
			if (vecStr.size() == 0)
				return std::string();
			if (l >= vecStr.size())
				return std::string();
			if (r >= vecStr.size())
				r = vecStr.size() - 1;
			std::string str;
			for (unsigned int i = l; i <=r; i++)
			{
				if (i == l)
					str = vecStr[i];
				else
					str = str + " " + vecStr[i];
			}
			return str;
		}

		static std::vector<std::string> getSubVec(const std::vector<std::string>& vecStr, unsigned int l = 0, unsigned int r = (unsigned int) - 1)
		{
			if ( l >= vecStr.size() || vecStr.size() == 0)
				return std::vector<std::string>();
			if ( r >= vecStr.size())
				r = vecStr.size() - 1;

			std::vector<std::string> _vecSub;
			for (unsigned int i = l; i <= r; i++)
			{
				_vecSub.push_back(vecStr[i]);
			}
			return _vecSub;
		}

		static std::string vec2str(const std::vector<std::string>& vec, const std::string tag = " ")
		{
			std::string str;
			std::vector<std::string>::const_iterator itr;
			for (itr = vec.begin(); itr != vec.end(); itr++)
			{
				if (itr == vec.begin())
					str = *itr;
				else
					str = str + tag + *itr;
			}
			return str;
		}

		static void trim(std::string& str, std::string tag = " ")
		{
			std::string::size_type left = str.find_first_not_of( tag );
			std::string::size_type right = str.find_last_not_of( tag );
			if (left == string::npos && right == string::npos)
				str = "";
			else
				str = str.substr( left, right - left + 1 );
		}

		static std::vector<std::string> cutSubSentence(const std::string& str, const std::string& tag, const unsigned int mini = std::string::npos)
		{
			std::string::size_type l = 0;
			std::string::size_type r = 0;

			std::vector<std::string> vecSubs;

			do 
			{
				l = str.find_first_not_of( tag.c_str(), r );
				if (l == std::string::npos)
					break;
				r = str.find_first_of( tag.c_str(), l );
				if (r == std::string::npos)
					vecSubs.push_back( str.substr(l, r-l));
				else
					vecSubs.push_back( str.substr(l, r-l+1));

			} while ( r < str.length() - 1 || r != std::string::npos );

			return vecSubs;
		}

		static std::string& replace_all(std::string& str,const std::string& old_value,const std::string& new_value, const unsigned int& _beg = 0, const unsigned int& _end = -1)
		{
			std::string::size_type pos(_beg);
			while(true)
			{
				pos=str.find(old_value, pos);
				if (pos > _end)
					return str;
				if(pos != std::string::npos)
					str.replace(pos,old_value.length(),new_value);
				else break;
				pos += new_value.length();
			}
			return str;
		}

		static void getPostFix(const std::vector<std::string>& vWord, std::vector<std::string>& vPostFix, const std::string& tag = "/")
		{
			vPostFix.resize(vWord.size(), "");
			for (unsigned int i = 0; i < vWord.size(); i++)
			{
				std::string::size_type pos = vWord[i].find_last_of(tag.c_str());
				if (pos != std::string::npos)
					vPostFix[i] = vWord[i].substr(pos+1);
			}
		}

		static void getComPostFix(const std::vector<std::string>& vWord, std::vector<std::string>& vComPreFix, const std::string& tag = "/")
		{
			vComPreFix.resize(vWord.size(), "");
			for (unsigned int i = 0; i < vWord.size(); i++)
			{
				std::string::size_type pos = vWord[i].find_last_of(tag.c_str());
				if (pos != std::string::npos)
					vComPreFix[i] = vWord[i].substr(0, pos);
			}
		}
		static void tolower(std::string& str)
		{
			for(std::string::size_type i = 0; i < str.size(); i++)
				if (str.at(i) >= 65 && str.at(i) <= 90)
					str.at(i) += 32;
		}
		template<class T1, class T2>
		static string pair2str(const pair<T1, T2>& _p)
		{
			stringstream sstream;
			sstream << "(" << _p.first << "," << _p.second << ")";
			return sstream.str();
		}
		static void removeTag(const string& src, string& tgt, const string& l_tag = "<", const string& r_tag = ">")
		{
			string::size_type l = 0;
			string::size_type r = 0;
			while (true)
			{
				r = src.find_first_of(l_tag, l);
				if (r == string::npos)
				{
					tgt += src.substr(l, r);
					break;
				}
				else // if (r != string::npos)
				{
					string::size_type r_temp = src.find_first_of(r_tag, r);
					if (r_temp == string::npos)
					{
						tgt += src.substr(r, r_temp);
						break;
					}
					else // if (l != string::npos)
					{
						tgt += src.substr(l, r-l);
						l = r_temp+1;
					}
				}
			}
		}
	};

	class AlgTool
	{
	public:
		template<class T>
		static bool nextArrange(vector<T>& cur, const vector<T>& beg, const vector<T>& end)
		{
			if (cur.size() == 0)
				return false;
			unsigned int last = cur.size()-1;
			while (true)
			{
				cur[last]++;
				if (cur[last] == end[last])
				{
					cur[last] = beg[last];
					if (last == 0)
						return false;
					else
						last--;
				}
				else
					return true;
			}
			return true;
		}
		/*
		size like this
		333
		33
		3
		*/
		template<class T>
		static void resizeCKY(vector<vector<T> >& vv, const unsigned int& n)
		{
			vv.resize(n);
			for (unsigned int i = 0; i < vv.size(); i++)
				vv[i].resize(n-i);
		}
		template<class T>
		static void resizeCKY(vector<vector<T> >& vv, const unsigned int& n, const T& _init)
		{
			vv.resize(n);
			for (unsigned int i = 0; i < vv.size(); i++)
				vv[i].resize(n-i, _init);
		}
	};
	class SysTool
	{
	public:
		template<class T>
		static void safe_delete(T p)
		{
			if (p != NULL)
			{
				delete p;
				p = NULL;
			}
		}
#define LK_IFSTREAM(fin,str) ifstream fin(str.c_str());if(fin.fail() == true) { cerr << "can not open file " << str << endl; exit(1);}
	};
	template<class T>
	struct TypeT
	{
		typedef vector<vector<T> > vector2;
		typedef vector<vector<vector<T> > > vector3;
		typedef pair<typename T::iterator, bool> insert_itr;
	};
};
#endif
