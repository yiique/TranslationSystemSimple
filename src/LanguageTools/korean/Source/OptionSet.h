// OptionSet.h: interface for the COptionSet class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _OPTIONSET_H_
#define _OPTIONSET_H_

#define OPTION_NUM 16
const char option_str[OPTION_NUM][50]={"DicDirPath", "InputMode", "OutputMode","AmbigResolution", 
									 "CompNounResolution","CodeInput", "CodeOutput", "NumeralConversion","ChangeLetter1",  
									 "ChangeLetter2", "DeleteLetter", "ChangeLetter3", "AddLetter", "SynLetter","TagMode","SymMode"};
enum OPTION_KIND{dic_path, input_mod, output_mod, ambi_res, compnoun_res,code_input, code_output, num_converse, 
				change_letter1, change_letter2, delete_letter, change_letter3, add_letter, syn_letter, tag_mode, sym_mode};

class COptionSet  
{
public:
	COptionSet();
	~COptionSet();
	
	int m_outputMode;
	int m_inputMode;
	char m_dicPath[MAX_PATH];
	int m_ambigRes;
	int m_compNounRes;
	int m_code_input;
	int m_code_output;
	int m_numberConverse;
	int m_change_letter1;
	int m_change_letter2;
	int m_change_letter3;
	int m_add_letter;
	int m_delete_letter;
	int m_syn_letter;
	int m_tag_mode;
	int m_sym_mode;
	bool load(const char *path);

private:
	int get_option_kind(const char* str);
};

#endif 
