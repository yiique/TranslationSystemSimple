#ifndef OLT_SUBMIT_RES_H
#define OLT_SUBMIT_RES_H

//MAIN
#include "EventEngine/EventData.h"

//STL
#include <vector>
using namespace std;

class OltSubmitRes
	: public EventData
{
public:
	OltSubmitRes(const CallID & cid, const vector<int> & result_vec); 
	~OltSubmitRes(void);

	const EDType GetType() const;

	const vector<int> & GetResultVec() const
	{
		return m_result_vec;
	};

private:

	vector<int> m_result_vec;

};

#endif //OLT_SUBMIT_RES_H

