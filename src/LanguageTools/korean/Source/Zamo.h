
#if !defined(_ZAMO_H_)
#define _ZAMO_H_


typedef struct 
{
	char kor[2];
	char sym[3];
}ZamoElem;

class Zamo  
{
	ZamoElem *m_pTable;
	ZamoElem *m_pTable_1;
	int m_len;

public:
	Zamo();
	bool create(char *tablefile);
	virtual ~Zamo();
	bool compose(char *symbols, char *letter);
	bool decompose(char *letter, char *symbols,int count=-1);
};

#endif 
