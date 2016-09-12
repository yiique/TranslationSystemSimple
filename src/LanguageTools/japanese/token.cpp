#include "stdafx.h"
#include "token.h"
using namespace JPTOKEN;

token::token(void)
{
}

token::~token(void)
{
}

bool token::init(const string& initfile)
{
//	cerr<<"start token init"<<endl;
	init_default();
	if(init_configure_file(initfile)==false)
	{
		cerr<<"Error in reading configure file."<<endl;
		exit(-2);
	}
	return true;
//	cerr<<"end token init"<<endl;
}

//输入          词性标注        结果
void token::segment(const string& sentence,const int& tag,vector<string>& token)
{
    token.clear();
    if(sentence.length()>=MAX_SENTENCE_LEN)
    {
        cerr<<"sentence is too long"<<endl;
        exit(-1);
    }
    if(sentence.length()==0) return;

	vector <PROCESS_BUFFER> p_buffer;
	vector <MRPH> m_buffer;
	int m_buffer_num,p_buffer_num;
	
	if(graph_make(sentence,p_buffer,m_buffer,p_buffer_num,m_buffer_num)==false)
    {
        cerr<<"Error in graph make."<<endl;
        exit(-1);
    }
    //print_path(rev,tag,token);
    print_best_path(tag,token,p_buffer,m_buffer,p_buffer_num);
}
void token::print_path(const int& tag,vector<string>& token,const vector <PROCESS_BUFFER>& p_buffer,const int& p_buffer_num,const vector <MRPH>& m_buffer)
{
    vector <vector <int> > all_path;
    vector <int> single_path;
    string line;
    all_path.clear();
    single_path.clear();
    find_all_path(p_buffer_num-1,single_path,all_path,p_buffer,p_buffer_num);

    for(int i=0;i<all_path.size();i++)
    {
        token.clear();
        for(int j=all_path[i].size()-1;j>=0;j--)
        {
            process_path_mrph(all_path[i][j],tag,line,p_buffer,m_buffer);
            token.push_back(line);
        }
	}
}

void token::find_all_path(const int& now_pos,vector<int>& save,vector<vector<int> >& all_path,const vector <PROCESS_BUFFER>& p_buffer,const int& p_buffer_num)
{
    if(now_pos==0) { all_path.push_back(save); return ; }
    if(now_pos!=p_buffer_num-1) save.push_back(now_pos);
    for(int i=0;i<p_buffer[now_pos].path.size();i++)
        find_all_path(p_buffer[now_pos].path[i],save,all_path,p_buffer,p_buffer_num);
}

void token::print_best_path(const int& tag,vector<string>& token,const vector <PROCESS_BUFFER>& p_buffer,const vector<MRPH>& m_buffer,const int& p_buffer_num)
{
	vector <int> path_buffer;
    int i,j,last;
    string line;
    j = 0;
    token.clear();
    last = p_buffer[p_buffer_num - 1].path[0];
    path_buffer.clear();
    do
    {
        path_buffer.push_back(last);
        last = p_buffer[last].path[0];
    }while(!p_buffer[last].path.empty());

    for(i = path_buffer.size()-1;i>=0;i--)
    {
        process_path_mrph(path_buffer[i],tag,line,p_buffer,m_buffer);
        token.push_back(line);
    }
}

void token::process_path_mrph(const int& path_num,const int& tag,string& line,const vector <PROCESS_BUFFER>& p_buffer,const vector <MRPH>& m_buffer)
{
    MRPH mrph_p;
    if(prepare_path_mrph(path_num,mrph_p,p_buffer,m_buffer)==false) return ;
    line=mrph_p.midasi;
    if(tag==0) return ;
    line += "\t"+classify.Classify[mrph_p.hinsi][0].id;
    //cout<<mrph_p.midasi<<" ";
    //cout<<classify.Classify[mrph_p.hinsi][0].id<<" ";
    if(mrph_p.bunrui!=0)
    {
        line += "\t"+classify.Classify[mrph_p.hinsi][mrph_p.bunrui].id;
    }
    if(mrph_p.katuyou1)
    {
        line += "\t"+katuyou.get_type(mrph_p.katuyou1)+"\t"+katuyou.get_katuyou_name(mrph_p.katuyou1,mrph_p.katuyou2);
    }
}

bool token::prepare_path_mrph(const int& path_num,MRPH &mrph_p,const vector <PROCESS_BUFFER>& p_buffer,const vector <MRPH>& m_buffer)
{
    int i;
    mrph_p = m_buffer[p_buffer[path_num].mrph_p];
    if(mrph_p.midasi2.empty()) mrph_p.midasi2 = mrph_p.midasi;
    if(mrph_p.katuyou1>0&&mrph_p.katuyou2>0)
    {
        mrph_p.midasi += katuyou.get_katuyou_gobi(mrph_p.katuyou1,mrph_p.katuyou2);
        for(i=1;katuyou.get_katuyou_gobi(mrph_p.katuyou1,i)=="基本形";i++);
        mrph_p.midasi2 += katuyou.get_katuyou_gobi(mrph_p.katuyou1,i);
        mrph_p.yomi += katuyou.get_katuyou_gobi_yomi(mrph_p.katuyou1,mrph_p.katuyou2);
    }
    return true;
}

