#include "NewTools.h"

namespace LKTools
{
	void ParseWord(const string& strLine, const string& tag, vector<string>& vecStr)
	{
		string::size_type l = 0;
		string::size_type r = 0;

		do 
		{
			l = strLine.find_first_not_of( tag.c_str(), r );
			if (l == string::npos)
				break;
			r = strLine.find_first_of( tag.c_str(), l );
			vecStr.push_back( strLine.substr(l, r-l));

		} while ( r < strLine.length() - 1 || r != string::npos );
	}

	void ParseWord(const string& strLine, const string& tag, vector<pair<string::size_type, string::size_type> >& vecTag)
	{
		string::size_type l = 0;
		string::size_type r = 0;

		do 
		{
			l = strLine.find_first_not_of( tag.c_str(), r );
			if (l == string::npos)
				break;
			r = strLine.find_first_of( tag.c_str(), l );
			vecTag.push_back(make_pair(l, r));

		} while ( r < strLine.length() - 1 || r != string::npos );
	}

	unsigned int ParseWord(const string& strLine, const string& tag)
	{
		string::size_type l = 0;
		string::size_type r = 0;
		unsigned int num = 0;
		do 
		{
			l = strLine.find_first_not_of( tag.c_str(), r );
			if (l == string::npos)
				break;
			r = strLine.find_first_of( tag.c_str(), l );
			num++;

		} while ( r < strLine.length() - 1 || r != string::npos );

		return num;
	}

	vector<int> GetIntAln(const string& strAln)
	{
		vector<string> vStr;
		ParseWord(strAln, " \t:-", vStr);
		vector<int> vInt;
		vString2vX(vStr, vInt);
		return vInt;
	}

	string GetStr(const vector<string>& vecStr, unsigned int l, unsigned int r)
	{
		if (vecStr.size() == 0)
			return string();
		if (l >= vecStr.size())
			return string();
		if (r >= vecStr.size())
			r = vecStr.size() - 1;
		string str;
		for (unsigned int i = l; i <=r; i++)
		{
			if (i == l)
				str = vecStr[i];
			else
				str = str + " " + vecStr[i];
		}
		return str;
	}

