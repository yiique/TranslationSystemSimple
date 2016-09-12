#ifndef DICT_DEF_H
#define DICT_DEF_H

//MAIN
#include "Common/BasicType.h"

#include <string>
#include <utility>
using namespace std;

#define SYSMANAGER "superadmin"

typedef size_t WordID;
typedef size_t DictID;
typedef int DictType;


#define DICT_TYPE_USR 0
#define DICT_TYPE_SYS 1
#define DICT_TYPE_PRO 2


class WordInfo
{
public:
	WordInfo(const int _is_deleted = 0, const int _is_active = 1, const int _is_checked = 1) : is_deleted(_is_deleted), is_active(_is_active), is_checked(_is_checked) {}

	WordID word_id;

	string src;
	string tgt;
	string src_key;

	int is_deleted; //�Ƿ���ɾ��  1��ʾ�Ѿ�ɾ�� �־ò���
	int is_active; //�Ƿ�����  1��ʾ����

	int is_checked;  //�Ƿ������  1��ʾ�Ѿ����

	size_t dict_id;  //��ʼ����ʱ����
};

class DictInfo
{
public:

	DictInfo(const int _is_deleted = 0, const int _is_active = 1, const int _type = 0) : is_deleted(_is_deleted), is_active(_is_active), type(_type) {}

	DictID dict_id;
	DomainType domain_info;

	string dict_name;
	UsrID usr_id;

	DictType type; //�ʵ����� 0 �û��ʵ䣻 1 ϵͳ�ʵ䣻 2 רҵ�ʵ䣻 �����ݿ��ֶζ�Ӧis_system��

	int is_deleted; //�Ƿ���ɾ��  1��ʾ�Ѿ�ɾ�� �־ò���
	int is_active; //�Ƿ�����  1��ʾ����

	string description;

};


#endif //DICT_DEF_H
