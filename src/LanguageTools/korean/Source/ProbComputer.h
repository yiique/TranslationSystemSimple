// ProbComputer.h: interface for the CProbComputer class.
//
//////////////////////////////////////////////////////////////////////

#if!defined _PROBCOMPUTER_H_
#define _PROBCOMPUTER_H_

//#include "KMAnalyser.h"

#define MAX_MODEL_COUNT 302596


#include "Kma_Data.h"

class CProbComputer  
{
public:
	CProbComputer();
	virtual ~CProbComputer();
	bool create(char *path);
	float compute_elem_weight(KMA_RESULT* left, KMA_RESULT* right);

private:
// 	ProbDic m_prob_dic;
	KEY_VAL m_prob_model[MAX_MODEL_COUNT];
	int m_prob_model_co;
	
	float get_elem_prob(char* prevtag, char* curtag, char* curmorp, int mode);
	float get_prob_val(char *pre_tag, char *cur_tag, char *cur_mop, int mode);

#ifdef _DB
	float get_elem_prob_DB(char* prevtag, char* curtag, char* curmorp, int mode);
#endif
};

#endif
