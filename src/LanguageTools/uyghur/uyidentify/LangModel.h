#ifndef LANGMODEL_H
#define LANGMODEL_H

#include <string>
#include <vector>

using namespace std;

class CLangModel
{
public:
    CLangModel();
    ~CLangModel();
    bool Initialize(const string &lmFile, const int &order);
    double AtomLMProb(const string &cur, const string &context);
    double SeqLMProb(const string &seq);
	double SeqLMProb(const vector<string> &spseq);
private:
    int m_nLMOrder;
    void *m_pLM;
};

#endif