bool token::init_configure_file(const string& file)
{
    iotool setting_file;
    HEAD_NODE   *file_head;
	string source_root;
    //cout<<SETTINGFILE<<endl;
    if(!setting_file.file_open(file))
    {
        cerr<<"Can not open SETTINGFILE: "<<file<<endl;
        exit(-3);
    }
    while(setting_file.read_tree())
    {
        file_head = setting_file.get_tree_head();
		//资源路径
		if(file_head->data[0]==DEF_SOURCE_FILE) source_root = file_head->son[0].data[0];
        //词法文件
        if(file_head->data[0]==DEF_GRAMMAR_FILE)
        {
            if(file_head->son.size()!=1||file_head->son[0].data.size()!=1) return false;
            classify.read_grammar_file(source_root+file_head->son[0].data[0]);
            katuyou.read_katuyou_file(source_root+file_head->son[0].data[0]);
            connect.read_connect_table(source_root+file_head->son[0].data[0]);
            connect.read_connect_matrix(source_root+file_head->son[0].data[0]);
			
        }
        //链接花费权重
        else if(file_head->data[0]==DEF_CONNECT_W)
        {
            if(file_head->data.size()==1) return false;
            cost_omomi.rensetsu = (int)atoi(file_head->data[1].c_str()) * MRPH_DEFAULT_WEIGHT;
        }
        //形态素花费权重
        else if(file_head->data[0]==DEF_WORD_W)
        {
            if(file_head->data.size()==1) return false;
            cost_omomi.keitaiso = (int)atoi(file_head->data[1].c_str());
        }
        //权重幅度
        else if(file_head->data[0]==DEF_COST_MARGIN)
        {
            if(file_head->data.size()==1) return false;
            cost_omomi.cost_haba = (int)atoi(file_head->data[1].c_str()) * MRPH_DEFAULT_WEIGHT;
        }
        //词类花费
        else if(file_head->data[0]==DEF_POS_C)
        {
            classify.read_cost(file_head->son);
        }
        //词典文件
        else if(file_head->data[0]==DEF_DIC_FILE)
        {
		//	cerr<<"start reading dictionary"<<endl;
            for(int i=0;i<file_head->son.size();i++)
			{
                dictionary.add_dictionary(source_root+file_head->son[i].data[0]);
			}
		}
        //用户字典
        else if(file_head->data[0]==USER_DIC_FILE)
        {
            for(int i=0;i<file_head->son.size();i++)
            {
                if(file_head->son[i].data[0].empty()) continue;
                add_user_dic(file_head->son[i].data[0]);
                dictionary.add_dictionary(file_head->son[i].data[0]);
            }
        }
        //用户字典权重
        else if(file_head->data[0]==USER_DIC_WEIGHT)
        {
            default_user_dic_weight = (int)atoi(file_head->data[1].c_str());
        }
		//特殊判定
		else if(file_head->data[0]==SPECIAL_DIC)
		{
			init_etc(source_root+file_head->son[0].data[0]);
		}
    }
    return true;
}

void token::init_default()

{
     cost_omomi.keitaiso = WORD_WEIGHT_DEFAULT;
     cost_omomi.rensetsu = CONNECT_WEIGHT_DEFAULT * MRPH_DEFAULT_WEIGHT;
     cost_omomi.cost_haba = COST_WIDTH_DEFAULT * MRPH_DEFAULT_WEIGHT;
     default_user_dic_weight = DEFAULT_DIC_WEIGHT;
     dictionary.init();
}

void token::init_etc(const string& file)
{
	string DEF_UNDEF;
	string DEF_UNDEF_KATA;
	string DEF_UNDEF_ALPH;
	string DEF_UNDEF_ETC;

	string DEF_SUUSI_HINSI;
	string DEF_SUUSI_BUNRUI;

	string DEF_KAKKO_HINSI;
	string DEF_KAKKO_BUNRUI1;
	string DEF_KAKKO_BUNRUI2;
	string DEF_KUUHAKU_HINSI;
	string DEF_KUUHAKU_BUNRUI;
	ifstream fin(file.c_str());
	string line;
	getline(fin,line);
	DEF_UNDEF = line;
	getline(fin,line);
	DEF_UNDEF_KATA = line;
	getline(fin,line);
	DEF_UNDEF_ALPH = line;
	getline(fin,line);
	DEF_UNDEF_ETC = line;
	getline(fin,line);
	DEF_SUUSI_HINSI = line;
	getline(fin,line);
	DEF_SUUSI_BUNRUI = line;
	getline(fin,line);
	DEF_KAKKO_HINSI = line;
	getline(fin,line);
	DEF_KAKKO_BUNRUI1 = line;
	getline(fin,line);
	DEF_KAKKO_BUNRUI2 = line;
	getline(fin,line);
	DEF_KUUHAKU_HINSI = line;
	getline(fin,line);
	DEF_KUUHAKU_BUNRUI = line;
	fin.close();
    //未登录词
	
    undef_hinsi = classify.get_hinsi_id(DEF_UNDEF);
    undef_kata_bunrui = classify.get_bunrui_id(DEF_UNDEF_KATA,undef_hinsi);
	undef_alph_bunrui = classify.get_bunrui_id(DEF_UNDEF_ALPH,undef_hinsi);
    undef_etc_bunrui = classify.get_bunrui_id(DEF_UNDEF_ETC,undef_hinsi);
    undef_kata_con_tbl = connect.check_table_for_undef(undef_hinsi,undef_kata_bunrui);
    undef_alph_con_tbl = connect.check_table_for_undef(undef_hinsi,undef_alph_bunrui);
    undef_etc_con_tbl = connect.check_table_for_undef(undef_hinsi,undef_etc_bunrui);
    //数词处理准备
    suusi_hinsi = classify.get_hinsi_id(DEF_SUUSI_HINSI);
    suusi_bunrui = classify.get_bunrui_id(DEF_SUUSI_BUNRUI,suusi_hinsi);
    //透过处理
    kakko_hinsi = classify.get_hinsi_id(DEF_KAKKO_HINSI);
    kakko_bunrui1 = classify.get_bunrui_id(DEF_KAKKO_BUNRUI1,kakko_hinsi);
    kakko_bunrui2 = classify.get_bunrui_id(DEF_KAKKO_BUNRUI2,kakko_hinsi);
    kuuhaku_hinsi = classify.get_hinsi_id(DEF_KUUHAKU_HINSI);
    kuuhaku_bunrui  = classify.get_bunrui_id(DEF_KUUHAKU_BUNRUI,kuuhaku_hinsi);
    kuuhaku_con_tbl = connect.check_table_for_undef(kuuhaku_hinsi,kuuhaku_bunrui);

}