	vector<string> GetSubVec(const vector<string>& vecStr, unsigned int l, unsigned int r)
	{
		if ( l >= vecStr.size() || vecStr.size() == 0)
			return vector<string>();
		if ( r >= vecStr.size())
			r = vecStr.size() - 1;

		vector<string> _vecSub;
		for (unsigned int i = l; i <= r; i++)
		{
			_vecSub.push_back(vecStr[i]);
		}
		return _vecSub;
	}
	//------------------------------------------------------------------------
	string Vec2Str(const vector<string>& vec, const string tag)
	{
		string str;
		vector<string>::const_iterator itr;
		for (itr = vec.begin(); itr != vec.end(); itr++)
		{
			if (itr == vec.begin())
				str = *itr;
			else
				str = str + tag + *itr;
		}
		return str;
	}
	//------------------------------------------------------------------------
	string Trim(const string& str, string tag)
	{
		string::size_type left = str.find_first_not_of( tag );
		string::size_type right = str.find_last_not_of( tag );
		if (left == string::npos && right == string::npos)
			return "";
		else
			return str.substr( left, right - left + 1 );
	}
	//------------------------------------------------------------------------
	vector<string> CutSubSentence(const string& str, const string& tag, const unsigned int mini)
	{
		string::size_type l = 0;
		string::size_type r = 0;

		vector<string> vecSubs;

		do 
		{
			l = str.find_first_not_of( tag.c_str(), r );
			if (l == string::npos)
				break;
			r = str.find_first_of( tag.c_str(), l );
			if (r == string::npos)
				vecSubs.push_back( str.substr(l, r-l));
			else
				vecSubs.push_back( str.substr(l, r-l+1));

		} while ( r < str.length() - 1 || r != string::npos );

		return vecSubs;
	}
	//------------------------------------------------------------------------
	string& Replace_all(string& str,const string& old_value,const string& new_value)
	{
		string::size_type pos(0);
		while(true)
		{
			if( (pos=str.find(old_value, pos))!= string::npos )
				str.replace(pos,old_value.length(),new_value);
			else break;
			pos += new_value.length();
		}
		return str;
	}
	//------------------------------------------------------------------------
	void GetPosFix(const vector<string>& vWord, vector<string>& vPostFix, const string& tag)
	{
		vPostFix.resize(vWord.size(), "");
		for (unsigned int i = 0; i < vWord.size(); i++)
		{
			string::size_type pos = vWord[i].find_last_of(tag.c_str());
			if (pos != string::npos)
				vPostFix[i] = vWord[i].substr(pos+1);
		}
	}
	//------------------------------------------------------------------------
	void GetPreFix(const vector<string>& vWord, vector<string>& vPreFix, const string& tag)
	{
		vPreFix.resize(vWord.size(), "");
		for (unsigned int i = 0; i < vWord.size(); i++)
		{
			string::size_type pos = vWord[i].find_last_of(tag.c_str());
			if (pos != string::npos)
				vPreFix[i] = vWord[i].substr(0, pos);
		}
	}
	//------------------------------------------------------------------------
	string Tolower(string& str)
	{
		for(string::size_type i = 0; i < str.size(); i++)
			if (str.at(i) >= 65 && str.at(i) <= 90)
				str.at(i) += 32;
		return str;
	}
	//------------------------------------------------------------------------
	string Toupper(string& str)
	{
		for(string::size_type i = 0; i < str.size(); i++)
			if (str.at(i) >= 97 && str.at(i) <= 122)
				str.at(i) -= 32;
		return str;
	}
	//------------------------------------------------------------------------
	unsigned int CountStrNum(const string& str, const string& tgt)
	{
		unsigned int count = 0;
		string::size_type pos = 0;
		while (true)
		{
			pos = str.find(tgt, pos);
			if (pos == string::npos)
				break;
			count++;
			pos += tgt.size();
		}
		return count;
	}
	//------------------------------------------------------------------------
	void RemoveTag(const string& src, string& tgt, const string& l_tag, const string& r_tag)
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
	//------------------------------------------------------------------------
	SPAN_TYPE SpanCompare(const PrInt& sp1, const PrInt& sp2)
	{
		if (sp1 == sp2) return SPAN_EQUAL;
		if (sp1.first <= sp2.first)
		{
			if (sp1.second >= sp2.second) return SPAN_SUPERSET; // sp1.first <= sp2.first
			if (sp1.first == sp2.first) return SPAN_SUBSET; // according to before: no equal & sp1.second < sp2.second => sp1.second < sp2.second
			// remaining are sp1.first < sp2.first :
			if (sp1.second == sp2.first-1) return SPAN_NEXTLEFT;
			if (sp1.second >= sp2.first) return SPAN_INTERSE; // 
			return SPAN_NONINTC; // no being next to sp2
		}
		else // sp1.first > sp2.first
		{
			if (sp1.second <= sp2.second) return SPAN_SUBSET; // sp1.first > sp2.first & sp1.second <= sp2.second
			// remaining are sp1.second > sp2.second
			if (sp1.first <= sp2.second) return SPAN_INTERSE; // sp1.second > sp2.second & sp1.first <= sp2.second
			if (sp1.first == sp2.second+1) return SPAN_NEXTRIGHT; // sp1.second > sp2.second
			return SPAN_NONINTC;
		}
	}
	//----------------------------------------------------------------------
	bool SpanIn(const PrInt& sp_in, const PrInt& sp_static)
	{
		return (sp_in.first >= sp_static.first && sp_in.second <= sp_static.second);
	}
	//----------------------------------------------------------------------
	// Simple one : give the minimized closure span of the two span
	PrInt SpanClosure(const PrInt& sp1, const PrInt& sp2)
	{
		PrInt resSp;
		resSp.first = min(sp1.first, sp2.first);
		resSp.second = max(sp1.second, sp2.second);
		return resSp;
	}
	// 
	unsigned int SpanSize(const PrInt& sp)
	{
		return abs(sp.second - sp.first);
	}
	// remove the small spans that have been contain 
	void SpanReduce(set<PrInt>& setSP)
	{
		for (set<PrInt>::iterator itr = setSP.begin(); itr != setSP.end(); )
		{
			set<PrInt>::iterator itr_next = itr;
			itr_next++;
			if (itr_next == setSP.end())
				break;
			if ((*itr).first == (*itr_next).first)
				itr = setSP.erase(itr);
			else if ((*itr).second >= (*itr_next).first && (*itr).second >= (*itr_next).second) //(*itr).sp.span.first <= (*itr_next).sp.span.first
				itr_next = setSP.erase(itr_next);
			else
				itr++;
		}
	}
}