// Tag.h: interface for the Tag class.
//
//////////////////////////////////////////////////////////////////////
#ifndef  _TAG_H_
#define  _TAG_H_

#define TAG_NUM 64

enum TAG_ID{T_NNC=1, T_NNR, T_NNX, T_NNB, T_NNU, T_NNP, T_NUM, 
			T_VVB, T_VAJ, T_VXV, T_VXA, T_PNN, T_ADV, T_ADC, T_IJC, T_LST,
			T_TCS, T_TCM, T_TCO, T_TCP, T_TCD, T_TCL, T_TCQ, T_TCV, T_TCC, T_TXP, T_TPL, T_TND,
			T_TDN, T_TDC, T_TDR, T_TDT, T_TDM, T_TDX, T_TDV, T_TDJ, T_TDS, T_TEN,
			T_PFN, T_PFU, T_PFV, T_PFA, T_SFN, T_SFU, T_SFV, T_SFA, T_SFD, 
			T_SEN, T_SCM, T_SLG, T_SQL, T_SQR, T_SUT, T_SWT, T_SNM, T_NON, T_VXD, T_VBT, T_VAT, T_VXT};

const char tag_name[TAG_NUM+1][5]={"", "NNC", "NNR", "NNX", "NNB", "NNU", "NNP", "NUM",
				 "VVB", "VAJ", "VXV", "VXA", "PNN", "ADV", "ADC", "IJC", "LST",
				 "TCS", "TCM", "TCO", "TCP", "TCD", "TCL", "TCQ", "TCV", "TCC", "TXP", "TPL", "TND",
				 "TDN", "TDC", "TDR", "TDT", "TDM", "TDX", "TDV", "TDJ", "TDS", "TEN",
				 "PFN", "PFU", "PFV", "PFA", "SFN", "SFU", "SFV", "SFA", "SFD", 
				 "SEN", "SCM", "SLG", "SQL", "SQR", "SUT", "SWT", "SNM", "NON", "VXD", "VBT", "VAT", "VXT"};
const char tag_short_name[TAG_NUM+1][5]={"", "n", "n", "n", "n", "n", "n", "n",
				 "v", "v", "v", "v", "v", "a", "a", "a", "s",
				 "j", "j", "j", "j", "j", "j", "j", "j", "j", "j", "l", "v",
				 "e", "e", "e", "e", "e", "e", "e", "e", "e", "e",
				 "p", "p", "p", "p", "p", "p", "p", "p", "p", 
				 "s", "s", "s", "s", "s", "s", "s", "s", "n", "v", "v", "v", "v"};

typedef struct  
{
	int main_tag;
// 	int count;
	int tags[50];
}TAG_COMB;