bool token::graph_make(const string& String,vector<PROCESS_BUFFER>& p_buffer,vector<MRPH>& m_buffer,int& p_buffer_num,int& m_buffer_num)
{
	vector <int> match_pbuf;

	int length;
    int pos;
    int p_start = 0;

    length = String.length();
    if(length==0) return false; //空行
	
    p_buffer.clear();
    m_buffer.clear();
    //文头处理
    p_buffer.push_back(PROCESS_BUFFER());
    p_buffer[0].end = 0;
    p_buffer[0].score = 0;
    p_buffer[0].mrph_p = 0;
    p_buffer[0].connect = true;
    m_buffer.push_back(MRPH());
    m_buffer[0].hinsi = 0;
    m_buffer[0].bunrui = 0;
    m_buffer[0].con_tbl = 0;
    m_buffer[0].weight = MRPH_DEFAULT_WEIGHT;
    m_buffer[0].midasi = "(文頭)";

    m_buffer_num = 1;
    p_buffer_num = 1;
    for(pos = 0;pos<length;pos+=word_length(String[pos]))
    {
        p_start = pos_match_process(match_pbuf,pos,p_start,p_buffer,p_buffer_num); //记录可以进行计算的前字符串
        if(match_pbuf.size()!=0)    //如果不存在，则表示无法从该位置开始进行连接，则省略，为非法连接位置
        {
           // if(String[pos]&0x80)        //如果为全角场合，则使用词典进行搜索
            {
                if(search_all(String,pos,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num) == false) return false;
            }
            //判定是否为未登录词
            if(undef_word(String,pos,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num)==false) return false;
        }
    }

    //文末处理
    m_buffer.push_back(MRPH());
    m_buffer[m_buffer_num].midasi = "(文末)";
    m_buffer[m_buffer_num].hinsi = 0;
    m_buffer[m_buffer_num].bunrui = 0;
    m_buffer[m_buffer_num].con_tbl = 0;
    m_buffer[m_buffer_num].weight = MRPH_DEFAULT_WEIGHT;
    m_buffer_num++;

    pos_match_process(match_pbuf,pos,p_start,p_buffer,p_buffer_num);
    if(check_connect(length,m_buffer_num-1,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num)==false) return false;
    return true;
}


bool token::undef_word(const string& String,const int& pos,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num)
{
    int i,end,code,next_code,cur_bytes;
    cur_bytes = word_length(String[pos]);
    code = check_code(String,pos);
    if(code == HIRAGANA ||code == KANJI)
    {
        end = pos+cur_bytes;
    }
    else if(code == KUUHAKU)
    {
        end = pos+1;
    }
    else
    {
        end = pos;
        while(true)
        {
            //未定义语长度不能超过MIDASI_MAX
            if(end-pos>=MIDASI_MAX-((code==HANKAKU)?1:cur_bytes)) break;
            end += cur_bytes;
            next_code  = check_code(String,end);
            cur_bytes = word_length(String[end]);
            if(next_code == code ||
                    (code == KATAKANA && next_code == CHOON)||
                    (code == ALPH && next_code == PRIOD) ) continue;
            else break;
        }
    }
    m_buffer.push_back(MRPH());
        switch(code)
        {
                case KUUHAKU:
                        m_buffer[m_buffer_num].hinsi = kuuhaku_hinsi;
                        m_buffer[m_buffer_num].bunrui = kuuhaku_bunrui;
                        m_buffer[m_buffer_num].con_tbl = kuuhaku_con_tbl;
                        break;
                case KATAKANA:
                        m_buffer[m_buffer_num].hinsi = undef_hinsi;
                         m_buffer[m_buffer_num].bunrui = undef_kata_bunrui;
                        m_buffer[m_buffer_num].con_tbl = undef_kata_con_tbl;
                        break;
                case ALPH:
                        m_buffer[m_buffer_num].hinsi = undef_hinsi;
                        m_buffer[m_buffer_num].bunrui = undef_kata_bunrui;
                        m_buffer[m_buffer_num].con_tbl = undef_kata_con_tbl;
                        break;
                default:
                        m_buffer[m_buffer_num].hinsi = undef_hinsi;
                        m_buffer[m_buffer_num].bunrui = undef_etc_bunrui;
                        m_buffer[m_buffer_num].con_tbl = undef_etc_con_tbl;
                        break;
        }
    m_buffer[m_buffer_num].katuyou1 = 0;
    m_buffer[m_buffer_num].katuyou2 = 0;
    m_buffer[m_buffer_num].length = end - pos;
    if(end - pos >= MIDASI_MAX)
    {
        cerr<<"Too long undef_word "<<String<<endl;
        return false;
    }
    if(code == KUUHAKU)
    {
        m_buffer[m_buffer_num].midasi = " ";
        m_buffer[m_buffer_num].yomi = " ";
    }
    else
    {
        m_buffer[m_buffer_num].midasi = String.substr(pos,end-pos);
        m_buffer[m_buffer_num].yomi = m_buffer[m_buffer_num].midasi;
    }
    m_buffer[m_buffer_num].weight = MRPH_DEFAULT_WEIGHT;
    m_buffer[m_buffer_num].midasi2 = m_buffer[m_buffer_num].midasi;
    m_buffer[m_buffer_num].imis = NILSYMBOL;
    check_connect(pos,m_buffer_num,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num);
    m_buffer_num++;
    if(code == KUUHAKU)
        through_word(pos,m_buffer_num-1,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num);
    return true;
}

