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
    bool add_user_dic(const string& filename); //添加用户字典
	bool init(const string& initfile);
	private:
    /*结构体定义*/
    //花费权重设定
    struct COST_OMOMI{
        int rensetsu;   //链接权重
        int keitaiso;   //形态素权重
        int cost_haba;  //权重幅度
    };
    
    struct PROCESS_BUFFER{
        int mrph_p;     //前一个形态素index
        int start;      //开始位置
        int end;        //结束位置
        int score;      //得分
        vector <int> path;   //0为最优路径，其余为在一定幅度范围内的备选路径
        bool connect;    //是否允许链接
        PROCESS_BUFFER()
        {
            path.clear();
        }
    };
    
    struct MRPH{
        string midasi;   //输出
        string midasi2; //发音
        string yomi;    //原型
        string imis;   //语义信息
        
        int hinsi;      //一级词类标号
        int bunrui;     //二级词类标号
        int katuyou1;   //活用型
        int katuyou2;   //活用形
        
        int weight;     //权重
        int con_tbl;    //连接表的位置
        int length;     //形态素长度
        
        MRPH(){}
    };
    
    struct CHK_CONNECT_WK
    {
        int pre_p;
        int score;
    };
    
    /*全局变量定义*/
    
    
	Class classify;
	Katuyou katuyou;
	Connect connect;
	Dictionary dictionary;
	COST_OMOMI cost_omomi;
	

	/*参数定义*/
	/*未登录词信息*/
	int undef_hinsi;
	int undef_kata_bunrui, undef_alph_bunrui, undef_etc_bunrui;
	int undef_kata_con_tbl, undef_alph_con_tbl, undef_etc_con_tbl;

	/*数词信息*/
	int suusi_hinsi,suusi_bunrui;

	/*透过信息*/
	int kakko_hinsi,kakko_bunrui1,kakko_bunrui2;
	int kuuhaku_hinsi,kuuhaku_bunrui,kuuhaku_con_tbl;

	int default_user_dic_weight;


	//代码执行过程中不断修改的变量
    
    
    /*函数定义*/
    void find_all_path(const int& now_pos,vector <int>& save,vector <vector <int> >& all_path,const vector <PROCESS_BUFFER>& p_buffer,const int& p_buffer_num);
    void process_path_mrph(const int& path_num,const int& tag,string& line,const vector <PROCESS_BUFFER>& p_buffer,const vector <MRPH>& m_buffer);
    bool prepare_path_mrph(const int& path_num,MRPH &mrph_p,const vector <PROCESS_BUFFER>& p_buffer,const vector <MRPH>& m_buffer);
    bool init_configure_file(const string& file); //configure文件处理
    void init_etc(const string& file);    //特定处理准备
    void init_default();        //初始化定义
    int check_code(const string& now,const int& pos);        //检测特定位的字符类型
    int check_utf8_char_type(const string& now,const int& pos);
    int check_unicode_char_type(const int& code);
    int word_length(const char& p);    //计算当前位置字长
    bool search_all(const string& String,const int& pos,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);   //查询词典构图
    int take_data(const string& String,int pos,string pbuf,const vector<int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num); //数据处理
    void _take_data(string& info,MRPH& mrph);   //获取数据
    bool katuyou_process(const string& String,const int& pos,int& k,MRPH& mrph,int& length);        //活用形查询
    bool check_connect(const int& pos,const int& m_num,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);      //连接判定及计算
    int pos_match_process(vector <int>& match_pbuf,const int& pos,const int& p_start,const vector<PROCESS_BUFFER>& p_buffer,const int& p_buffer_num); //寻找当前位置可以连接的形态素
    bool undef_word(const string& String,const int& pos,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);   //未登录词处理
    bool suusi_word(const int& pos,const int& m_num,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num); //数词处理
    bool through_word(const int& pos,const int& m_num,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);       //透过处理
    bool is_through(MRPH *mrph_p);      //判定是否可以透过
    bool get_user_dic_atom(const string& line,MRPH& now);  //读取用户字典一行
    bool graph_make(const string& data,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num);       //构图
    void print_path(const int& tag,vector <string>& token,const vector <PROCESS_BUFFER>& p_buffer,const int& p_buffer_num,const vector <MRPH>& m_buffer);
    void print_best_path(const int& tag,vector <string>& token,const vector <PROCESS_BUFFER>& p_buffer,const vector<MRPH>& m_buffer,const int& p_buffer_num); //0:输出词性 1:不输出词性
    
};

}