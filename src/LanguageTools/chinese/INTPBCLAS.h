#ifndef INTPBCLAS_H
#define INTPBCLAS_H

#include <string>
#include <set>

using namespace std;

class CINTPBCLAS
{
	public:
		CINTPBCLAS():INT_gen(NULL) {};
		~CINTPBCLAS();
		bool Initialize(const string & path, const string &tag, const double ddict = 0.0);
		bool INTGenerate(const string &src,string &best, const string &tag,const set<string> &useDict);
		bool INTGenerate(const string &src,string &best, const string &tag)
        {
            set<string> dict;
            return INTGenerate(src, best, tag, dict);
        }
        bool InsertEntry(const string &sword,set<string> &useDict);
		bool DeleteEntry(const string &sword,set<string> &useDict);

		bool InsertEntry(const string &sword)
		{
			set<string> dict;
			return InsertEntry(sword, dict);
		}
		bool DeleteEntry(const string &sword)
		{
			set<string> dict;
			return DeleteEntry(sword, dict);
		}
		
	private:
		void * INT_gen;
};

#endif