//记录可以进行计算的前字符串
int token::pos_match_process(vector <int>& match_pbuf,const int& pos,const int& p_start,const vector<PROCESS_BUFFER>& p_buffer,const int& p_buffer_num)
{
    int i,j,pstart = p_start;

    match_pbuf.clear();
    for(i = p_start; i<p_buffer_num;i++)
    {
        if(p_buffer[i].end<=pos||p_buffer[i].connect==false)
        {
            if(i==pstart)      pstart++;
            //如果end<=pos，则下一个字符不可能使用到，或者如果无法作为一个前串，也无法用到，则省略了搜索的可能性
            if(p_buffer[i].end == pos && p_buffer[i].connect == true)
                match_pbuf.push_back(i);
        }
    }
    return pstart;
}

bool token::search_all(const string& String,const int& pos,const vector<int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num)
{
    vector <string> buf;
    for(int dic_no = 0;dic_no<dictionary.getDicNum();dic_no++)
    {
        dictionary.changedic(dic_no);
        dictionary.pat_search(String.substr(pos),buf);
        for(vector <string>::iterator iter = buf.begin();iter!=buf.end();iter++)
        {
		//cout<<*iter<<endl;
            if(take_data(String,pos,*iter,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num)==false) return false;
        }
    }
    return true;
}

