// Kma_DLL.h: interface for the Kma_DLL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined _KMA_DLL_H_
#define _KMA_DLL_H_

class KMA_DLL  
{
public:
	KMA_DLL();
	virtual ~KMA_DLL();
	char* proc_sen(const char *sen);
	bool create(const char* ini_file);

};

#endif
