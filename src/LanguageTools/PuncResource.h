#ifndef PUNC_RESOURCE_H
#define PUNC_RESOURCE_H

#include <iostream>
#include <map>
#include <string>
#include <set>

using namespace std;

namespace ssplite
{

#define TYPE_PUNC_NORMAL   0
#define TYPE_PUNC_TERMINAL 1
#define TYPE_WORD          2
#define TYPE_PUNC_QUOT     3
#define TYPE_PUNC_QUOT_LEFT 4
#define TYPE_PUNC_QUOT_RIGHT 5


class PuncResource
{
public:
	virtual ~PuncResource(void) {}

	virtual string & Conv2StdPunc(string & punc_name) const;
	bool FindPuncAttr(const string & punc_name, int & attr) const;
	bool IsLeftRightMatch(const string & left_name, const string & right_name) const;
	bool IsRightSpeakQuot(const string & punc_name) const;
	const string & SentEndPunc() const
	{
		return m_sent_end_punc;
	}

	const string & SentEndPuncName() const
	{
		return m_sent_end_punc_name;
	}


protected:
	PuncResource(void);


	virtual void init_punc_resource() = 0;

	map<string, int> m_punc_map;
	set<pair<string, string> > m_left_right_set;
	map<string, string> m_std_conv_map;
	set<string> m_right_speak_quot_set;

	string m_sent_end_punc; //用于加在没有结束标点的句子后面
	string m_sent_end_punc_name;
	
};



class ChPuncResource
	: public PuncResource
{
public:
	static const ChPuncResource & GetInstrance();
	~ChPuncResource(void) {}

	string & Conv2StdPunc(string & punc_name) const;
private:
	
	void init_punc_resource();
	static ChPuncResource ms_ch_punc_resource;
	

	ChPuncResource(void) 
	{
		init_punc_resource();
	}
	

};

class EnPuncResource
	: public PuncResource
{
public:
	static const EnPuncResource & GetInstrance();
	~EnPuncResource(void) {}
private:
	
	void init_punc_resource();
	static EnPuncResource ms_en_punc_resource;
	

	EnPuncResource(void) 
	{
		init_punc_resource();
	}
	

};

class UyPuncResource
	: public PuncResource
{
public:
	static const UyPuncResource & GetInstrance();
	~UyPuncResource(void) {}
private:
	
	void init_punc_resource();
	static UyPuncResource ms_uy_punc_resource;
	

	UyPuncResource(void) 
	{
		init_punc_resource();
	}
	

};

}

#endif //PUNC_RESOURCE_H
