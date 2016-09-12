#ifndef _NEW_TOOLS_
#define _NEW_TOOLS_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <sstream>

using namespace std;

namespace LKTools
{
//////////////////////////////////////////////////////////////////////////
						/*String Tools*/
//////////////////////////////////////////////////////////////////////////
	//------------------------------------------------------------------------
	template<class T>
	T String2X(const string str, T& x)
	{
		stringstream sstream(str);
		sstream >> x;
		return x;
	}
	//------------------------------------------------------------------------
	template<class T>
	T String2X(const string str)
	{
		T x;
		stringstream sstream(str);
		sstream >> x;
		return x;
	}
	//------------------------------------------------------------------------
	template<class T>
	string X2String(const T& x)
	{
		stringstream ssteam;
		ssteam << x;
		return ssteam.str();
	}
	//------------------------------------------------------------------------
	template<class T>
	void vString2vX(const vector<string>& vStr, vector<T>& vResult)
	{
		T temp;
		for (unsigned int i = 0; i < vStr.size(); i++)
		{
			String2X(vStr[i], temp);
			vResult.push_back(temp);
		}
	}
	//------------------------------------------------------------------------
	void ParseWord(const string& strLine, const string& tag, vector<string>& vecStr);
	//------------------------------------------------------------------------
	void ParseWord(const string& strLine, const string& tag, vector<pair<string::size_type, string::size_type> >& vecTag);
	//------------------------------------------------------------------------
	// return word number
	unsigned int ParseWord(const string& strLine, const string& tag);
	//------------------------------------------------------------------------
	vector<int> GetIntAln(const string& strAln);
	//------------------------------------------------------------------------
	string GetStr(const vector<string>& vecStr, unsigned int l = 0, unsigned int r = (unsigned int) - 1);
	//------------------------------------------------------------------------
	vector<string> GetSubVec(const vector<string>& vecStr, unsigned int l = 0, unsigned int r = (unsigned int) - 1);
	//------------------------------------------------------------------------
	string Vec2Str(const vector<string>& vec, const string tag = " ");
	//------------------------------------------------------------------------
	string Trim(const string& str, string tag = " ");
	//------------------------------------------------------------------------
	vector<string> CutSubSentence(const string& str, const string& tag, const unsigned int mini = string::npos);
	//------------------------------------------------------------------------
	string& Replace_all(string& str,const string& old_value,const string& new_value);
	//------------------------------------------------------------------------
	void GetPosFix(const vector<string>& vWord, vector<string>& vPostFix, const string& tag = "/");
	//------------------------------------------------------------------------
	void GetPreFix(const vector<string>& vWord, vector<string>& vPreFix, const string& tag = "/");
	//------------------------------------------------------------------------
	string Tolower(string& str);
	//------------------------------------------------------------------------
	string Toupper(string& str);
	//------------------------------------------------------------------------
	template<class T1, class T2>
	string Pair2Str(const pair<T1, T2>& _p)
	{
		stringstream sstream;
		sstream << "(" << _p.first << "," << _p.second << ")";
		return sstream.str();
	}
	//------------------------------------------------------------------------
	unsigned int CountStrNum(const string& str, const string& tgt);
	//------------------------------------------------------------------------
	void RemoveTag(const string& src, string& tgt, const string& l_tag = "<", const string& r_tag = ">");
	//------------------------------------------------------------------------
	template<class T>
	unsigned int GetNumStrLength(const T& num)
	{
		stringstream sstream;
		sstream << num;
		return sstream.str().size();
	}
//////////////////////////////////////////////////////////////////////////
						/*Structure Tools*/
//////////////////////////////////////////////////////////////////////////
	template<class T>
	bool NextArrange(vector<T>& cur, const vector<T>& beg, const vector<T>& end)
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
	//------------------------------------------------------------------------
	/*
	size like this
	333
	33
	3
	*/
	template<class T>
	void ResizeCKY(vector<vector<T> >& vv, const unsigned int& n)
	{
		vv.resize(n);
		for (unsigned int i = 0; i < vv.size(); i++)
			vv[i].resize(n-i);
	}
	//------------------------------------------------------------------------
	template<class T>
	void ResizeCKY(vector<vector<T> >& vv, const unsigned int& n, const T& _init)
	{
		vv.resize(n);
		for (unsigned int i = 0; i < vv.size(); i++)
			vv[i].resize(n-i, _init);
	}
	//------------------------------------------------------------------------
	template<class T>
	unsigned int SetDiff(const T& sorted1, const T& sorted2, T& diff)
	{
		set_different(sorted1.begin(), sorted1.end(), sorted2.begin(), sorted2.end(), inserter(diff, diff.begin()));
		return diff.size();
	}
	//------------------------------------------------------------------------
	template<class T>
	unsigned int SetIntersection(const T& sorted1, const T& sorted2, T& insec)
	{
		set_intersection(sorted1.begin(), sorted1.end(), sorted2.begin(), sorted2.end(), inserter(insec, insec.begin()));
		return insec.size();
	}
	//------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
						/*Sys Tools*/
//////////////////////////////////////////////////////////////////////////
	template<class T>
	void SafeDelete(T p)
	{
		if (p != NULL)
		{
			delete p;
			p = NULL;
		}
	}
	//------------------------------------------------------------------------
#define LK_IFSTREAM(fin,str) ifstream fin(str.c_str());if(fin.fail() == true) { cerr << "can not open file " << str << endl; exit(1);}
	
//////////////////////////////////////////////////////////////////////////
					/*My Data Structure*/
//////////////////////////////////////////////////////////////////////////
	template <class _Type, unsigned int _Size = 4>
	struct vector_t 
	{
		vector_t(void)
		{
			memset(value, 0, sizeof(_Type)*_Size);
		}

