#pragma once

#include <string>
#include <vector>
#include "configure.h"
#include "Dictionary.h"
#include "Class.h"
#include "Katuyou.h"
#include "Connect.h"
namespace JPTOKEN
{
class token
{
public:
	token(void);
	~token(void);
	void segment(const string& sentence, string& seg)
	{
		vector<string> v_seg;
		seg.clear();
		segment(sentence, 0, v_seg);
		for (unsigned int i = 0; i < v_seg.size(); i++)
			seg += v_seg[i] + " ";
	}

    void segment(const string& sentence,const int& tag,vector <string>& token);
    bool add_user_dic(const string& filename); //����û��ֵ�
	bool init(const string& initfile);
	private:
    /*�ṹ�嶨��*/
    //����Ȩ���趨
    struct COST_OMOMI{
        int rensetsu;   //����Ȩ��
        int keitaiso;   //��̬��Ȩ��
        int cost_haba;  //Ȩ�ط���
    };
    
    struct PROCESS_BUFFER{
        int mrph_p;     //ǰһ����̬��index
        int start;      //��ʼλ��
        int end;        //����λ��
        int score;      //�÷�
        vector <int> path;   //0Ϊ����·��������Ϊ��һ�����ȷ�Χ�ڵı�ѡ·��
        bool connect;    //�Ƿ���������
        PROCESS_BUFFER()
        {
            path.clear();
        }
    };
    
    struct MRPH{
        string midasi;   //���
        string midasi2; //����
        string yomi;    //ԭ��
        string imis;   //������Ϣ
        
        int hinsi;      //һ��������
        int bunrui;     //����������
        int katuyou1;   //������
        int katuyou2;   //������
        
        int weight;     //Ȩ��
        int con_tbl;    //���ӱ��λ��
        int length;     //��̬�س���
        
        MRPH(){}
    };
    
    struct CHK_CONNECT_WK
    {
        int pre_p;
        int score;
    };
    
    /*ȫ�ֱ�������*/
    
    
	Class classify;
	Katuyou katuyou;
	Connect connect;
	Dictionary dictionary;
	COST_OMOMI cost_omomi;
	

	/*��������*/
	/*δ��¼����Ϣ*/
	int undef_hinsi;
	int undef_kata_bunrui, undef_alph_bunrui, undef_etc_bunrui;
	int undef_kata_con_tbl, undef_alph_con_tbl, undef_etc_con_tbl;

	/*������Ϣ*/
	int suusi_hinsi,suusi_bunrui;

	/*͸����Ϣ*/
	int kakko_hinsi,kakko_bunrui1,kakko_bunrui2;
	int kuuhaku_hinsi,kuuhaku_bunrui,kuuhaku_con_tbl;

	int default_user_dic_weight;


	//����ִ�й����в����޸ĵı���
    
    
    /*��������*/
    void find_all_path(const int& now_pos,vector <int>& save,vector <vector <int> >& all_path,const vector <PROCESS_BUFFER>& p_buffer,const int& p_buffer_num);
    void process_path_mrph(const int& path_num,const int& tag,string& line,const vector <PROCESS_BUFFER>& p_buffer,const vector <MRPH>& m_buffer);
    bool prepare_path_mrph(const int& path_num,MRPH &mrph_p,const vector <PROCESS_BUFFER>& p_buffer,const vector <MRPH>& m_buffer);
    bool init_configure_file(const string& file); //configure�ļ�����
    void init_etc(const string& file);    //�ض�����׼��
    void init_default();        //��ʼ������
    int check_code(const string& now,const int& pos);        //����ض�λ���ַ�����
    int check_utf8_char_type(const string& now,const int& pos);
    int check_unicode_char_type(const int& code);
    int word_length(const char& p);    //���㵱ǰλ���ֳ�
    bool search_all(const string& String,const int& pos,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);   //��ѯ�ʵ乹ͼ
    int take_data(const string& String,int pos,string pbuf,const vector<int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num); //���ݴ���
    void _take_data(string& info,MRPH& mrph);   //��ȡ����
    bool katuyou_process(const string& String,const int& pos,int& k,MRPH& mrph,int& length);        //�����β�ѯ
    bool check_connect(const int& pos,const int& m_num,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);      //�����ж�������
    int pos_match_process(vector <int>& match_pbuf,const int& pos,const int& p_start,const vector<PROCESS_BUFFER>& p_buffer,const int& p_buffer_num); //Ѱ�ҵ�ǰλ�ÿ������ӵ���̬��
    bool undef_word(const string& String,const int& pos,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);   //δ��¼�ʴ���
    bool suusi_word(const int& pos,const int& m_num,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num); //���ʴ���
    bool through_word(const int& pos,const int& m_num,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);       //͸������
    bool is_through(MRPH *mrph_p);      //�ж��Ƿ����͸��
    bool get_user_dic_atom(const string& line,MRPH& now);  //��ȡ�û��ֵ�һ��
    bool graph_make(const string& data,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);       //��ͼ
    void print_path(const int& tag,vector <string>& token,const vector <PROCESS_BUFFER>& p_buffer,const int& p_buffer_num,const vector <MRPH>& m_buffer);
    void print_best_path(const int& tag,vector <string>& token,const vector <PROCESS_BUFFER>& p_buffer,const vector<MRPH>& m_buffer,const int& p_buffer_num); //0:������� 1:���������
    
};

}