#pragma once

namespace thaiSeg {

const int MaxLen = 8;

typedef struct SElem{
	
	string ch;
	
	/*
	0 -> 复合辅音
	1 -> 特殊元音
	2 -> 元音
	3 -> 复合辅音
	4 -> 尾辅音
	5 -> 辅音
	6 -> 声调
	7 -> 其他（不该有这个类别）
	*/
	int ntype;

	SElem()
	{
	}

	SElem(const string &s, const int &n)
	{
		ch = s;
		ntype = n;
	}

	SElem& operator =(const SElem &node2) 
	{
		this->ch = node2.ch;
		this->ntype = node2.ntype;

		return *this;
	}
	
}SElem;

//标注元素
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