int token::take_data(const string& String,int pos, string pbuf,const vector<int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num)
{
    int i,j,num,cno;
    int rengo_con_tbl,rengo_weight;
    MRPH mrph;
    MRPH *new_mrph;
    int length,con_tbl_bak,k2,pnum_bak;
    int new_mrph_num;
    size_t end = pbuf.find("\t");
    mrph.midasi = pbuf.substr(0,end);
    mrph.midasi2.clear();
    pbuf = pbuf.substr(end+1);
    if((unsigned char)pbuf[0]==0xff)
    {
        /* 連語情報だった場合 */
        vector <MRPH> mrph_buf;
        vector <int> add_list;
        mrph_buf.clear();
        add_list.clear();
        int pos_bak;

        pos_bak = pos;

        _take_data(pbuf,mrph);
        rengo_con_tbl = mrph.con_tbl;
        rengo_weight = mrph.weight;
        num = mrph.bunrui;
        for(i = 0; i< num;i++)
        {
            mrph_buf.push_back(MRPH());
            mrph_buf[i].midasi = pbuf.substr(0,pbuf.find(" "));
            pbuf = pbuf.substr(pbuf.find(" ")+1);
            mrph_buf[i].midasi2.clear();
            _take_data(pbuf,mrph_buf[i]);

            length = mrph_buf[i].midasi.length();
            if(classify.kt(mrph_buf[i].hinsi,mrph_buf[i].bunrui))
            {
                if(i<num-1)     //如果不是语尾
                {
                    length += katuyou.get_katuyou_gobi(mrph_buf[i].katuyou1,mrph_buf[i].katuyou2).length();
                    mrph_buf[i].con_tbl += mrph_buf[i].katuyou2-1;
                }
            }
            mrph_buf[i].length = length;
            if(i<num-1) pos+=length;    //记录末尾形态素的位置
        }
        //cout<<"缓冲读完"<<endl;
        //cout<<"hinsi: "<<mrph_buf[num-1].hinsi<<" bunrui: "<<mrph_buf[num-1].bunrui<<endl;
        //进行活用形的记录
        if(classify.kt(mrph_buf[num-1].hinsi,mrph_buf[num-1].bunrui)==false)
        {
            add_list.push_back(0);
        }
        else
        {
            if(mrph_buf[num-1].katuyou2!=0)     //活用型确定的话
            {
                add_list.push_back(mrph_buf[num-1].katuyou2);
            }
            else
            {
                k2 = mrph_buf[num-1].midasi.empty()?2:1;
                while(katuyou_process(String,pos,k2,mrph_buf[num-1],length))
                {
                    add_list.push_back(k2);
                    k2++;
                }
            }
        }
        //cout<<add_list.size()<<endl;
        //活用形连语的追加
        for(j = 0;j<add_list.size();j++)
        {
            pos = pos_bak;
            for(i = 0;i<num;i++)
            {
                m_buffer.push_back(mrph_buf[i]);
                new_mrph = &m_buffer[m_buffer_num];
                m_buffer_num++;
                if(i==0)        //为搭配开头的形态素
                {
                    con_tbl_bak = new_mrph->con_tbl;
                    if(rengo_con_tbl != -1)
                    {
                        if(add_list[j]) cno = add_list[j] -1;
                        else cno = 0;
                        if(connect.check_matrix_left(rengo_con_tbl+cno)) //存在连语特有的左连接规则
                            new_mrph->con_tbl = rengo_con_tbl + cno;
                    }
                    pnum_bak = p_buffer_num;

                    check_connect(pos,m_buffer_num-1,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num);
                    if(p_buffer_num == pnum_bak) break; //如果无法连接,则说明错误,不继续添加路径

                    p_buffer[pnum_bak].end = pos+ new_mrph->length;
                    p_buffer[pnum_bak].connect = false;
                    p_buffer[pnum_bak].score += (classify.getcost(new_mrph->hinsi,new_mrph->bunrui)*new_mrph->weight*cost_omomi.keitaiso*(rengo_weight-10)/10);

                    new_mrph->con_tbl = con_tbl_bak;
                }
                else
                {
                    p_buffer.push_back(PROCESS_BUFFER());
                    p_buffer[p_buffer_num].score =
                            p_buffer[p_buffer_num-1].score+
                            (classify.getcost(new_mrph->hinsi,new_mrph->bunrui)*new_mrph->weight*cost_omomi.keitaiso+
                            (connect.check_matrix(m_buffer[p_buffer[p_buffer_num-1].mrph_p].con_tbl,new_mrph->con_tbl)?
                                connect.check_matrix(m_buffer[p_buffer[p_buffer_num-1].mrph_p].con_tbl,new_mrph->con_tbl):DEFAULT_C_WEIGHT)
                            *cost_omomi.rensetsu)*rengo_weight/10;
                    p_buffer[p_buffer_num].mrph_p = m_buffer_num-1;
                    p_buffer[p_buffer_num].start = pos;
                    p_buffer[p_buffer_num].end = pos+new_mrph->length;
                    p_buffer[p_buffer_num].path.push_back(p_buffer_num-1);
                    if(i<num-1) //如果不为搭配末尾,则不允许连接
                    {
                        p_buffer[p_buffer_num].connect = false;
                    }
                    else
                    {
                        p_buffer[p_buffer_num].connect = true;
                        if(rengo_con_tbl != -1)
                        {
                            if(add_list[j]) cno = add_list[j]-1;
                            else cno = 0;
                            if(connect.check_matrix_right(rengo_con_tbl+cno)) new_mrph->con_tbl = rengo_con_tbl;
                        }
                        if(add_list[j])
                        {
                            new_mrph->katuyou2 = add_list[j];
                            new_mrph->con_tbl += add_list[j]-1;
                            new_mrph->length += katuyou.get_katuyou_gobi(new_mrph->katuyou1,new_mrph->katuyou2).length();
                            p_buffer[p_buffer_num].end += katuyou.get_katuyou_gobi(new_mrph->katuyou1,new_mrph->katuyou2).length();
                        }
                    }
                    p_buffer_num++;
                }
                pos += new_mrph->length;
            }
        }
    }
    else        //普通形态素的情况下
    {
        _take_data(pbuf,mrph);
        if(mrph.weight == STOP_MRPH_WEIGHT) return true;
        if(classify.kt(mrph.hinsi,mrph.bunrui)) //如果存在活用
        {
            if(mrph.katuyou2 == 0)      //没有确定活用形时
            {
                k2 = 1;
                while(katuyou_process(String,pos,k2,mrph,length))  //寻找可能的活用
                {
                    m_buffer.push_back(mrph);
                    m_buffer[m_buffer_num].katuyou2 = k2;
                    m_buffer[m_buffer_num].con_tbl += (k2-1);
                    m_buffer[m_buffer_num].length = length;
                    check_connect(pos,m_buffer_num,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num);
                    k2++;
                    m_buffer_num++;
                }
            }
            else
            {
                m_buffer.push_back(mrph);
                m_buffer[m_buffer_num].midasi.clear();
                m_buffer[m_buffer_num].yomi.clear();
                m_buffer[m_buffer_num].midasi2.clear();
                check_connect(pos,m_buffer_num,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num);
                m_buffer_num++;
            }
        }
        else    //不存在活用
        {
            //if(mrph.midasi.length()>=BYTES4CHAR)
            {
                m_buffer.push_back(mrph);
                check_connect(pos,m_buffer_num,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num);
                new_mrph_num = m_buffer_num;
                m_buffer_num++;
                if(suusi_word(pos,new_mrph_num,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num)==false) return false;
                if(through_word(pos,new_mrph_num,match_pbuf,p_buffer,m_buffer,p_buffer_num,m_buffer_num)==false) return false;
            }
        }

    }
    return true;
}

//透过处理

