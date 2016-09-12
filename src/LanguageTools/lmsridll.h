#ifndef __LMSRCDLL_H
#define __LMSRCDLL_H
#define _WIN32
//#define __GNUG__
//#define _LMUSER_
//#define _LMDOMAIN_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 下列 ifdef 块是创建使从 DLL 导出更简单的
//宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 LMSRIDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
//任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将 
// LMSRIDLL_API 函数视为是从此 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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

// 载入语言模型
// fn: 文件名，支持的文件类型有两种：普通文本文件和gzip压缩文件(扩展名为gz的文件)
// arpa: ARPA格式和BIN格式，0为BIN格式，非0为ARPA格式
// 返回语言模型的指针
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

// 设定最大元数
// plm: 语言模型的指针
// o: 新的元数，小于等于0时无效(不变)，大于语言模型数据的元数时按语言模型数据的元数计算
// 返回原来的元数
LMSRIDLL_API int sriSetOrder(void * plm, int o);

// 卸载语言模型
// plm: 语言模型的指针
LMSRIDLL_API void sriUnloadLM(void * plm);

// 计算句子困惑度
// plm: 语言模型的指针
// sentence: 句子
// 返回句子的困惑度
//     perplexity(a1 a2 a3 ... an)=log10(P(a1)*P(a2|a1)*P(a3|a1a2)*...*P(an|a1...an-1))
LMSRIDLL_API double sriPerplexity(void * plm, const char * sentence);

// 计算n-gram
// plm: 语言模型指针
// 返回值: log10(P(word|context))
LMSRIDLL_API double sriWordProb(void * plm, const char * word, const char * context);
//
LMSRIDLL_API double sriWordProbInt1(void * plm, const LmStr& word);
//
LMSRIDLL_API double sriWordProbInt2(void * plm, const LmStr& word, const LmStr& context);

// 将字符串转为SRI内部整型
LMSRIDLL_API int sriWord2Idx(void * plm, const char* word);

} // extern "C"
#endif
