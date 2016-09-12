#pragma once

 namespace thaiSeg{
//±ê×¢ÔªËØ
class CTagElem
{
public:
	string ch;
	int type;
	string tag;

	double best;
	int length;

	CTagElem(const string &chVal, const int &typeVal)
	{
		ch = chVal;
		type = typeVal;
	}
};
}