bool token::through_word(const int& pos,const int& m_num,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num)
{
    int i,j,k,l,n,nn,sc,scmin;
    MRPH *now_mrph,*mrph_p;
    now_mrph = &m_buffer[m_num];

    if(!is_through(now_mrph)) return true;
    for(l = 0;l<match_pbuf.size();l++)
    {
        i = match_pbuf[l];
        for(j = 0;j<m_buffer_num;j++)
        {
            mrph_p = &m_buffer[j];
            if(mrph_p->hinsi == now_mrph->hinsi &&
                    mrph_p->bunrui == now_mrph->bunrui &&
                    mrph_p->con_tbl == m_buffer[p_buffer[i].mrph_p].con_tbl &&
                    mrph_p->weight == now_mrph->weight &&
                    mrph_p->midasi == now_mrph->midasi &&
                    mrph_p->yomi == now_mrph->yomi) break;
        }
        if((n=j)==m_buffer_num)
        {
            m_buffer.push_back(*now_mrph);
            m_buffer[m_buffer_num].con_tbl = m_buffer[p_buffer[i].mrph_p].con_tbl;
            m_buffer_num ++;
            now_mrph = &m_buffer[m_num];
        }

        sc = now_mrph->weight*cost_omomi.keitaiso*classify.getcost(now_mrph->hinsi,now_mrph->bunrui);
        for(j = 0;j<p_buffer_num;j++)
            if(p_buffer[j].mrph_p == n && p_buffer[j].start == pos) break;
        if((nn = j)==p_buffer_num)
        {
            p_buffer.push_back(PROCESS_BUFFER());
            p_buffer[p_buffer_num].score = p_buffer[i].score + sc;
            p_buffer[p_buffer_num].mrph_p = n;
            p_buffer[p_buffer_num].start = pos;
            p_buffer[p_buffer_num].end = pos + now_mrph->length;
            p_buffer[p_buffer_num].path.push_back(i);
            p_buffer[p_buffer_num].connect = true;
            p_buffer_num ++;
        }
        else
        {
            p_buffer[nn].path.push_back(i);

            scmin = INT_MAX;
            for(j = 0;j<p_buffer[nn].path.size();j++)
                if(scmin>p_buffer[p_buffer[nn].path[j]].score)
                    scmin = p_buffer[p_buffer[nn].path[j]].score;
            for(j = 0;j<p_buffer[nn].path.size();j++)
                if(p_buffer[p_buffer[nn].path[j]].score > scmin + cost_omomi.cost_haba)
                {
                    for(k=j;k<p_buffer[nn].path.size()-1;k++)
                        p_buffer[nn].path[k] = p_buffer[nn].path[k+1];
                    if(p_buffer[nn].path.size()!=0) p_buffer[nn].path.pop_back();
                    j--;
                }
            p_buffer[nn].score = scmin+sc;
        }
    }
    return true;
}

//判定透过过程

bool token::is_through(MRPH *mrph_p)
{
    //如果为空白,或者为阔符结尾,则表示可以透过
    if(
        (mrph_p->hinsi == kakko_hinsi && mrph_p->bunrui == kakko_bunrui2)||
            (mrph_p->hinsi == kuuhaku_hinsi && mrph_p->bunrui == kuuhaku_bunrui)) return true;
    return false;
}

//进行数词的处理
bool token::suusi_word(const int& pos,const int& m_num,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num)
{
    int i,j;
    MRPH *new_mrph ,*pre_mrph;
    new_mrph = &m_buffer[m_num];
    if(new_mrph->hinsi!=suusi_hinsi||new_mrph->bunrui!=suusi_bunrui) return true;
    for(j=0;j<match_pbuf.size();j++)
    {
        i = match_pbuf[j];
        pre_mrph = &m_buffer[p_buffer[i].mrph_p];
        if(pre_mrph->hinsi==suusi_hinsi
                &&pre_mrph->bunrui==suusi_bunrui
                &&connect.check_matrix(pre_mrph->con_tbl,new_mrph->con_tbl)!=0)
        {
            if(pre_mrph->midasi.length()+new_mrph->midasi.length()>=MIDASI_MAX
                    ||pre_mrph->yomi.length()+new_mrph->yomi.length()>=YOMI_MAX)
            {
                return true;
            }

            m_buffer.push_back(*pre_mrph);
            m_buffer[m_buffer_num].midasi+=new_mrph->midasi;
            m_buffer[m_buffer_num].yomi += new_mrph->yomi;
            m_buffer[m_buffer_num].length += new_mrph->midasi.length();

            m_buffer[m_buffer_num].weight = new_mrph->weight;
            m_buffer[m_buffer_num].con_tbl = new_mrph->con_tbl;

            p_buffer.push_back(p_buffer[i]);
            p_buffer[p_buffer_num].end = pos+new_mrph->midasi.length();
            p_buffer[p_buffer_num].mrph_p = m_buffer_num;
            p_buffer[p_buffer_num].score += (new_mrph->weight-pre_mrph->weight)*
                    classify.getcost(new_mrph->hinsi,new_mrph->bunrui)*cost_omomi.keitaiso;

            m_buffer_num++;
            p_buffer_num++;
        }
    }
    return true;
}

