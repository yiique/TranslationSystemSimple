#ifndef ZDQ_TOKEN_H
#define ZDQ_TOKEN_H
#include <string>

using namespace std;

class ZTokenizer
{
public:
    ZTokenizer(): mp_token(NULL) {};
    ~ZTokenizer();
    
    bool Initialize(const string & re_file_path, const string abbr_file_path);
    bool Token(const string &src, string &tgt);
private:

    void * mp_token;

};

#endif 


