// KMAnalyser.h: interface for the KMAnalyser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _KMANALYSER_H_
#define _KMANALYSER_H_

#include "Kma_Data.h"
#include "SymbolProcessor.h"
#include "Dictionary.h"


#define MAX_VI_DIC 8000
#define MAX_PRE_DIC 6472
enum sen_type{sentence, word_noun, word_dec, title, word_test, err_sen};
#define MAX_ADV_DIC 15000

class CompoundAnalysis;
class COptionSet;
class Zamo;
class CProbComputer;
class KUnicode; 



class KMAnalyser  
{
public:
	KMAnalyser();
	~KMAnalyser();

public:
	bool	create(const char* ini_file);

	void	proc_sen(const char *str);
	char	m_res_buff[MAX_SEN_LENGTH*3];
	int		m_max_id;
	int		pre_eojol_last_tagid;
	int		m_word_count;

private:
    KMA_SEN  *m_pKmaSen;
    CompoundAnalysis	*m_pCompoundAnalyer;
	COptionSet			*m_pOptionSet;
	Zamo				*m_pZamo;
	CProbComputer    *m_pProbComputer;
	MorphDic		*m_pDic;

	bool	m_single;
	int		m_sen_type;//0:real_sen, 1:word_noun, 2:word_dec, 3:title

	bool	open_dictionary(char *dicpath);
	void	initialize();
	void	pre_proc(char *pInputSen);
	bool	check_indep_word(char *str);
	bool	check_mixed_word(char *str);
	bool	load_assist_dic(const char *path, int kind);

	void	proc_word(char *str, KMA_WORD *word);
	bool	proc_pre_anal_dic(char *str);
	bool	proc_except(char *str);
	bool	proc_error(char *str);

	void	analyse_word(char *str, int pre_idx, int pos, bool is_err=false, bool is_one=false);
	int		proc_compound(char *str, int pos, int pre_idx, int *end_idx, bool is_sub=false, bool first_try=true, bool is_mixed=false);
	int		proc_mixed_word(char *str, int pre_idx = -1, bool is_sub=false, bool is_compound=false);
	void	proc_other(char *str);
	bool	proc_noun(char *str1, char *str2, int tag1, int tag2, bool is_noun=false,int cur_idx=-1);

	bool	check_comb_str(int tag1, int tag2, char *str1, char *str2, int level=1);
	bool	check_tail(char *str, int pre_tag, int pos);
	int		make_candi(char *str, int tag_id, int pos, int pre_idx, bool is_end=false, bool is_compound=false);
	int		make_sub_result(char *str, char *tag_list);
	void	make_result(int type);
	bool	find_tag(char *str, int tag, MorphRec *recInfo, int kind);
	bool	check_list(char *str);
	bool	check_comb_to(char *szPrevWord, char *szCurrWord);


	int		find_dic_res(char *str, int pos, int kind, int *offset=NULL);
	bool	find_bound(char *str);

	void	change_result(int wrd_id,int res_id, int kind, char *str);
	bool	filter_result();
	void	print_anal_result();
	void	print_anal_multi_result();
	void	make_string();

	void	compute_whole_path_weight(int eojolID);
	int		get_eojol_prob();
	int		get_last_max_ID();
	void	get_good_path(int ID);
	bool	analyse_xr(char *str);
	int		sort_path(int *res);
	int		get_num_str(char *str);


	KMA_WORD *m_word;
	char	m_input[MAX_WORD_LENGTH];
	const char*	m_pInputSen;

	KMA_CANDI m_candis[MAX_CANDI];
	int		m_candi_co;
	int		m_end_idx[100];
	int		m_end_co;

	int		m_tail_pos[10];
	int		m_tail_val[10];
	int		m_tail_co;

	char	m_sepa_str[20][MAX_TOKEN_LENGTH];
	int		m_sepa_pos[20];
	int		m_sepa_flags[20];
	int		m_sepa_co;

	int		m_good_values[10];

	DIC_KEY_VAL m_vi_dic[MAX_VI_DIC];
	PRE_DIC m_pre_dic[MAX_PRE_DIC];
	DIC_KEY_VAL m_adv_dic[MAX_ADV_DIC];
	XrDic	m_xr_dic;
	MorphRec m_dic_res[MAX_TOKEN];
	
	int		m_vi_dic_co;
	int		m_pre_dic_co;
	int		m_adv_dic_co;
	int		m_dic_res_co;
	int		m_offset_res[MAX_WORD_LENGTH][50];
	int		m_offset_res_co[MAX_WORD_LENGTH];

	CSymbolProcessor m_SymProcer;
	KUnicode*	m_pUnicode;

	int			m_nInputCodeConvMode;
	int			m_nOutputCodeConvMode;
	int			code_convert(char* szSource, char* szTarget, int nConvMode);

};

#endif 
