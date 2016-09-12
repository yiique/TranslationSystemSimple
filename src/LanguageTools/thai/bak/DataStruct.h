#pragma once

namespace thaiSeg {

const int MaxLen = 8;

typedef struct SElem{
	
	string ch;
	
	/*
	0 -> ���ϸ���
	1 -> ����Ԫ��
	2 -> Ԫ��
	3 -> ���ϸ���
	4 -> β����
	5 -> ����
	6 -> ����
	7 -> ������������������
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

//��עԪ��
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

