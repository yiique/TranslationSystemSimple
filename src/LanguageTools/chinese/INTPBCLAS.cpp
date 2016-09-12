#include "INTPBCLAS.h"
#include "PBCLAS.h"

CINTPBCLAS::~CINTPBCLAS()
{
        delete (CPBCLAS*) INT_gen;
}

bool CINTPBCLAS::Initialize(const string &path, const string &tag, const double ddict)
{
        CPBCLAS * pb_gen = new CPBCLAS();

        INT_gen = (void *) pb_gen;

		if(false == pb_gen->Initialize(path, tag,ddict))
        {
			delete pb_gen;
            return false;
        }
		else
            return true;
}

bool CINTPBCLAS::INTGenerate(const string &src, string &best, const string &tag, const set<string> &useDict)
{
        if(!INT_gen) 
			return false;

        CPBCLAS * pb_gen = (CPBCLAS*) INT_gen;

		pb_gen->Generate(src, best, tag, useDict);

		return true;
}

bool CINTPBCLAS::InsertEntry(const string &sword, set<string> &useDict)
{
        if(!INT_gen)
            return false;

        CPBCLAS * pb_gen = (CPBCLAS*) INT_gen;

        pb_gen->InsertEntry(sword,useDict);

        return true;
}

bool CINTPBCLAS::DeleteEntry(const string &sword, set<string> &useDict)
{
        if(!INT_gen)
            return false;

        CPBCLAS * pb_gen = (CPBCLAS*) INT_gen;

        pb_gen->DeleteEntry(sword,useDict);

        return true;
}
