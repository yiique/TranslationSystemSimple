#if !defined (_STRING_TOKENIZER_)
#define _STRING_TOKENIZER_


class StringTokenizer{
	  int length;
	  char  delimit[100];
	  char except[4];
	  int kind;
public : 	
	StringTokenizer(const char *str0);
	StringTokenizer(const char *str0, const char c);
	StringTokenizer(const char *str0,  const  char *delimit);
	StringTokenizer(const char *str0,  const  char *delimit, const char *except);
	~StringTokenizer();
	const char *nextToken();
	char  *str;
    int index;
};

class StringTokenizerWith2Bracket{
  char  *str;
  int index;
  int length;
public : 	
	StringTokenizerWith2Bracket(const char *str0);
	~StringTokenizerWith2Bracket();
	const char *nextToken();
};

class StringTokenizerByStr{
  char  *str;
  int index;
  int length;
  char  *m_tokenstr;
  int    m_tokenlen; 
  const char *m_oristr;
public : 	
	StringTokenizerByStr(const char *str0, const char *tokenstr);
	~StringTokenizerByStr();
	const char *nextToken();
};


class StringTokenizerWithBracket{
  char  *str;
  int index;
  int length;
  char m_open;
  char m_close;
public : 	
	StringTokenizerWithBracket(const char *str0, char c_open, char c_close);
	~StringTokenizerWithBracket();
	const char *nextToken();
};

#endif
