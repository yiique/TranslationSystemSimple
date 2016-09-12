#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "LangModel.h"

#ifdef WINDOWS
	#include "lmsridll.h"
#else 
	#include "flmsri.h" 
#endif 
/**
   *construct CLangModel
   */
CLangModel::CLangModel()
{
    m_pLM = 0;
}

/**
   *destruct CLangModel
   */
CLangModel::~CLangModel()
{
    if (m_pLM)
    {
        sriUnloadLM(m_pLM);
    }
}

/**
   *load language model
   *@param lmFile
   *     where the gz files are stored
   * @order-gram
   *@return whether intialization is successful
   */
bool CLangModel::Initialize(const string &lmFile, const int &order)
{
    m_nLMOrder = order;
    m_pLM = sriLoadLM(lmFile.c_str(), 1, order, 1, 0);
    return m_pLM != NULL;
}

/**
	*calculate the prob of atom a1a2....an (an|a1 a2 a3....)
	*@param cur
	*     element an
	*@param context
	*    element a1,a2,a3
	*@return result prob
    */
   
double CLangModel::AtomLMProb(const string &cur, const string &context)
{
    if (cur.empty() || context.empty())
    {
        return 0.0;
    }
    return sriWordProb(m_pLM, cur.c_str(), context.c_str());
}

/**
	*calculate the prob of seq
	*@param seq
    *     a1 a2....
	*@return result prob divided by byte
    */
double CLangModel::SeqLMProb(const string &seq)
{
	int i;
    int len = int(seq.length());
    char *templet = new char[len + 1];
	char *nowchar=new char[2];
    for(i=0;i<=len;i++)
	{
		templet[i]='\0';
	}
	nowchar[1]='\0';
    templet[0]=seq[0];
    
    int wordCnt = len;

    double prob = 0.0;
    int headCnt = min(m_nLMOrder - 1, wordCnt);
    for (i = 1; i < headCnt; ++i)
    {
		nowchar[0]=seq[i];
        prob += AtomLMProb(nowchar,templet);
		templet[i]=seq[i];
    }
    for (i = headCnt; i < wordCnt; ++i)
    {
		nowchar[0]=seq[i];
        prob += AtomLMProb(nowchar, templet + i - m_nLMOrder + 1);
        templet[i]=seq[i];
    }

	delete []nowchar; //fcl fix 2011-4-29
    delete []templet;
    return prob;
}
//����������ģ�͸���///(����Ϊ�ÿո������һ�����ʣ����⿼�ǵ��ʣ������n-gramģ���¸õ��ʵĸ���)
///double CLangModel::SeqLMProb(const string &seq)//delete*
double CLangModel::SeqLMProb(const vector<string> &spseq)
{
    ///int len = int(seq.length());//delete*
	int len = int(spseq.size());
    ///char *templet = new char[len + 1];///��β�������־��--���ĳ�vector<string> *templet�ɷ�//delete*

    ///strcpy(templet, seq.c_str());///stringת��Ϊchar������һ������Ԫ�ش�һ���ַ���������� �������������������splitseq�ĺ���//delete*

    ///vector<int> vPos;//delete*

    ///vPos.push_back(0);//delete*

    int i;
	int j;
	string pre="<s>";

    ///for (i = 0; i < len; ++i)//delete*
    ///{
    ///    if (templet[i] == ' ')//delete*
    ///    {
    ///        vPos.push_back(i + 1);///��¼�ո����ڵ��ַ������������ţ���1��ʼ����//delete*
    ///    }//delete*
    ///}//delete*

	///int wordCnt = int(vPos.size());//delete* ������<s>��</s>�Ĵʵĸ���

    ///vPos.push_back(len + 1);///vPos.size()-1=�ʸ�/��׺����Ŀ(��һ��0����)//delete*

    double prob = 0.0;
    ///int headCnt = min(m_nLMOrder - 1, wordCnt);///��ͷʱҪ�����Ƚ�//delete*
    int headCnt = min(m_nLMOrder - 1, len);

	///�ӵ�һ����<s>��ʼ����������n-gram��n�������Ʃ�磬<s> a b </s>������Ԫ�﷨ģ����������ѭ�������⴦��P(a|<s>)��P��b|<s> a��
    for (i = 1; i < headCnt; ++i)///��headCnt=2...
    {
        ///templet[vPos[i] - 1] = '\0';///��������ԭ���Ŀ��ַ�' 'ת��\0������templetʱ���˽���//delete*
        ///templet[vPos[i + 1] - 1] = '\0';//delete*
		///��֤����AtomLMProb�Ļ���string:
		pre="<s>";
		for(j = 1;j < i; ++j)
		{
		    pre += ' ' + spseq[j];
	    }
        prob += AtomLMProb(spseq[i], pre);
		///prob += AtomLMProb(templet + vPos[i], templet);///ǰһ��/ǰһ~���� �ʸ�/��׺������ ��ǰ�Ĵʸ�/��׺�ĸ���(�����Ǵ���ͷ���ĸ�����)templet + vPos[i]:Խ����i���ո�~��һ���ո�֮��//delete*
        ///templet[vPos[i] - 1] = ' ';//delete*
        ///templet[vPos[i + 1] - 1] = ' ';///��ԭ//delete*
    }
    
	///�������ģ�
    ///for (i = headCnt; i < wordCnt; ++i)//delete*
	for (i = headCnt; i < len; ++i)
    {
		pre=spseq[i-m_nLMOrder+1];
        ///templet[vPos[i] - 1] = '\0';//delete*
        ///templet[vPos[i + 1] - 1] = '\0';//delete*
        ///��֤����AtomLMProb�Ļ���string:
		for(j = i-m_nLMOrder+2; j < i; ++j)  ///<s> a b </s>
		{
		    pre += " " + spseq[j];
	    }

        prob += AtomLMProb(spseq[i], pre);
        ///prob += AtomLMProb(templet + vPos[i], templet + vPos[i - m_nLMOrder + 1]);//delete*
        ///templet[vPos[i] - 1] = ' ';//delete*
        ///templet[vPos[i + 1] - 1] = ' ';//delete*
    }

    ///delete []templet;//delete*
    return prob;
}