const TAG_COMB tag_comb[TAG_NUM] = {
		{T_NNC, /*20,*/ {T_NNB, T_NNX, /*T_VVB, T_VAJ,*/ T_TND, T_TCS, T_TCM, T_TCO, T_TCL, T_TCD, T_TCP, T_TCV, T_TCC, T_TXP, T_TPL,T_SFN, T_SNM, T_SFV, T_SFA,T_SFD, T_SEN, T_SCM, T_SQL, T_SQR, T_SUT, T_SWT, T_TEN,0}},
		{T_NNR, /*18,*/ {T_NNC, T_NNB, T_NNX, T_TND, T_TCS, T_TCM, T_TCO, T_TCL, T_TCD, T_TCP, T_TCV, T_TCC, T_TXP, T_TPL,/*T_SFN,*/ T_SEN, T_SCM, T_SQL, T_SQR, T_SWT, T_SNM, 0}},
		{T_NNX, /*16,*/ {T_TND, T_TCS, T_TCM, T_TCO, T_TCL, T_TCD, T_TCP, T_TCC, T_TXP, T_TPL,/* T_SFN,*/ T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_NNB, /*16,*/ {T_TND, T_VAJ, T_TCS, T_TCM, T_TCO, T_TCL, T_TCD, T_TCP, T_TCV, T_TCC, T_TXP, T_TPL, /*T_SFN,*/ T_SFV, T_SFA, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_NNU, /*17,*/ {T_TND, T_TCS, T_TCM, T_TCO, T_TCL, T_TCD, T_TCP, T_TCV, T_TCC, T_TXP, T_TPL, T_SFN, /*T_SFV, T_SFA,*/ T_SEN, T_SCM, T_SQL, T_SQR,T_NNX,0}},
		{T_NNP, /*17,*/ {T_TND, T_TCS, T_TCM, T_TCO, T_TCL, T_TCD, T_TCP, T_TCV, T_TCC, T_TXP, T_TPL, /*T_SFN, T_SFV, T_SFA,*/ T_SEN, T_SCM, T_SQL, T_SQR,0}},
		{T_NUM, /*15,*/ {/*T_NNC, T_NNB, T_NNX,*/ T_NNU, T_NUM, T_TND, T_TCS, T_TCM, T_TCO, T_TCL, T_TCD, T_TCP, T_TCV, T_TCC, T_TXP, T_TPL, T_SFU, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_VVB, /*11,*/ {T_TDN, T_TDR, T_TDT, T_TDC, T_TDS, T_TDJ, T_TDX, T_TDM, T_TEN, T_SQL, T_SQR, 0}},
		{T_VAJ, /*12,*/ {T_TDN, T_TDR, T_TDT, T_TDC, T_TDS, T_TDJ, T_TDV, T_TDX, T_TDM, T_TEN, T_SQL, T_SQR, 0}},
		{T_VXV, /*11,*/ {T_TDN, T_TDR, T_TDT, T_TDS, T_TDJ, T_TDX, T_TDM, T_TEN, T_SQL, T_SQR, 0}},
		{T_VXA, /*11,*/ {T_TDN, T_TDR, T_TDT, T_TDV, T_TDS, T_TDJ, T_TDX, T_TDM, T_TEN, T_SQL, T_SQR, 0}},
		{T_TND, /*11,*/ {T_TDN, T_TDR, T_TDT, T_TDS, T_TDJ, T_TDX, T_TDM, T_TEN, T_SQL, T_SQR, 0}},
		{T_PNN, /*9,*/  {T_SCM, T_SQL, T_SQR, 0}},
		{T_ADV, /*5,*/  {T_TXP, T_VVB, T_NUM, T_VAJ, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_ADC, /*4,*/  {T_TXP, T_SEN, T_SQL, T_SQR, 0}},
		{T_IJC, /*3,*/  {T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_LST, /*3,*/  {T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TCS, /*4,*/  {T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TCM, /*4,*/  {T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TCO, /*4,*/  {T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TCP, /*4,*/  {T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TCD, /*4,*/  {T_TXP, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TCL, /*5,*/  {T_TCC, T_TCP, T_TXP, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TCQ, /*3,*/  {T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TCV, /*4,*/  {T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TPL, /*9,*/  {T_NNC, T_NNB, T_NNX, T_TND, T_TCS, T_TCO, T_TCL, T_TCP, T_TCC, T_TXP, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TXP, /*9,*/  {/*T_VXV,*/ T_TCS, T_TCO, T_TCL,T_TCD, T_TCP, T_TXP, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TCC, /*4,*/  {T_TCP, T_TXP, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TDR, /*11,*/ {T_TDN, T_TDT, T_TDS, T_TDJ, T_TDX, T_TDM, T_TEN, T_SQL, T_SQR, 0}},
		{T_TDT, /*11,*/ {T_TDN, T_TDS, T_TDJ, T_TDM, T_TEN, T_SQL, T_SQR, 0}},
		{T_TDJ, /*6,*/  {T_VVB, T_VAJ, T_TXP, T_SCM, T_SQL, T_SQR, 0}},
		{T_TDS, /*6,*/  {T_VXV, T_VXA, /*T_VVB,*/ T_TXP, T_SCM, T_SQL, T_SQR, 0}},
		{T_TDX, /*6,*/  {T_VVB, T_VXV, T_VXA, T_TXP, T_SCM, T_SQL, T_SQR, 0}},
		{T_TDN, /*11,*/ {/*T_NNC, T_NNB, T_NNX,*/ T_VAJ, T_TCS, T_TCO, T_TCL, T_TCD, T_TCP, T_TXP, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TDM, /*4,*/  {T_NNB,/*T_NNC,  T_NNX, */T_SCM, T_SQL, T_SQR,T_TEN, 0}},
		{T_TDV, /*5,*/  {T_TXP, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_TEN, /*4,*/  {T_TXP,T_TDS, /*T_TEN,*/ T_TDJ, T_TDM, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
// 		{T_TEQ, /*4,*/  {T_TEP, T_TCO, T_TDJ, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
// 		{T_TEI, /*4,*/  {T_TEP, T_TDJ, T_TDM, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
// 		{T_TEA, /*4,*/  {T_TEP, T_TDJ, T_TDM, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_PFN, /*7,*/  {/*T_NNC,*/ 0}},
		{T_PFU, /*7,*/  {T_NUM, T_SNM, 0}},
		{T_PFV, /*6,*/  {T_VVB, 0}},
		{T_PFA, /*6,*/  {T_VAJ, 0}},
		{T_SFN, /*14,*/ { /*T_NNC,T_NU,T_VAJ, T_NNX, */ T_TND, T_TCS, T_TCM, T_TCO, T_TCL, T_TCP, T_TCV, T_TCP, T_TCC, T_TXP, T_TPL, T_SFV, T_SCM, T_SLG, T_SQL, T_SQR, 0}},
		{T_SFU, /*14,*/ {T_NNC, T_NNU, T_NUM, T_TND, T_TCS, T_TCM, T_TCO, T_TCL, T_TCP, T_TCV, T_TCP, T_TCC, T_TXP, T_SCM, T_SQL, T_SQR, 0}},
		{T_SFV, /*11,*/ {T_TDN, T_TDR, T_TDT, T_TDC, T_TDS, T_TDJ, T_TDX, T_TDM, T_TEN, T_SQL, T_SQR, 0}},
		{T_SFA, /*11,*/ {T_TDN, T_TDR, T_TDT, T_TDC, T_TDS, T_TDJ, T_TDV, T_TDX, T_TDM, T_TEN, T_SQL, T_SQR, 0}},
		{T_SFD, /*3,*/  {T_SCM, T_SQL, T_SQR, 0}},
		{T_SEN, /*4,*/  {T_TND, T_SEN, T_SCM, T_SQL, T_SQR, 0}},
		{T_SCM, /*3,*/  {T_SQL, T_SQR, 0}},
		{T_SQL, /*2,*/  {T_ADC, T_ADV, T_TND, T_NNC, T_NNR, T_NUM, T_SWT, T_SLG, T_SNM, 0}},
		{T_SQR, /*2,*/  {T_NNB, T_TND, T_TCS, T_TCP, T_TCL, T_TCC, T_TCO, T_TCQ, T_TXP, T_TDJ, T_TDM, 0}},
		{T_SLG, /*4,*/  {T_NNC, T_NNB, T_NNX, T_SFN, T_SFV, T_TND, T_TCP, T_TCL, T_TCC, T_TCO, T_SEN, T_SCM, T_SQL, T_SQR, T_TXP, 0}},
		{T_SUT, /*4,*/  {T_NNC, T_NNX, T_TND, T_TXP, T_TCP, T_TCS, T_TCL, T_TCC, T_TCO, T_SFN, T_SCM, T_SQL, T_SQR, 0}},
		{T_SNM, /*9,*/  {T_NNC, T_NNU, T_NNB, T_NNX, T_NUM, T_TCP, T_TND, T_SFU, /*T_SFN,*/ T_SCM, T_SQL, T_SQR, T_SUT, T_SWT, T_TCL,T_TXP,0}},
		{T_SWT, /*8,*/  {T_NNC, T_TND, T_SCM, T_SNM, T_SQL, T_SQR, T_TCO, T_TCP, T_TEN, T_TXP, T_SFV, T_TCL,0}},/* by hhj-17 T_TCO, T_TEN, T_TXP, T_TCL ¾¡°¡ end*/
		{0, {0}}
};

inline bool check_comb(int tag1, int tag2)
{
// 	if(tag1 >= T_NF && tag1 <= T_NA)
// 		return true;
	if (tag1==0)
	{
// 		if(/*tag2==T_NNU || tag2==T_NNB || */(tag2>=T_JKS && tag2<=T_ETD) || (tag2>=T_XSN && tag2<=T_XSA) || tag2>=T_EC)
		if((tag2>=T_TCS && tag2<=T_TEN) || (tag2>=T_SFN && tag2<=T_SFD))
			return false;
		else
			return true;
	}
	if (tag2==T_SCM)
		return true;
	if (tag1==T_SQR||tag1==T_SQL||tag2==T_SQR||tag2==T_SQL)
		return true;

	if (tag2 == 0)
	{
		if(tag1 != T_VVB && tag1 != T_VAJ && tag1 != T_TND)
			return true;
	}
	
	for (int i=0; i<TAG_NUM; i++)
	{
		if(tag_comb[i].main_tag==0 || tag_comb[i].main_tag!=tag1)
			continue;
		for(int j=0; tag_comb[i].tags[j]!=0; j++)
		{
			if(tag2 == tag_comb[i].tags[j])
				return true;
		}
	}
// 	for (int i = 0; i < tag_comb[tag1-1].tags[i]!=0; i++)//(int i = 0; i < tag_comb[tag1-1].count; i++)
// 	{
// 		if(tag2 == tag_comb[tag1-1].tags[i])
// 			return true;
// 	}
	return false;
}

inline int  get_tagid(char *str)
{
	int i;
	for(i = 0; i<TAG_NUM+1; i++)
	{
		if(strcmp(str, tag_name[i]) == 0)
		{
			if(i == 0)
				int ii=0;
			return i;
		}
	}
	return 0;
}

#endif

