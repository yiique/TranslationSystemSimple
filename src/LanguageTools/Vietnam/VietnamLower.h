#ifndef _VILOWER_H_
#define _VILOWER_H_

#include <fstream>
#include <string>
#include <map>

using namespace std;


#include "../../Common/Utf8Stream.h"

class VietnamLower
{
public:
	VietnamLower(const string& file)
	{
		ifstream mapfile(file.c_str());
		string line;
		while(getline(mapfile,line))
		{
			stringstream iss(line);
			string first,second;
			iss >> first >> second;
			mData.insert(make_pair(first,second));
		}
	}
	string Tolower(const string& str)
	{
		string str_lower;
		Utf8Stream utf(str);

		string letter;
		map<string,string>::iterator iter;
		while(utf.ReadOneCharacter(letter))
		{
			iter = mData.find(letter);
			if (iter != mData.end())
				str_lower += iter->second;
			else
				str_lower += letter;
		}
		return str_lower;
	}
private:
	map<string, string> mData;
};
#endif