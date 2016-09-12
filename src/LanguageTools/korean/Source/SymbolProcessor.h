// SymbolProcessor.h: created by Kim Chol Su
//
//////////////////////////////////////////////////////////////////////

#if !defined _SYMBOLPROCESSOR_H_
#define _SYMBOLPROCESSOR_H_

class CSymbolProcessor  
{
public:
	CSymbolProcessor();
	virtual ~CSymbolProcessor();
	bool check_symbol(char *source, int* tagnum, int* len);
	int get_mophem_token(const char *input, char* token);
	int get_symbol_tag(char *pInput, int bflag);
	bool check_F_E_symbol(char* pword);
	int separate_sym(char* source, char* target, int *nFlag);
	bool possible_sep(char *pInput, bool byte1=true);
	void insert_space(char* source);
	void proc_string(char* source);
	void replace_str(char *src, char *old_str, char *new_str);
private:
	int discriminate_char(char* csChar, bool check_EngUppLow=NULL);

};

#endif 
