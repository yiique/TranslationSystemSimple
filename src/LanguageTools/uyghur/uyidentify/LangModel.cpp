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
//词序列语言模型概率///(输入为用空格隔开的一个单词，纯粹考虑单词，计算出n-gram模型下该单词的概率)
///double CLangModel::SeqLMProb(const string &seq)//delete*
double CLangModel::SeqLMProb(const vector<string> &spseq)
{
    ///int len = int(seq.length());//delete*
	int len = int(spseq.size());
    ///char *templet = new char[len + 1];///结尾存结束标志符--》改成vector<string> *templet可否？//delete*

    ///strcpy(templet, seq.c_str());///string转化为char【】，一个数组元素存一个字符，这里估计 会出问题啦！！！加上splitseq的函数//delete*

    ///vector<int> vPos;//delete*

    ///vPos.push_back(0);//delete*

    int i;
	int j;
	string pre="<s>";

    ///for (i = 0; i < len; ++i)//delete*
    ///{
    ///    if (templet[i] == ' ')//delete*
    ///    {
    ///        vPos.push_back(i + 1);///记录空格所在的字符在数组里的序号（从1开始数起）//delete*
    ///    }//delete*
    ///}//delete*

	///int wordCnt = int(vPos.size());//delete* 包括了<s>和</s>的词的个数

    ///vPos.push_back(len + 1);///vPos.size()-1=词干/词缀的数目(第一个0不算)//delete*

    double prob = 0.0;
    ///int headCnt = min(m_nLMOrder - 1, wordCnt);///数头时要经过比较//delete*
    int headCnt = min(m_nLMOrder - 1, len);

	///从第一个非<s>开始，计算少于n-gram的n的情况，譬如，<s> a b </s>，在四元语法模型里，在下面的循环中特殊处理P(a|<s>)，P（b|<s> a）
    for (i = 1; i < headCnt; ++i)///当headCnt=2...
    {
        ///templet[vPos[i] - 1] = '\0';///？？？把原来的空字符' '转成\0，调用templet时到此结束//delete*
        ///templet[vPos[i + 1] - 1] = '\0';//delete*
		///保证传给AtomLMProb的还是string:
		pre="<s>";
		for(j = 1;j < i; ++j)
		{
		    pre += ' ' + spseq[j];
	    }
        prob += AtomLMProb(spseq[i], pre);
		///prob += AtomLMProb(templet + vPos[i], templet);///前一个/前一~二的 词干/词缀出现下 当前的词干/词缀的概率(不考虑纯粹头部的概率吗？)templet + vPos[i]:越过第i个空格~下一个空格之间//delete*
        ///templet[vPos[i] - 1] = ' ';//delete*
        ///templet[vPos[i + 1] - 1] = ' ';///还原//delete*
    }
    
	///处理后面的？
    ///for (i = headCnt; i < wordCnt; ++i)//delete*
	for (i = headCnt; i < len; ++i)
    {
		pre=spseq[i-m_nLMOrder+1];
        ///templet[vPos[i] - 1] = '\0';//delete*
        ///templet[vPos[i + 1] - 1] = '\0';//delete*
        ///保证传给AtomLMProb的还是string:
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