		vector_t(const  vector_t<_Type, _Size>& vRight)
		{
			for (unsigned int i = 0; i < _Size; i++)
				value[i] = vRight.value[i];
		}

		vector_t(const  vector<_Type>& vRight, const unsigned int& uNum)
		{
			memset(value, 0, sizeof(_Type)*_Size);
			for (unsigned int i = 0; i < uNum; i++)
				value[i] = vRight[i];
		}

		vector_t<_Type, _Size>& operator = (const vector_t<_Type, _Size>& vRight)
		{
			for (unsigned int i = 0; i < _Size; i++)
				value[i] = vRight.value[i];
			return *this;
		}

		bool operator == (const  vector_t<_Type, _Size>& vRight) const
		{
			return memcmp(value, vRight.value, sizeof(_Type)*_Size) == 0;
		}

		bool operator < (const  vector_t<_Type, _Size>& vRight) const
		{
			return memcmp(value, vRight.value, sizeof(_Type)*_Size) < 0;
		}

		bool operator > (const  vector_t<_Type, _Size>& vRight) const
		{
			return memcmp(value, vRight.value, sizeof(_Type)*_Size) > 0;
		}

		_Type& operator[] (const unsigned int& uIdx) { return value[uIdx]; }

		const _Type& operator[] (const unsigned int& uIdx) const { return value[uIdx]; }

		inline unsigned int GetSize(void) const { return _Size; }

		_Type value[_Size];
	};

	typedef pair<unsigned int, unsigned int> PrUns;
	typedef pair<string, string> PrStr;
	typedef pair<int, int> PrInt;
	typedef vector<int> vint;
	typedef vector<double> vdouble;
	typedef vector<string> vstring;
	//-----------------------------------------------------------------------
	enum SPAN_TYPE
	{
		SPAN_EQUAL,
		SPAN_SUBSET,
		SPAN_SUPERSET,
		SPAN_INTERSE,
		SPAN_NONINTC,
		SPAN_NEXTLEFT,
		SPAN_NEXTRIGHT
	};
	// sp1 > sp2 => sp1 is a superset
	SPAN_TYPE SpanCompare(const PrInt& sp1, const PrInt& sp2);
	//----------------------------------------------------------------------
	bool SpanIn(const PrInt& sp_in, const PrInt& sp_static);
	//----------------------------------------------------------------------
	// Simple one : give the minimized closure span of the two span
	PrInt SpanClosure(const PrInt& sp1, const PrInt& sp2);
	// 
	unsigned int SpanSize(const PrInt& sp);
	// remove the small spans that have been contain 
	void SpanReduce(set<PrInt>& setSP);
//////////////////////////////////////////////////////////////////////////
					/*Useful String*/
//////////////////////////////////////////////////////////////////////////
	const char RegEnNumStr[] = "[\\s-]+(o[\\s-]+|zero[\\s-]+|one[\\s-]+|first[\\s-]+|second[\\s-]+|third[\\s-]+|two[\\s-]+|double[\\s-]+|three[\\s-]+|four[\\s-]+|five[\\s-]+|six[\\s-]+|seven[\\s-]+|eight[\\s-]+|nine[\\s-]+|ten[\\s-]+|eleven[\\s-]+|twelve[\\s-]+|thirteen[\\s-]+|fourteen[\\s-]+|fifteen[\\s-]+|sixteen[\\s-]+|seventeen[\\s-]+|eighteen[\\s-]+|nineteen[\\s-]+|twenty[\\s-]+|thirty[\\s-]+|forty[\\s-]+|fifty[\\s-]+|sixty[\\s-]+|seventy[\\s-]+|eighty[\\s-]+|ninety[\\s-]+|hundred[\\s-]+|thousand[\\s-]+|million[\\s-]+|billion[\\s-]+){2,}(th)?";
	const char RegChNumStr[] = "(零|一|二|两|三|四|五|六|七|八|九|十|百|千|万|亿){2,}";
	const char RegArNumStr[] = "[0-9]+";
};

#endif
