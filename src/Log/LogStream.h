/**
* Copyright (c) 2010, ������Ϣ�ص�ʵ����,�й���ѧԺ���㼼���о���
* All rights reserved.
*
* �ļ����� LogStream.h
* @brief  ��־�࣬��װ��־������ĸ�ʽ�ͷ�ʽ
* @project ToWin���߷���ƽ̨
*
* @comment 
* @version 1.1
* @author ������
* @date 2010-06-13
* @
*/


#ifndef LOG_STREAM_H
#define LOG_STREAM_H

//MAIN
#include "Common/BasicType.h"
#include "LogPrinter.h"
#include "LogData.h"

//STL
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class LogStream
{
public:
	~LogStream(void);

	/**
	 * @brief  ��������������һ��LogStreamʵ��
	 * @return ����ֵ ��
	 */
	static LogStream CreateLogStream();

	// Typedef  ���Ͷ��壺endl, ����д<<������
	typedef basic_ostream<char, char_traits<char> >& (*EndlType)(basic_ostream<char, char_traits<char> >&);

	/**
	 * @brief ���ز�����<< ��endlʱ��Ӳ���
	 * @param[in]  endl_obj endl����
	 * @return ����ֵ ��
	 */
	LogStream & operator << (EndlType endl_obj);

	/**
	 * @brief ���ز�����<< �ر����DomainType����
	 * @param[in]  domain  ����
	 * @return ����ֵ ��
	 */
	LogStream & operator << (const DomainType & domain);
	
	/**
	 * @brief ���ز�����<< �ر����ServerAddrType����
	 * @param[in]  addr  ��������ַ
	 * @return ����ֵ ��
	 */
	LogStream & operator << (const ServerAddrType & addr);

	/**
	 * @brief ���ز�����<< ��������
	 * @param[in]  content  ���������
	 * @return ����ֵ ��
	 */
	template <class Type>
	LogStream & operator << (const Type & content)
	{
		m_sstream << content;
		return *this;
	};


private:

	//���캯��˽�� ֻ���й�����������
	LogStream(void);

	//˽�п������캯���Ϳ���������
	LogStream(const LogStream & rsh);
	LogStream & operator = (const LogStream &rsh);

    // Member   ������� ������endlʱ �����
    stringstream m_sstream;

	/**
	 * @brief  ��� ���������ݷ��͸�LogPrinter
	 * @return ����ֵ ��
	 */
	void output();

};

class NullLogStream
{
public:
	NullLogStream(void) {};
	~NullLogStream(void) {};

	static NullLogStream CreateNullLogStream()
	{
		NullLogStream nulllog;

		return nulllog;
	}

	// Typedef  ���Ͷ��壺endl, ����д<<������
	typedef basic_ostream<char, char_traits<char> >& (*EndlType)(basic_ostream<char, char_traits<char> >&);

	NullLogStream & operator << (EndlType endl_obj)
	{
		//DO NOTHING
		return *this;
	}

	template <class Type>
	NullLogStream & operator << (const Type & content)
	{
		//DO NOTHING
		return *this;
	}
};


//=========================================//
//������־�������
//����1 ldbg1  ���������Ϣ�������ִ�е������ģ������м���������
//����2 ldbg2  ���������Ϣ�еļƻ��ڴ��������ɾ��ʧ�ܣ��ʵ�IDδ�ҵ������ĸ�ʽ���������   
//��ͨ  lout   ���������Ϣ������Դ��ȡ��·���������ļ���Ϣ��
//����  lwrn   ���������Ϣ�������Ժ��ԵĴ�����Ϣ���˴���ɶ��������Ա���ҵ����Ӱ�죩���磺�յ�����ı��ģ�������¼����͵�
//����  lerr   ����������󣬼���ɳ������޷�����ִ�еĴ����磺��ȡ��Դ�ļ�ʧ�ܣ������߼������


//1. ����1 ldbg1
#ifdef LOG_DEBUG1_OFF 
	#define ldbg1 ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define ldbg1 ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ]" << " [DEBUG_1] " )
	#else
		#define ldbg1 ( LogStream::CreateLogStream() << " [DEBUG_1] " )
	#endif
#endif

//2. ���� ldbg2
#ifdef LOG_DEBUG2_OFF 
	#define ldbg2 ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define ldbg2 ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ]" << " [DEBUG_2] " ) 
	#else
		#define ldbg2 ( LogStream::CreateLogStream() << " [DEBUG_2] " )
	#endif
#endif

//3. ��ͨ lout
#ifdef LOG_INFO_OFF 
	#define lout ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define lout ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ] " ) 
	#else
		#define lout ( LogStream::CreateLogStream() )
	#endif
#endif

//4. ���� lwrn
#ifdef LOG_WARNNING_OFF 
	#define lwrn ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define lwrn ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ]" << " [WARNNING] ") 
	#else
		#define lwrn ( LogStream::CreateLogStream() << " [WARNNING] " )
	#endif
#endif


//5. ���� lerr
#ifdef LOG_ERROR_OFF 
	#define lerr ( NullLogStream::CreateNullLogStream() )
#else
	#ifdef LOG_CODE_DETAIL
		#define lerr ( LogStream::CreateLogStream() << " [ " << (__FUNCTION__) << "() , " << (__FILE__) << " , " << (__LINE__) << " ]" << " [ERROR] ") 
	#else
		#define lerr ( LogStream::CreateLogStream() << " [ERROR] " )
	#endif
#endif

//END
//==========================================//


#endif //LOG_STREAM_H
