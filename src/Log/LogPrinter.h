/**
* Copyright (c) 2010, ������Ϣ�ص�ʵ����,�й���ѧԺ���㼼���о���
* All rights reserved.
*
* �ļ����� LogPrinter.h
* @brief  ��־����࣬�̳���EventEngine
* @project ToWin���߷���ƽ̨
*
* @comment 
* @version 1.1
* @author ������
* @date 2010-06-13
* @
*/

#ifndef LOG_PRINTER_H
#define LOG_PRINTER_H

//BOOST
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

//MAIN
#include "EventEngine/EventEngine.h"
#include "LogData.h"
#include "Common/f_utility.h"

//STL
#include <fstream>
#include <string>
#include <map>
#include <iostream>
using namespace std;


class LogPrinter
	: public EventEngine
{
public:
	~LogPrinter(void);

	static LogPrinter * GetInstance();

	/**
	 * @brief  ��д�����Start����
	 * @param[in]  is_std_io  �Ƿ�ͬʱ����־�������׼��� true ����� false �����
	 * @return ����ֵ ��
	 */
	bool Start(bool is_std_io = true);

private:

	//˽�л������� ��̬ģʽ
	LogPrinter(void);

	//Ψһʵ��
	static LogPrinter ms_log_instance;

	//�Ƿ��Ѿ������������ظ�����
	bool m_is_start;

	//�Ƿ�ͬʱ�������׼���
	bool m_is_std_io;

	//�����
	ofstream m_log_file;

	//��־�м���
	unsigned long m_log_cnt;	

	/**
	 * @brief  �¼�������  �̳��Ը���EventEngine
	 * @return ����ֵ ��
	 */
	void on_event(Event & e);

	/**
	 * @brief  ����һ����־����
	 * @return ����ֵ ��
	 */
	void print_data(const LogData & data);

};


#endif //LOG_PRINTER_H

