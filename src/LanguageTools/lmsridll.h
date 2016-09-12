#ifndef __LMSRCDLL_H
#define __LMSRCDLL_H
#define _WIN32
//#define __GNUG__
//#define _LMUSER_
//#define _LMDOMAIN_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
//��ı�׼�������� DLL �е������ļ��������������϶���� LMSRIDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
//�κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ 
// LMSRIDLL_API ������Ϊ�ǴӴ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef _WIN32
#define LMSRIDLL_EXPORTS
#ifdef LMSRIDLL_EXPORTS
#define LMSRIDLL_API __declspec(dllexport)
#else
#define LMSRIDLL_API __declspec(dllimport)
#endif
#else
#define LMSRIDLL_API
#endif
#include <string>
#include <vector>
using namespace std;

typedef basic_string<int> LmStr;

extern "C" {

// ��������ģ��
// fn: �ļ�����֧�ֵ��ļ����������֣���ͨ�ı��ļ���gzipѹ���ļ�(��չ��Ϊgz���ļ�)
// arpa: ARPA��ʽ��BIN��ʽ��0ΪBIN��ʽ����0ΪARPA��ʽ
// ��������ģ�͵�ָ��
//LMSRIDLL_API void * sriLoadLM(const char * fn, int arpa=0);
/**
*load language model
*@param fn
*   file name(supporting two file types: common and gz)
*@param arpa
*   0 for BIN and 1 for ARPA
*@param order
*    order, default is 3
*@param unk
*   save<unk>, default is unsaved
*@param tolow
*   transform all letters to small letters
*@return pointer of language model
*/    
LMSRIDLL_API void * sriLoadLM(const char * fn, int arpa=0, int order=3, int unk=0, int tolow=0);

// �趨���Ԫ��
// plm: ����ģ�͵�ָ��
// o: �µ�Ԫ����С�ڵ���0ʱ��Ч(����)����������ģ�����ݵ�Ԫ��ʱ������ģ�����ݵ�Ԫ������
// ����ԭ����Ԫ��
LMSRIDLL_API int sriSetOrder(void * plm, int o);

// ж������ģ��
// plm: ����ģ�͵�ָ��
LMSRIDLL_API void sriUnloadLM(void * plm);

// ������������
// plm: ����ģ�͵�ָ��
// sentence: ����
// ���ؾ��ӵ������
//     perplexity(a1 a2 a3 ... an)=log10(P(a1)*P(a2|a1)*P(a3|a1a2)*...*P(an|a1...an-1))
LMSRIDLL_API double sriPerplexity(void * plm, const char * sentence);

// ����n-gram
// plm: ����ģ��ָ��
// ����ֵ: log10(P(word|context))
LMSRIDLL_API double sriWordProb(void * plm, const char * word, const char * context);
//
LMSRIDLL_API double sriWordProbInt1(void * plm, const LmStr& word);
//
LMSRIDLL_API double sriWordProbInt2(void * plm, const LmStr& word, const LmStr& context);

// ���ַ���תΪSRI�ڲ�����
LMSRIDLL_API int sriWord2Idx(void * plm, const char* word);

} // extern "C"
#endif