//对于刚记录下的可能形态素，对每种可能的前向连接进行权重等的计算，判定是否可以连接，并计算连接花费
bool token::check_connect(const int& pos,const int& m_num,const vector <int>& match_pbuf,vector <PROCESS_BUFFER>& p_buffer, vector <MRPH>& m_buffer, int& p_buffer_num, int& m_buffer_num)
{
    static CHK_CONNECT_WK chk_connect[MAX_PATHES_WK];
    MRPH *new_mrph;
    int chk_con_num;
    int score,best_score,best_score_num,haba_score;
    int c_score,class_score;    //连接权重,形态素权重
    int i,j;
    int left_con,right_con;

    new_mrph = &m_buffer[m_num];
    best_score = INT_MAX;
    chk_con_num = 0;
    score = class_score = best_score_num = 0;
     //cout<<new_mrph->hinsi<<" "<<new_mrph->bunrui<<" "<<new_mrph->con_tbl;
    class_score = classify.getcost(new_mrph->hinsi,new_mrph->bunrui)*new_mrph->weight*cost_omomi.keitaiso;
    for(i = 0;i<match_pbuf.size();i++)
    {
        j = match_pbuf[i];
        //计算现在的形态素与前面的可能链接的形态素之间的链接花费权重
        left_con = m_buffer[p_buffer[j].mrph_p].con_tbl;
        right_con = new_mrph->con_tbl;
        c_score = connect.check_matrix(left_con,right_con);
        if(c_score)     //如果可以连接
        {
            chk_connect[chk_con_num].pre_p = j;
            score = p_buffer[j].score + c_score*cost_omomi.rensetsu;
            chk_connect[chk_con_num].score = score;
            if(score<best_score)
            {
                best_score = score;
                best_score_num = chk_con_num;
            }
            chk_con_num++;
        }

         /***********测试输出*****************/

        if(false)
        {
                cout<<pos<<"    ";
                MRPH *pre_mrph = &m_buffer[p_buffer[j].mrph_p];
                cout<<pre_mrph->midasi;
                if(classify.kt(pre_mrph->hinsi,pre_mrph->bunrui))
                        cout<<katuyou.get_katuyou_gobi(pre_mrph->katuyou1,pre_mrph->katuyou2);
                if(!classify.Classify[pre_mrph->hinsi][0].id.empty())
                {
                        cout<<"("<<classify.Classify[(pre_mrph->hinsi)][0].id;
                        if(pre_mrph->bunrui) cout<<"-"<<classify.Classify[(pre_mrph->hinsi)][(pre_mrph->bunrui)].id;
                        if(pre_mrph->katuyou1) cout<<"-"<<katuyou.get_type(pre_mrph->katuyou1);
                        if(pre_mrph->katuyou2) cout<<"-"<<katuyou.get_katuyou_name(pre_mrph->katuyou1,pre_mrph->katuyou2);
                        cout<<")";
                }
                cout<<"[= "<<p_buffer[j].score<<"]";

                if(c_score)
                {
                        cout<<"--[+"<<c_score<<"*"<<cost_omomi.rensetsu<<"]--";
                }
                else
                {
                        cout<<"--XXX--";
                }
                cout<<new_mrph->midasi;
                if(classify.kt(new_mrph->hinsi,new_mrph->bunrui))
                        cout<<katuyou.get_katuyou_gobi(new_mrph->katuyou1,new_mrph->katuyou2);
                if(!classify.Classify[new_mrph->hinsi][0].id.empty())
                {
                        cout<<"("<<classify.Classify[(new_mrph->hinsi)][0].id;
                        if(new_mrph->bunrui) cout<<"-"<<classify.Classify[new_mrph->hinsi][new_mrph->bunrui].id;
                        if(new_mrph->katuyou1) cout<<"-"<<katuyou.get_type(new_mrph->katuyou1);
                        if(new_mrph->katuyou2) cout<<"-"<<katuyou.get_katuyou_name(new_mrph->katuyou1,new_mrph->katuyou2);
                        cout<<")";
                }
                if(c_score==0)
                {
                        cout<<endl;
                }
                else
                {
                        cout<<"[+"<<classify.Classify[new_mrph->hinsi][new_mrph->bunrui].cost<<"*"<<new_mrph->weight/10<<"."<<new_mrph->weight%10<<"*"<<cost_omomi.keitaiso*10<<"="<<p_buffer[j].score+c_score*cost_omomi.rensetsu+class_score<<"]"<<endl;
                }
        }

        /*************************************************/
    }

    if(best_score == INT_MAX) return true; //不存在路径

    //0号存储最佳路径
    p_buffer.push_back(PROCESS_BUFFER());
    p_buffer[p_buffer_num].path.push_back(chk_connect[best_score_num].pre_p);
    haba_score = best_score+cost_omomi.cost_haba;
    //其余存储权重小于haba_score的所有可能路径
    for(j=0;j<chk_con_num;j++)
    {
        if(chk_connect[j].score <= haba_score && j!=best_score_num)
            p_buffer[p_buffer_num].path.push_back(chk_connect[j].pre_p);
    }

    p_buffer[p_buffer_num].score = best_score + class_score;    //当前最优权重 + 该形态素权重
    p_buffer[p_buffer_num].mrph_p = m_num;
    p_buffer[p_buffer_num].start = pos;
    p_buffer[p_buffer_num].end = pos+new_mrph->length;
    p_buffer[p_buffer_num].connect = true;
    p_buffer_num++;
    return true;
}

//如果只给了大类活用，那么寻找可能的小类活用，如果后面的词为正确活用，则返回
bool token::katuyou_process(const string& String,const int& pos,int& k,MRPH& mrph,int& length)
{
    while(k<katuyou.get_form_size(mrph.katuyou1))
    {
        if(String.substr(pos+mrph.length).find(katuyou.get_katuyou_gobi(mrph.katuyou1,k))==0)
        {
            length = mrph.length + katuyou.get_katuyou_gobi(mrph.katuyou1,k).length();
            return true;
        }
        else k++;
    }
    return false;
}

void token::_take_data(string& info, MRPH& mrph)
{
    int j;
    mrph.hinsi = dictionary.numeral_decode(info);
    mrph.bunrui = dictionary.numeral_decode(info);
    mrph.katuyou1 = dictionary.numeral_decode(info);
    mrph.katuyou2 = dictionary.numeral_decode(info);
    mrph.weight = dictionary.numeral_decode(info);
    mrph.con_tbl = dictionary.numeral_decode2(info);
    dictionary.hiragana_decode(info,mrph.yomi);
    mrph.length = mrph.midasi.length();
    mrph.imis.clear();
    //cout<<"take_data: "<<info<<endl;
    if(info.length()!=0&&info[0]!=' ')     //情报没有完结
    {
        j = dictionary.numeral_decode(info);
        mrph.imis = info.substr(0,j);
        info = info.substr(j);
    }
    if(info.length()!=0) info = info.substr(1);
}

int token::word_length(const char& p)
{
    unsigned char c = p;
    if (c > 0xfb) { /* 6 bytes */
	return 6;
    }
    else if (c > 0xf7) { /* 5 bytes */
	return 5;
    }
    else if (c > 0xef) { /* 4 bytes */
	return 4;
    }
    else if (c > 0xdf) { /* 3 bytes */
	return 3;
    }
    else if (c > 0x7f) { /* 2 bytes */
	return 2;
    }
    else { /* 1 byte */
	return 1;
    }
}

int token::check_code(const string& now,const int& pos)
{
    if(now[pos] =='\0') return 0;
    else if(now[pos] == KUUHAKU) return KUUHAKU;
    else if(now[pos] == HANKAKU) return HANKAKU;
    return check_utf8_char_type(now,pos);
}

int token::check_unicode_char_type(const int& code)
{
    if(code >0x303f && code < 0x30a0) return HIRAGANA;
    if(code >0x309f&&code <0x30fb) return KATAKANA;
    if(code == 0x30fc) return CHOON;
    if(code == 0xff0e) return PRIOD;
    if(code > 0xff0f && code < 0xff1a) return SUJI;
    if((code>0x40&&code<0x5b)||
            (code>0x60&&code<0x7b)||
            (code>0xbf&&code<0x0100)||
            (code>0xff20&&code<0xff5b)) return ALPH;
    if((code > 0x4dff && code < 0xa000) || code == 0x3005) return KANJI;
    if(code > 0x036f && code<0x0400) return GR;
    return KIGOU;
}

int token::check_utf8_char_type(const string& now,const int& pos)
{
    int code = 0;
    int unicode;

    unsigned char c = now[pos];
    if(c>0xfb) {        //6bytes
        code = 0;
    }
    else if(c>0xf7){    //5bytes
        code =0;
    }
    else if(c>0xef){    //4bytes
        code =0;
    }

    else if(c>0xdf){    //3bytes
        unicode = (c&0x0f)<<12;
        c= now[pos+1];
        unicode +=(c&0x3f)<<6;
        c = now[pos+2];
        unicode += c&0x3f;
        code = check_unicode_char_type(unicode);
    }
    else if(c>0x7f){    //2bytes
        unicode = (c&0x1f) <<6;
        c = now[pos+1];
        unicode += c& 0x3f;
        code = check_unicode_char_type(unicode);
    }
    else {      //1bytes
        code = check_unicode_char_type(c);
    }
    return code;
}

bool token::add_user_dic(const string& filename)
{
    ifstream fin(filename.c_str());
    if(!fin.is_open())
    {
        cerr<<"Can not open user file "<<filename<<endl;
        exit(-1);
    }
    vector <string> userdic;
    MRPH now;
    userdic.clear();
    int Line_No = 0;
    string line;
    while(getline(fin,line))
    {
        Line_No++;
        if(get_user_dic_atom(line,now)==false)
        {
            cerr<<"Error in reading user dic file line "<<Line_No<<endl;
            cerr<<line<<endl;
            exit(-1);
        }

        line.clear();
        line = now.midasi+"\t";
        line +=dictionary.numeral_encode(now.hinsi);
        line +=dictionary.numeral_encode(now.bunrui);
        line +=dictionary.numeral_encode(now.katuyou1);
        line +=dictionary.numeral_encode(now.katuyou2);
        line +=dictionary.numeral_encode(now.weight);
        line +=dictionary.numeral_encode2(now.con_tbl);
        line +=dictionary.hiragana_encode(now.yomi);
        userdic.push_back(line);
       /*
        _take_data(line,tmp);
        {
            cout<<tmp.hinsi<<" "<<tmp.bunrui<<" "<<tmp.katuyou1<<" "<<tmp.katuyou2<<" "<<tmp.weight<<" "<<tmp.con_tbl<<" "<<tmp.yomi<<endl;
        }
        */
    }
    ofstream out(filename.c_str());
    for(int i=0;i<userdic.size();i++)
        out<<userdic[i]<<endl;
    out.close();
    //dictionary.createDic(userdic,filename);
    cerr<<"User Dictionary Add Successfully"<<endl;
    return true;
}

bool token::get_user_dic_atom(const string& line,MRPH& now)
{
    if(line.length()==0) return true;
    istringstream stream(line);
    string tmp;
    vector <string> ll;
    ll.clear();
    while(stream>>tmp) ll.push_back(tmp);
    if(ll.size()!=3)
    {
        cerr<<"User dic line can only have three elements!"<<endl;
        return false;
    }
    now.midasi = ll[0];
    now.midasi2 = now.midasi;
    now.yomi = now.midasi;
    now.weight = DEFAULT_DIC_WEIGHT;
    now.imis.clear();
    if(ll[1]=="*")
    {
        cerr<<"Uer dic must have hinsi"<<endl;
        return false;
    }
    now.hinsi = classify.get_hinsi_id(ll[1]);
    now.bunrui = classify.get_bunrui_id(ll[2],now.hinsi);
    now.con_tbl = 0;
    if(classify.kt(now.hinsi,now.bunrui)==true)
    {
        cerr<<ll[1]<<" "<<ll[2]<<" have conjugation!"<<endl;
        return false;
    }
    now.katuyou1  = now.katuyou2 = 0;
    return true;
}