#include "DBOperation.h"
#include "DBConnPool.h"

//add by yangli 20150724 with function UyLower::to_lower()
#include "LanguageTools/uyghur/uytoken/UyToken.h"

#include "Common/ICUConverter.h"
#include <time.h>
#include <boost/algorithm/string.hpp>
#define GROUPNUM 100

//filetrans表
sql_create_7(filetrans,
			 1, 7,
			 mysqlpp::sql_varchar, guid,
			 mysqlpp::sql_varchar, username,
			 mysqlpp::sql_varchar, fileext,
			 mysqlpp::sql_varchar, srcname,
			 mysqlpp::sql_varchar, type, //领域
			 mysqlpp::sql_varchar, srclanguage,
			 mysqlpp::sql_varchar, tgtlanguage);

//transsentence表
sql_create_14(transsentence,
			 1,14,
			 mysqlpp::sql_bigint,  SentID,
			 mysqlpp::sql_varchar, TextID,
			 mysqlpp::sql_blob, SrcSent,
			 mysqlpp::sql_blob, TgtSent, //用于辅助翻译
			 mysqlpp::sql_blob, FinalTgtSent,
			 mysqlpp::sql_blob, AssistRes,
			 mysqlpp::sql_blob, CurrentEditJsonStr,
			 mysqlpp::sql_blob, ModifySent,
			 mysqlpp::sql_blob, RulesRes,
			 mysqlpp::sql_blob, DecodeSent,
			 mysqlpp::sql_blob, PreSent,
			 mysqlpp::sql_int,  ParaIdx,
			 mysqlpp::sql_varchar, SrcLanguage,
			 mysqlpp::sql_varchar, TgtLanguage);

//dictinfo表
sql_create_10(dictinfo,
			 1, 10,
			 mysqlpp::sql_int, tid,  //dict id
			 mysqlpp::sql_varchar, username,
			 mysqlpp::sql_tinyblob, dictname,
			 mysqlpp::sql_int, isdeleted,   //是否已删除  1表示已经删除
			 mysqlpp::sql_int, isactive,   //是否启用  1表示启用
			 mysqlpp::sql_int, issystem,   //是否是系统词典 1表示是
			 mysqlpp::sql_tinyblob, description,
			 mysqlpp::sql_varchar, type, //领域
			 mysqlpp::sql_varchar, srclanguage,
			 mysqlpp::sql_varchar, tgtlanguage
			);

//dictiteminfo表
sql_create_7(dictiteminfo,
	         1, 7,
			 mysqlpp::sql_int, tid, //词条id
			 mysqlpp::sql_int, dictid, //所属词典的ID
			 mysqlpp::sql_tinyblob, src, 
			 mysqlpp::sql_tinyblob, tgt,
			 mysqlpp::sql_int, isdeleted,   //是否已删除  1表示已经删除
			 mysqlpp::sql_int, isactive,   //是否启用  1表示启用 
			 mysqlpp::sql_int, ischecked
			 );


//templatelibinfo表
sql_create_10(templatelibinfo,
			 1, 10,
			 mysqlpp::sql_int, tid,  //templatelib id
			 mysqlpp::sql_varchar, username,
			 mysqlpp::sql_tinyblob, templatelibname,
			 mysqlpp::sql_int, isdeleted,   //是否已删除  1表示已经删除
			 mysqlpp::sql_int, isactive,   //是否启用  1表示启用
			 mysqlpp::sql_int, issystem,
			 mysqlpp::sql_tinyblob, description,
			 mysqlpp::sql_varchar, type, //领域
			 mysqlpp::sql_varchar, srclanguage,
			 mysqlpp::sql_varchar, tgtlanguage
			 );

//templateiteminfo表
sql_create_7(templateiteminfo,
	         1, 7,
			 mysqlpp::sql_int, tid, //词条id
			 mysqlpp::sql_int, templatelib_id, //所属词典的ID
			 mysqlpp::sql_tinyblob, src, 
			 mysqlpp::sql_tinyblob, tgt,
			 mysqlpp::sql_int, isdeleted,   //是否已删除  1表示已经删除
			 mysqlpp::sql_int, isactive,   //是否启用  1表示启用
			 mysqlpp::sql_int, ischecked
			 );

//afterdictinfo表
sql_create_9(afterdictinfo,
			 1, 9,
			 mysqlpp::sql_int, tid,  //dict id
			 mysqlpp::sql_varchar, username,
			 mysqlpp::sql_tinyblob, dictname,
			 mysqlpp::sql_int, isdeleted,   //是否已删除  1表示已经删除
			 mysqlpp::sql_int, isactive,   //是否启用  1表示启用
			 mysqlpp::sql_tinyblob, description,
			 mysqlpp::sql_varchar, type, //领域
			 mysqlpp::sql_varchar, srclanguage,
			 mysqlpp::sql_varchar, tgtlanguage
			);

//afterdictiteminfo表
sql_create_6(afterdictiteminfo,
	         1, 6,
			 mysqlpp::sql_int, tid, //词条id
			 mysqlpp::sql_int, afterdictid, //所属词典的ID
			 mysqlpp::sql_tinyblob, src, 
			 mysqlpp::sql_tinyblob, tgt,
			 mysqlpp::sql_int, isdeleted,   //是否已删除  1表示已经删除
			 mysqlpp::sql_int, isactive   //是否启用  1表示启用 
			 );

//chsegment 表
sql_create_4(chsegment,
			 1, 4,
			 mysqlpp::sql_int, tid,
			 mysqlpp::sql_tinyblob, segword,
			 mysqlpp::sql_int, isdeleted,
			 mysqlpp::sql_int, isactive
			 );
//usercharege 表
sql_create_5(usercharege,
             1, 5,
             mysqlpp::sql_varchar, username,
             mysqlpp::sql_date,    expireddate,
             mysqlpp::sql_int,     chargetype,
             mysqlpp::sql_int,     totalcharacter,
             mysqlpp::sql_int,     usedcharacter
			);


//trandir 表
sql_create_7(transdir,
			1, 7,
			mysqlpp::sql_int, dirid,
			mysqlpp::sql_varchar, srclanguage,
			mysqlpp::sql_varchar, tgtlanguage,
			mysqlpp::sql_datetime, inserttime,
			mysqlpp::sql_varchar, dirinfo,
			mysqlpp::sql_varchar, type,
			mysqlpp::sql_int, isspace
			);


int DBOperation::GetTransFileInfo(const TextID & tid, string & file_type, string & file_path)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_trans_file_info, _1, 
												boost::ref(tid),
												boost::ref(file_type),
												boost::ref(file_path)));
}

int DBOperation::get_trans_file_info(mysqlpp::Query & query, 
									 const TextID & tid, 
									 string & file_type, 
									 string & file_path)
{
	try
	{
		query << "select fileext, srcname from filetrans where guid = \"" << tid << "\""; //TODO 以后如果TextID类型改变 需要设置一个转换为string或其他常用类型的方法
		vector<filetrans> res;
		query.storein(res);

		if(res.size() == 0 )
			return ERR_DB_NORESULT;

		cout << "res.size() = " << res.size() << endl;

		filetrans & ft = res[0];

		file_type = ft.fileext;
		file_path = ft.srcname;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;

}

int DBOperation::GetPermission(const string & usr_id, const size_t text_character)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_permission, _1,
													boost::ref(usr_id),
													boost::ref(text_character)));
}


int DBOperation::get_permission(mysqlpp::Query & query, const string & usr_id, const size_t text_character)
{
	try
	{

        query << "select chargetype, totalcharacter, usedcharacter from usercharge where username =\"" << usr_id << "\"";
        vector<usercharege> res;
        query.storein(res);

        if(res.size() == 0)
        {
            cerr << "Can't find this user in usercharge = " << usr_id << endl;
            return ERR_DB_NORESULT;
        }

        usercharege & uc = res[0];

        if( uc.chargetype == 1)
		{
			query.reset();

			query << "select * from usercharge where username = \"" << usr_id << "\" and expireddate >= now()";
			mysqlpp::StoreQueryResult res = query.store();

			if(res.num_rows() == 0)
			    return ERR_PERMISSION_TIME;
			else
				return SUCCESS;
		}else
		{
			if(uc.usedcharacter + text_character > uc.totalcharacter) //TODO size_t 会不会太小？
				return ERR_PERMISSION_CHARACTER;
			
			query.reset();
			query << "update usercharge set usedcharacter=" << uc.usedcharacter + text_character << " where username = \"" << usr_id << "\"";
			query.execute();

			return SUCCESS;
		}
          
	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::SubmitTransResult(TransText * p_text)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::submit_trans_result, _1, boost::ref(p_text)));
}

int DBOperation::submit_trans_result(mysqlpp::Query & query, TransText * p_text)
{
	try
	{

		//Step 1. 提交到TransSent表
		TransText & text = *p_text;
		string res_data;
		stringstream sent_id_ss;
		string old_assist_sent_set;

		
		for(size_t i=0; i<text.GetTransSize(); ++i)
		{
			string src_sent; 
			string tgt_sent; 
			string assist;   
			string assist2;

			//转换为UTF-8
            //lout << "tgt = [" << text.GetTransTgt(i)._tgt_str << "]" << endl;
            //lout << "as1 = [" <<  text.GetTransTgt(i)._assist_str << "]" << endl;
            //lout << "as2 = [" <<  text.GetTransTgt(i)._assist2_str << "]" << endl;
			

			transsentence sent(text.GetTransCell(i)._sent_uid,
							   text.GetID(), 
							   mysqlpp::sql_blob(text.GetTransSrc(i)._src_str) ,
							   mysqlpp::sql_blob(text.GetTransTgt(i)._assist_tgt),
							   mysqlpp::sql_blob(text.GetTransTgt(i)._tgt_str),
							   mysqlpp::sql_blob(text.GetTransTgt(i)._assist_str), 
							   mysqlpp::sql_blob(text.GetTransTgt(i)._assist2_str),
							   mysqlpp::sql_blob(""), 
							   mysqlpp::sql_blob(text.GetTransSrc(i).Rules()),
							   mysqlpp::sql_blob(text.GetTransTgt(i)._decode_result),
							   mysqlpp::sql_blob(text.GetTransSrc(i)._pre_proc_result),
							   text.GetTransCell(i)._para_idx.first,
							   text.GetSrcLanguage().c_str(),
							   text.GetTgtLanguage().c_str());       

			query.reset();	
			query.insert(sent);
			query.execute();
			
			unsigned long suid = query.insert_id();
			
			//设置句子ID
			text.SetSentUniqID(i, suid);
				
		}

		//Step 2.更新文件翻译表
		query << "update filetrans set transstate = \"FINISH\", transtatus = \"100%\" where guid = \"" << text.GetID() << "\"";
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::SubmitOOV(TransText & trans_text)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::submit_oov, _1, boost::ref(trans_text)));
}

int DBOperation::submit_oov(mysqlpp::Query & query, TransText & trans_text)
{
	try
	{

		for(size_t i=0; i<trans_text.GetTransSize(); ++i)
		{

			const TransTgt & trans_tgt = trans_text.GetTransTgt(i);

			//lout << "oov vec size = " << trans_tgt._oov_vec.size() << endl;
			for(size_t k=0; k<trans_tgt._oov_vec.size(); ++k)
			{
				string key = trans_tgt._oov_vec[k] + trans_text.GetDomainName() + trans_text.GetSrcLanguage() + trans_text.GetTgtLanguage();

				query << "insert ignore oovword(tid,  oovword, type, srclanguage, tgtlanguage) values(\"";
				query << key << "\", \"";
				query << trans_tgt._oov_vec[k] << "\", \"";
				query << trans_text.GetDomainName() << "\", \"";
				query << trans_text.GetSrcLanguage() << "\", \"";
				query << trans_text.GetTgtLanguage() << "\")";

				//lout << query.str() << endl;

				query.execute();

				query.reset();

				query << "insert into oovsent(oovwordid, oovsent) values(\"";
				query << key << "\", \"";
				query << mysqlpp::escape << trans_tgt._oov_sent << "\")";

				//lout << query.str() << endl;

				query.execute();
			}

		}


	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetTransResult(const TextID & tid, 
								vector< boost::tuple<string, string, string, size_t> > & result_vec, 
								string & file_type, 
								string & tgt_language)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_trans_result, _1,
													boost::ref(tid),
													boost::ref(result_vec),
													boost::ref(file_type),
													boost::ref(tgt_language)));
}

int DBOperation::get_trans_result(mysqlpp::Query & query, 
								  const TextID & tid, 
								  vector< boost::tuple<string, string, string, size_t> > & result_vec, 
								  string & file_type, 
								  string & tgt_language)
{
	try
	{
		//读取文件类型和目标端语种
		query << "select fileext, tgtlanguage from filetrans where guid = \"" << tid << "\""; //TODO 以后如果TextID类型改变 需要设置一个转换为string或其他常用类型的方法
    	
		//lout << query.str() << endl;
		vector<filetrans> res;
		query.storein(res);

		if(res.size() == 0 )
			return ERR_DB_NORESULT;

		filetrans & ft = res[0];

		file_type = ft.fileext;
		tgt_language = ft.tgtlanguage;

		//读取翻译结果句对
		query << "select SrcSent, FinalTgtSent, ModifySent, ParaIdx from transsentence where TextID = \"" << tid << "\" order by SentID asc";

		vector<transsentence> ts;
        
        query.storein(ts);
        
		for(size_t i=0; i<ts.size(); ++i)
		{
			result_vec.push_back(boost::make_tuple(ts[i].SrcSent.c_str(),  ts[i].FinalTgtSent.c_str(), ts[i].ModifySent.c_str() , ts[i].ParaIdx) );

		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetTransResultPair(const TextID & tid, 
									vector<boost::tuple<string, string, string> > & result_vec, 
									string & tgt_language)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_trans_result_pair, _1,
													boost::ref(tid),
													boost::ref(result_vec),
													boost::ref(tgt_language)));
}

int DBOperation::get_trans_result_pair(mysqlpp::Query & query, 
									   const TextID & tid, 
									   vector<boost::tuple<string, string, string> > & result_vec, 
									   string & tgt_language)
{
	try
	{
		//读取文件类型和目标端语种
		query << "select tgtlanguage from filetrans where guid = \"" << tid << "\""; 
    	
		//lout << query.str() << endl;
		vector<filetrans> res;
		query.storein(res);

		if(res.size() == 0 )
			return ERR_DB_NORESULT;

		filetrans & ft = res[0];

		tgt_language = ft.tgtlanguage;

		//读取翻译结果句对
		query << "select SrcSent, FinalTgtSent, ModifySent from transsentence where TextID = \"" << tid << "\" order by SentID asc";

		vector<transsentence> ts;
        
        query.storein(ts);
        
		for(size_t i=0; i<ts.size(); ++i)
		{
			result_vec.push_back( boost::make_tuple(ts[i].SrcSent.c_str(), ts[i].FinalTgtSent.c_str(), ts[i].ModifySent.c_str()) );

		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetTransResultDetail(const TextID & tid, 
									  vector<boost::tuple<string, string, string, string, string> > & result_vec, 
									  string & domain, 
									  string & src_language, 
									  string & tgt_language)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_trans_result_detail, _1,
													boost::ref(tid),
													boost::ref(result_vec),
													boost::ref(domain),
													boost::ref(src_language),
													boost::ref(tgt_language)));
}

int DBOperation::get_trans_result_detail(mysqlpp::Query & query, 
										 const TextID & tid, 
										 vector<boost::tuple<string, string, string, string, string> > & result_vec, 
										 string & domain, 
										 string & src_language, 
										 string & tgt_language)
{
	//define: vector<boost::tuple<string, string, string, string, string> >
	// string 0 : 前处理结果
	// string 1 : 匹配的词典和模板
	// string 2 : 解码结果
	// string 3 : 用户修改结果
	// string 4 : 最终（后处理）结果

	try
	{
		//读取文件类型和目标端语种
		query << "select domain, srclanguage, tgtlanguage from filetrans where guid = \"" << tid << "\""; 
    	
		//lout << query.str() << endl;
		vector<filetrans> res;
		query.storein(res);

		if(res.size() == 0 )
			return ERR_DB_NORESULT;

		filetrans & ft = res[0];

		domain = ft.type;
		src_language = ft.srclanguage;
		tgt_language = ft.tgtlanguage;

		//读取翻译结果句对
		query << "select PreSent, RulesRes, DecodeSent, ModifySent, FinalTgtSent from transsentence where TextID = \"" << tid << "\" order by SentID asc";

		vector<transsentence> ts;
        
        query.storein(ts);
        
		for(size_t i=0; i<ts.size(); ++i)
		{
			result_vec.push_back( boost::make_tuple(ts[i].PreSent.c_str(), ts[i].RulesRes.c_str(), ts[i].DecodeSent.c_str(), ts[i].ModifySent.c_str(), ts[i].FinalTgtSent.c_str()) );

		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}



int DBOperation::ReportTransPercent(const TextID & tid, const size_t percent)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::report_trans_percent, _1,
													boost::ref(tid),
													boost::ref(percent)));
}

int DBOperation::report_trans_percent(mysqlpp::Query & query, const TextID & tid, const size_t percent)
{
	try
	{

		size_t update_percent = (percent >= 100) ? 100 : percent;
		query << "UPDATE filetrans SET transtatus='" << update_percent << "%' WHERE guid = '" << tid << "'";
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::ReportTransState(const TextID & tid, const string & state)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::report_trans_state, _1,
													boost::ref(tid),
													boost::ref(state)));
}

int DBOperation::report_trans_state(mysqlpp::Query & query, const TextID & tid, const string & state)
{
	try
	{
		query << "UPDATE filetrans SET transstate='" << state << "' WHERE guid = '" << tid << "'";
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::ReportTransStateAndPercent(const TextID & tid, const string & state, const size_t percent)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::report_trans_state_and_percent, _1,
													boost::ref(tid),
													boost::ref(state),
													boost::ref(percent)));
}

int DBOperation::report_trans_state_and_percent(mysqlpp::Query & query, const TextID & tid, const string & state, const size_t percent)
{
	try
	{
		query << "UPDATE filetrans SET transstate='" << state << "', transtatus='" << percent << "%' WHERE guid = '" << tid << "'";
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::ReportTransError(const TextID & tid, const int err_code, const string & state)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::report_trans_error, _1,
													boost::ref(tid),
													boost::ref(err_code),
													boost::ref(state)));
}

int DBOperation::report_trans_error(mysqlpp::Query & query, const TextID & tid, const int err_code, const string & state)
{
	try
	{
		query << "UPDATE filetrans SET errorcode=" << err_code << ", transstate = '" << state << "'" << " WHERE guid = '" << tid << "'";
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::UpdateTransInfo(const TextID & tid, const string & domain, const string & src_language, const string & tgt_language, const size_t words_num)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::update_trans_info, _1,
													boost::ref(tid),
													boost::ref(domain),
													boost::ref(src_language),
													boost::ref(tgt_language),
													boost::ref(words_num)));
}

int DBOperation::update_trans_info(mysqlpp::Query & query, const TextID & tid, const string & domain, const string & src_language, const string & tgt_language, const size_t words_num)
{
	try
	{
		//query << "update filetrans set type=\"" << domain << "\", srclanguage=\"" << src_language << "\", tgtlanguage=\"" << tgt_language << "\", wordsNum=" << words_num << " where guid = \"" << tid << "\"";
		query << "update filetrans set type=\"" << domain << "\", srclanguage=\"" << src_language << "\", tgtlanguage=\"" << tgt_language << "\" where guid = \"" << tid << "\"";
		//lout << "update : " << query.str() << endl;
		query.execute();


	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetMaxDictWordID(DictID & dict_id, WordID & word_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_max_dict_wordid, _1,
													boost::ref(dict_id),
													boost::ref(word_id)));
}

int DBOperation::get_max_dict_wordid(mysqlpp::Query & query, DictID & dict_id, WordID & word_id)
{
	try
	{
		query << "select tid from dictinfo order by tid desc limit 0,1";
		vector<dictinfo> dict_res;
		query.storein(dict_res);

		if(dict_res.size() == 0 )
		{
			dict_id = 0;
		}else
		{
			dict_id = dict_res[0].tid;
		}

		lout << "dict_id = " << dict_id << endl;

		query.reset();

		query << "select tid from dictiteminfo order by tid desc limit 0,1";
		vector<dictiteminfo> item_res;
		query.storein(item_res);

		if(item_res.size() == 0 )
		{
			word_id = 0;
		}else
		{
			word_id = item_res[0].tid;
		}

		lout << "word_id = " << word_id << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::InsertWord(const DictID dict_id, const WordInfo & word_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::insert_word, _1,
													boost::ref(dict_id),
													boost::ref(word_info)));
}

int DBOperation::insert_word(mysqlpp::Query & query, const DictID dict_id, const WordInfo & word_info)
{
	try
	{
		query << "insert into dictiteminfo(tid, dictid, src, tgt, isdeleted, isactive, ischecked, createtime) values (" ;
		query << word_info.word_id << ", ";
		query << dict_id << ", \"" ;
		query << mysqlpp::escape << word_info.src << "\", \""; 
		query << mysqlpp::escape << word_info.tgt << "\", " ;
		query << word_info.is_deleted << ", " ;
		query << word_info.is_active << ", ";
		query << word_info.is_checked << ", ";
		query << "now() )";

		//lout << query.str() << endl;
		
        query.execute();

		//获得insertid
		//word_info.word_id = query.insert_id();

		//lout << "Get word insert id = " << word_info.word_id << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}


	return SUCCESS;
}

int DBOperation::InsertWord(const DictID dict_id, const vector<WordInfo> & word_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::insert_word_group, _1,
													boost::ref(dict_id),
													boost::ref(word_info_vec))); 
}

int DBOperation::insert_word_group(mysqlpp::Query & query, const DictID dict_id, const vector<WordInfo> & word_info_vec)
{
	if(word_info_vec.size() == 0)
		return SUCCESS;

	try
	{
		int count = 0;
		int tag = 1;
		int group_num = word_info_vec.size() / GROUPNUM;

		if(word_info_vec.size() < GROUPNUM)
			group_num = 1;
		else if( word_info_vec.size() % GROUPNUM > 0)
			group_num += 1;

		while(tag <= group_num)
		{
			query << "insert into dictiteminfo(tid, dictid, src, tgt, isdeleted, isactive, ischecked, createtime) values ";

			for(size_t i=0; i<GROUPNUM && count+i < word_info_vec.size(); i++)
			{
				query << "(";
				query << word_info_vec[count+i].word_id << ", ";
				query << dict_id << ", \"" ;
				query << mysqlpp::escape << word_info_vec[count+i].src << "\", \""; 
				query << mysqlpp::escape << word_info_vec[count+i].tgt << "\", " ;
				query << word_info_vec[count+i].is_deleted << ", " ;
				query << word_info_vec[count+i].is_active << ", ";
				query << word_info_vec[count+i].is_checked << ", ";

				if(tag == group_num)
				{
					if(word_info_vec.size()-1 == i+count)
					{
						query << "now() )";
						break;
					}
					else
					{
						query << "now() )";
						query << ", ";
					}
				}
				else
				{
					if(GROUPNUM-1 == i)
					{
						query << "now() )";
					}
					else
					{
						query << "now() )";
						query << ", ";
					}
				}
			}
			
			//lout << query.str() << endl;
			query.execute();
			query.reset();

			count += GROUPNUM;
			tag++;
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}

	return SUCCESS;	
}

int DBOperation::DeleteWord(const WordID word_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::delete_word, _1,
													boost::ref(word_id))); 
}

int DBOperation::delete_word(mysqlpp::Query & query, const WordID word_id)
{
	try
	{
		query << "update dictiteminfo set isdeleted=1 where tid = " << word_id;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::ModifyWord(const WordInfo & word_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::modify_word, _1,
													boost::ref(word_info))); 
}

int DBOperation::modify_word(mysqlpp::Query & query, const WordInfo & word_info)
{
	try
	{
		query << "update dictiteminfo set src=\"" << mysqlpp::escape << word_info.src << "\", tgt=\"" << mysqlpp::escape << word_info.tgt << "\", isactive=" << word_info.is_active << ", ischecked=" << word_info.is_checked  << "  where tid = " << word_info.word_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::CreateDict(DictInfo & dict_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::create_dict, _1,
													boost::ref(dict_info))); 
}

int DBOperation::create_dict(mysqlpp::Query & query, DictInfo & dict_info)
{
	dict_info.is_deleted = 0;
	//入数据库之前转换为UTF-8

	//lout << "Before conv name[" << dict_info.dict_name << "] , description[" << dict_info.description << "]" << endl;
	string dict_name = dict_info.dict_name;
	string description = dict_info.description;

	try
	{
		//dictinfo dinfo(0,
		//			   dict_info.usr_id,
		//			   dict_info.dict_name,
		//			   dict_info.is_deleted,
		//			   dict_info.is_active,
		//			   dict_info.description,
		//			   dict_info.domain_info.first,
		//			   dict_info.domain_info.second.first,
		//			   dict_info.domain_info.second.second);
		
		//执行插入
		query << "insert into dictinfo(tid, username, dictname, isdeleted, isactive, issystem, description, type, srclanguage, tgtlanguage, createtime) values (" ;
		query << "\"" << dict_info.dict_id << "\", " ;
		query << "\"" << dict_info.usr_id << "\", " ;
		query << "\"" << mysqlpp::escape << dict_name << "\", "; 
		query << dict_info.is_deleted << ", " ;
		query << dict_info.is_active << ", ";
		query << dict_info.type << ", ";
		query << "\"" << mysqlpp::escape << description << "\", "; 
		query << "\"" << dict_info.domain_info.first << "\", "; 
		query << "\"" << dict_info.domain_info.second.first << "\", "; 
		query << "\"" << dict_info.domain_info.second.second << "\", "; 
		query << "now() )";

		lout << query.str() << endl;

        query.execute();

		//获得insertid
		dict_info.dict_id = query.insert_id();
		lout << "Get dict insert id = " << dict_info.dict_id << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception 111: " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}

	
	return SUCCESS;
}

int DBOperation::ModifyDict(const DictInfo & dict_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::modify_dict, _1,
													boost::ref(dict_info))); 
}

int DBOperation::modify_dict(mysqlpp::Query & query, const DictInfo & dict_info)
{
	try
	{
		string dict_name = dict_info.dict_name;
		string description = dict_info.description;

		query << "update dictinfo set dictname=\"" << mysqlpp::escape << dict_name;
		query << "\", description=\"" << mysqlpp::escape << description;
		query << "\", isactive=" << dict_info.is_active;
		query << ", issystem=" << dict_info.type;
		query << ", type=\"" << dict_info.domain_info.first;
		query << "\", srclanguage=\"" << dict_info.domain_info.second.first;
		query << "\", tgtlanguage=\"" << dict_info.domain_info.second.second;
		query << "\"  where tid = " << dict_info.dict_id;

		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::DeleteDict(const DictID & dict_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::delete_dict, _1,
													boost::ref(dict_id))); 
}

int DBOperation::delete_dict(mysqlpp::Query & query, const DictID & dict_id)
{
	try
	{
		query << "update dictinfo set isdeleted=1 where tid = " << dict_id;
		//lout << query.str() << endl;
		query.execute();

		query.reset();
		query << "update dictiteminfo set isdeleted=1 where dictid = " << dict_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::LoadAllDict(vector<DictInfo> & dict_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::load_all_dict, _1,
													boost::ref(dict_info_vec)));
}

int DBOperation::load_all_dict(mysqlpp::Query & query, vector<DictInfo> & dict_info_vec)
{
	dict_info_vec.clear();

	try
	{
		query << "select * from dictinfo where isdeleted != 1 ";
		vector<dictinfo> dict_res;
		query.storein(dict_res);

		for(size_t i=0; i<dict_res.size(); ++i)
		{
			dictinfo & dinfo = dict_res[i];

			DictInfo info;

			info.dict_id = dinfo.tid;
			info.usr_id = dinfo.username;
			info.dict_name = dinfo.dictname.c_str();
			info.is_deleted = dinfo.isdeleted;
			info.is_active = dinfo.isactive;
			info.type = dinfo.issystem;
			info.description = dinfo.description.c_str();
			info.domain_info.first = dinfo.type;
			info.domain_info.second.first = dinfo.srclanguage;
			info.domain_info.second.second = dinfo.tgtlanguage;

			

			dict_info_vec.push_back(info);
		}

		//test
		//cout << dict_info_vec.size() << "   sssssssssss" << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::LoadAllWord(vector<WordInfo> & word_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::load_all_word, _1,
													boost::ref(word_info_vec)));
}

int DBOperation::load_all_word(mysqlpp::Query & query, vector<WordInfo> & word_info_vec)
{
	word_info_vec.clear();

	try
	{
		query << "select * from dictiteminfo where isdeleted != 1";
		vector<dictiteminfo> item_res;
		query.storein(item_res);

		for(size_t i=0; i<item_res.size(); ++i)
		{
			dictiteminfo & item = item_res[i];

			WordInfo info;

			info.word_id = item.tid;
			info.dict_id = item.dictid;
			info.src = UyLower::to_lower(item.src.c_str());
			info.tgt = item.tgt.c_str();
			info.is_deleted = item.isdeleted;
			info.is_active = item.isactive;

			info.is_checked = item.ischecked;
			

			word_info_vec.push_back(info);
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::LoadWord(const vector<WordID> & word_id_vec, vector<WordInfo> & word_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::load_word, _1,
													boost::ref(word_id_vec),
													boost::ref(word_info_vec)));
}

int DBOperation::load_word(mysqlpp::Query & query, const vector<WordID> & word_id_vec, vector<WordInfo> & word_info_vec)
{
	word_info_vec.clear();

	try
	{
		for(size_t i=0; i<word_id_vec.size(); i++)
		{
			query << "select * from dictiteminfo where tid = " << word_id_vec[i];
			vector<dictiteminfo> item;
			query.storein(item);

			WordInfo info;

			info.word_id = item[0].tid;
			info.src = UyLower::to_lower(item[0].src.c_str());
			info.tgt = item[0].tgt.c_str();
			info.is_deleted = item[0].isdeleted;
			info.is_active = item[0].isactive;

			info.is_checked = item[0].ischecked;


			word_info_vec.push_back(info);
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::ModifyImportStatus(const vector<size_t> & import_word_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::modify_import_status, _1,
													boost::ref(import_word_id)));
}

int DBOperation::modify_import_status(mysqlpp::Query & query, const vector<size_t> & import_word_id)
{
	try
	{
		for(size_t i=0; i<import_word_id.size(); i++)
		{
			query << "update dictimport set status=2 , isdeleted=1 where fromid = " << import_word_id[i];
			//lout << query.str() << endl;
			query.execute();
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetRecoverDictWordInfo(const DictID & dict_id, vector<WordInfo> & word_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_recover_dict_word_info, _1,
													boost::ref(dict_id),
													boost::ref(word_info_vec)));
}

int DBOperation::get_recover_dict_word_info(mysqlpp::Query & query, const DictID & dict_id, vector<WordInfo> & word_info_vec)
{
	try
	{
		query << "select * from dictiteminfo where dictid = " << dict_id;
		vector<dictiteminfo> item;
		query.storein(item);

		if(item.size() != 0)
		{
			WordInfo info;
			for(size_t i=0; i<item.size(); i++)
			{
				info.word_id = item[i].tid;
				info.src = item[i].src.c_str();
				info.tgt = item[i].tgt.c_str();
				info.is_deleted = item[i].isdeleted;
				info.is_active = item[i].isactive;

				info.is_checked = item[i].ischecked;


				word_info_vec.push_back(info);
			}
		}

		return ERR_NOWORD_GET;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetRecoverDictInfo(const DictID & dict_id, DictInfo & dict_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_recover_dict_info, _1,
													boost::ref(dict_id),
													boost::ref(dict_info)));
}

int DBOperation::get_recover_dict_info(mysqlpp::Query & query, const DictID & dict_id, DictInfo & dict_info)
{
	try
	{	
		query << "select * from dictinfo where tid = " << dict_id;
		vector<dictinfo> dict_res;
		query.storein(dict_res);	
		
		if(dict_res.size() != 0)
		{	
			dictinfo & dinfo = dict_res[0];

			dict_info.dict_id = dinfo.tid;
			dict_info.usr_id = dinfo.username;
			dict_info.dict_name = dinfo.dictname.c_str();
			dict_info.is_deleted = dinfo.isdeleted;
			dict_info.is_active = dinfo.isactive;
			dict_info.type = dinfo.issystem;
			dict_info.description = dinfo.description.c_str();
			dict_info.domain_info.first = dinfo.type;
			dict_info.domain_info.second.first = dinfo.srclanguage;
			dict_info.domain_info.second.second = dinfo.tgtlanguage;		
		}
		
		return ERR_NODICT_GET;
	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetRecoverWordInfo(const WordID & word_id, WordInfo & word_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_recover_word_info, _1,
													boost::ref(word_id),
													boost::ref(word_info)));
}

int DBOperation::get_recover_word_info(mysqlpp::Query & query, const WordID & word_id, WordInfo & word_info)
{
	try
	{	
		query << "select * from dictiteminfo where tid = " << word_id;
		vector<dictiteminfo> item;
		query.storein(item);

		if(item.size() != 0)
		{
			word_info.word_id = item[0].tid;
			word_info.src = item[0].src.c_str();
			word_info.tgt = item[0].tgt.c_str();
			word_info.is_deleted = item[0].isdeleted;
			word_info.is_active = item[0].isactive;

			word_info.is_checked = item[0].ischecked;
		}
		
		return ERR_NOWORD_GET;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::RecoverDict(const DictID & dict_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::recover_dict, _1,
													boost::ref(dict_id)));
}

int DBOperation::recover_dict(mysqlpp::Query & query, const DictID & dict_id)
{
	try
	{
		query << "update dictinfo set isdeleted=0 where tid = " << dict_id;
		//lout << query.str() << endl;
		query.execute();

		query.reset();
		query << "update dictiteminfo set isdeleted=0 where dictid = " << dict_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::RecoverWord(const WordID & word_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::recover_word, _1,
													boost::ref(word_id)));
}

int DBOperation::recover_word(mysqlpp::Query & query, const WordID & word_id)
{
	try
	{
		query << "update dictiteminfo set isdeleted=0 where tid = " << word_id;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}
	
	return SUCCESS;
}

int DBOperation::InsertAfterWord(const AfterDictID dict_id, AfterWordInfo & word_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::insert_after_word, _1,
													boost::ref(dict_id),
													boost::ref(word_info)));
}

int DBOperation::insert_after_word(mysqlpp::Query & query, const AfterDictID dict_id, AfterWordInfo & word_info)
{
	string word_src = word_info.src;
	string word_tgt = word_info.tgt;
	
	try
	{	
		//执行插入
		query << "insert into afterdictiteminfo(tid, afterdictid, src, tgt, isdeleted, isactive, createtime) values (" ;
		query << word_info.word_id << ", ";
		query << dict_id << ", \"" ;
		query << mysqlpp::escape << word_src << "\", \""; 
		query << mysqlpp::escape << word_tgt << "\", " ;
		query << word_info.is_deleted << ", " ;
		query << word_info.is_active << ", ";
		query << "now() )";

		//lout << query.str() << endl;
		
        query.execute();

		//获得insertid
		//word_info.word_id = query.insert_id();

		//lout << "Get word insert id = " << word_info.word_id << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}

	
	return SUCCESS;
}

int DBOperation::InsertAfterWordGroup(const AfterDictID dict_id, const vector<bool> & filter_vec, vector<AfterWordInfo> & word_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::insert_after_word_group, _1,
													boost::ref(dict_id),
													boost::ref(filter_vec),
													boost::ref(word_info_vec)));
}

int DBOperation::insert_after_word_group(mysqlpp::Query & query, const AfterDictID dict_id, const vector<bool> & filter_vec, vector<AfterWordInfo> & word_info_vec)
{
	if(filter_vec.size() != word_info_vec.size())
		return ERR_DB_WORD_VEC_NOT_RIGHT;

	int flag = 1;

	if(0 == filter_vec.size())
	{
		return SUCCESS;
	}

	try
	{
		int count = 0;
		int tag = 1;
		int group_num = word_info_vec.size() / GROUPNUM;

		if(word_info_vec.size() < GROUPNUM)
			group_num = 1;
		else if( word_info_vec.size() % GROUPNUM > 0)
			group_num += 1;

		while(tag <= group_num)
		{
			query << "insert into afterdictiteminfo(tid, afterdictid, src, tgt, isdeleted, isactive, createtime) values ";

			for(size_t i=0; i<GROUPNUM && count+i < word_info_vec.size(); i++)
			{
				if(true == filter_vec[count+i])
				{
					query << "(";
					query << word_info_vec[count+i].word_id << ", ";
					query << dict_id << ", \"" ;
					query << mysqlpp::escape << word_info_vec[count+i].src << "\", \""; 
					query << mysqlpp::escape << word_info_vec[count+i].tgt << "\", " ;
					query << word_info_vec[count+i].is_deleted << ", " ;
					query << word_info_vec[count+i].is_active << ", ";
					//query << word_info_vec[count+i].is_checked << ", ";

					if(tag == group_num)
					{
						if(word_info_vec.size()-1 == i+count)
						{
							query << "now() )";
							break;
						}
						else
						{
							query << "now() )";
							query << ", ";
						}
					}
					else
					{
						if(GROUPNUM-1 == i)
						{
							query << "now() )";
						}
						else
						{
							query << "now() )";
							query << ", ";
						}
					}
				}
			}
			
			//lout << query.str() << endl;
			query.execute();

			query.reset();

			count += GROUPNUM;
			tag++;
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}

	return SUCCESS;	
}

int DBOperation::DeleteAfterWord(const AfterWordID word_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::delete_after_word, _1,
													boost::ref(word_id)));
}

int DBOperation::delete_after_word(mysqlpp::Query & query, const AfterWordID word_id)
{
	try
	{
		query << "update afterdictiteminfo set isdeleted=1 where tid = " << word_id;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}
	
	return SUCCESS;
}

int DBOperation::ModifyAfterWord(const AfterWordInfo & word_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::modify_after_word, _1,
													boost::ref(word_info)));
}

int DBOperation::modify_after_word(mysqlpp::Query & query, const AfterWordInfo & word_info)
{
	try
	{
		string src = word_info.src;
		string tgt = word_info.tgt;
		
		query << "update afterdictiteminfo set src=\"" << mysqlpp::escape << src << "\", tgt=\"" << mysqlpp::escape << tgt << "\", isactive=" << word_info.is_active << "  where tid = " << word_info.word_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::CreateAfterDict(AfterDictInfo & dict_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::create_after_dict, _1,
													boost::ref(dict_info)));
}

int DBOperation::create_after_dict(mysqlpp::Query & query, AfterDictInfo & dict_info)
{
	dict_info.is_deleted = 0;
	
	//入数据库之前转换为UTF-8

	//lout << "Before conv name[" << dict_info.dict_name << "] , description[" << dict_info.description << "]" << endl;
	string dict_name = dict_info.dict_name;
	string description = dict_info.description;

	try
	{
		//dictinfo dinfo(0,
		//			   dict_info.usr_id,
		//			   dict_info.dict_name,
		//			   dict_info.is_deleted,
		//			   dict_info.is_active,
		//			   dict_info.description,
		//			   dict_info.domain_info.first,
		//			   dict_info.domain_info.second.first,
		//			   dict_info.domain_info.second.second);
		
		//执行插入
		query << "insert into afterdictinfo(username, dictname, isdeleted, isactive, description, type, srclanguage, tgtlanguage, createtime) values (" ;
		query << "\"" << dict_info.usr_id << "\", " ;
		query << "\"" << mysqlpp::escape << dict_name << "\", "; 
		query << dict_info.is_deleted << ", " ;
		query << dict_info.is_active << ", ";
		query << "\"" << mysqlpp::escape << description << "\", "; 
		query << "\"" << dict_info.domain_info.first << "\", "; 
		query << "\"" << dict_info.domain_info.second.first << "\", "; 
		query << "\"" << dict_info.domain_info.second.second << "\", "; 
		query << "now() )";

		//lout << query.str() << endl;

        query.execute();

		//获得insertid
		dict_info.dict_id = query.insert_id();
		//lout << "Get dict insert id = " << dict_info.dict_id << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}

	
	return SUCCESS;
}

int DBOperation::ModifyAfterDict(const AfterDictInfo & dict_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::modify_after_dict, _1,
													boost::ref(dict_info)));
}

int DBOperation::modify_after_dict(mysqlpp::Query & query, const AfterDictInfo & dict_info)
{
	try
	{
		string dict_name = dict_info.dict_name;
		string description = dict_info.description;

		query << "update afterdictinfo set dictname=\"" << mysqlpp::escape << dict_name ;
		query << "\", description=\"" << mysqlpp::escape << description ;
		query << "\", isactive=" << dict_info.is_active ;
		query << ", type=\"" << dict_info.domain_info.first ;
		query << "\", srclanguage=\"" << dict_info.domain_info.second.first;
		query << "\", tgtlanguage=\"" << dict_info.domain_info.second.second;
		query << "\"  where tid = " << dict_info.dict_id;

		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::DeleteAfterDict(const AfterDictInfo & dict_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::delete_after_dict, _1,
													boost::ref(dict_info)));
}

int DBOperation::delete_after_dict(mysqlpp::Query & query, const AfterDictInfo & dict_info)
{
	try
	{
		query << "update afterdictinfo set isdeleted=1 where tid = " << dict_info.dict_id;
		//lout << query.str() << endl;
		query.execute();

		query.reset();
		query << "update afterdictiteminfo set isdeleted=1 where afterdictid = " << dict_info.dict_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::LoadAllAfterDict(vector<AfterDictInfo> & dict_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::load_all_after_dict, _1,
													boost::ref(dict_info_vec)));
}

int DBOperation::load_all_after_dict(mysqlpp::Query & query, vector<AfterDictInfo> & dict_info_vec)
{
	dict_info_vec.clear();

	try
	{
		query << "select * from afterdictinfo where isdeleted != 1 ";
		vector<dictinfo> dict_res;
		query.storein(dict_res);

		for(size_t i=0; i<dict_res.size(); ++i)
		{
			dictinfo & dinfo = dict_res[i];

			AfterDictInfo info;

			info.dict_id = dinfo.tid;
			info.usr_id = dinfo.username;
			info.dict_name = dinfo.dictname.c_str();
			info.is_deleted = dinfo.isdeleted;
			info.is_active = dinfo.isactive;
			info.description = dinfo.description.c_str();
			info.domain_info.first = dinfo.type;
			info.domain_info.second.first = dinfo.srclanguage;
			info.domain_info.second.second = dinfo.tgtlanguage;

			

			dict_info_vec.push_back(info);
		}

		//test
		//cout << dict_info_vec.size() << "   sssssssssss" << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}
	
	return SUCCESS;
}

int DBOperation::LoadAfterWord(const AfterDictInfo & dict_info, vector<AfterWordInfo> & word_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::load_after_word, _1,
													boost::ref(dict_info),
													boost::ref(word_info_vec)));
}

int DBOperation::load_after_word(mysqlpp::Query & query, const AfterDictInfo & dict_info, vector<AfterWordInfo> & word_info_vec)
{
	word_info_vec.clear();

	try
	{
		query << "select * from afterdictiteminfo where isdeleted != 1 and afterdictid = " << dict_info.dict_id;
		vector<afterdictiteminfo> item_res;
		query.storein(item_res);

		for(size_t i=0; i<item_res.size(); ++i)
		{
			afterdictiteminfo & item = item_res[i];

			AfterWordInfo info;

			info.word_id = item.tid;
			info.src = item.src.c_str();
			info.tgt = item.tgt.c_str();
			info.is_deleted = item.isdeleted;
			info.is_active = item.isactive;

			word_info_vec.push_back(info);
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetRecoverAfterDictWordInfo(const AfterDictID & dict_id, vector<AfterWordInfo> & dict_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_recover_after_dict_word_info, _1,
													boost::ref(dict_id),
													boost::ref(dict_vec)));
}

int DBOperation::get_recover_after_dict_word_info(mysqlpp::Query & query, const AfterDictID & dict_id, vector<AfterWordInfo> & dict_vec)
{
	try
	{
		query << "select * from afterdictiteminfo where afterdictid = " << dict_id;
		vector<afterdictiteminfo> item;
		query.storein(item);

		if(item.size() != 0)
		{
			AfterWordInfo info;
			for(size_t i=0; i<item.size(); i++)
			{
				info.word_id = item[i].tid;
				info.src = item[i].src.c_str();
				info.tgt = item[i].tgt.c_str();
				info.is_deleted = item[i].isdeleted;
				info.is_active = item[i].isactive;


				dict_vec.push_back(info);
			}
		}

		return ERR_NOWORD_GET;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetRecoverAfterDictInfo(const AfterDictID & dict_id, AfterDictInfo & dict_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_recover_after_dict_info, _1,
													boost::ref(dict_id),
													boost::ref(dict_info)));
}

int DBOperation::get_recover_after_dict_info(mysqlpp::Query & query, const AfterDictID & dict_id, AfterDictInfo & dict_info)
{
	try
	{	
		query << "select * from afterdictinfo where tid = " << dict_id;
		vector<afterdictinfo> dict_res;
		query.storein(dict_res);	
		
		if(dict_res.size() != 0)
		{	
			afterdictinfo & dinfo = dict_res[0];

			dict_info.dict_id = dinfo.tid;
			dict_info.usr_id = dinfo.username;
			dict_info.dict_name = dinfo.dictname.c_str();
			dict_info.is_deleted = dinfo.isdeleted;
			dict_info.is_active = dinfo.isactive;
			dict_info.description = dinfo.description.c_str();
			dict_info.domain_info.first = dinfo.type;
			dict_info.domain_info.second.first = dinfo.srclanguage;
			dict_info.domain_info.second.second = dinfo.tgtlanguage;		
		}
		
		return ERR_NODICT_GET;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetRecoverAfterWordInfo(const AfterWordID & word_id, AfterWordInfo & word_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_recover_after_word_info, _1,
													boost::ref(word_id),
													boost::ref(word_info)));
}

int DBOperation::get_recover_after_word_info(mysqlpp::Query & query, const AfterWordID & word_id, AfterWordInfo & word_info)
{
	try
	{	
		query << "select * from afterdictiteminfo where tid = " << word_id;
		vector<afterdictiteminfo> item;
		query.storein(item);

		if(item.size() != 0)
		{
			word_info.word_id = item[0].tid;
			word_info.src = item[0].src.c_str();
			word_info.tgt = item[0].tgt.c_str();
			word_info.is_deleted = item[0].isdeleted;
			word_info.is_active = item[0].isactive;
		}
		
		return ERR_NOWORD_GET;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::RecoverAfterDict(const AfterDictID & dict_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::recover_after_dict, _1,
													boost::ref(dict_id)));
}

int DBOperation::recover_after_dict(mysqlpp::Query & query, const AfterDictID & dict_id)
{
	try
	{
		query << "update afterdictinfo set isdeleted=0 where tid = " << dict_id;
		//lout << query.str() << endl;
		query.execute();

		query.reset();
		query << "update afterdictiteminfo set isdeleted=0 where afterdictid = " << dict_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::RecoverAfterWord(const AfterWordID & word_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::recover_after_word, _1,
													boost::ref(word_id)));
}

int DBOperation::recover_after_word(mysqlpp::Query & query, const AfterWordID & word_id)
{
	try
	{
		query << "update afterdictiteminfo set isdeleted=0 where tid = " << word_id;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::InsertTemplate(const TemplateLibID templatelib_id, TemplateInfo & template_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::insert_template, _1,
													boost::ref(templatelib_id),
													boost::ref(template_info)));
}

int DBOperation::insert_template(mysqlpp::Query & query, const TemplateLibID templatelib_id, TemplateInfo & template_info)
{
	template_info.is_deleted = 0;

	//入数据库之前转换为UTF-8
	string src = template_info.src;
	string tgt = template_info.tgt;

	try
	{
		//执行插入
		query << "insert into templateiteminfo(tid, templatelib_id, src, tgt, isdeleted, isactive, ischecked, createtime) values (" ;
		query << template_info.template_id << ", ";
		query << templatelib_id << ", \"" ;
		query << mysqlpp::escape << src << "\", \""; 
		query << mysqlpp::escape << tgt << "\", " ;
		query << template_info.is_deleted << ", " ;
		query << template_info.is_active << ", ";
		query << template_info.is_checked << ", ";
		query << "now() )";

		//lout << query.str() << endl;
		
        query.execute();

		//获得insertid
		//template_info.template_id = query.insert_id();

		//lout << "Get word insert id = " << word_info.word_id << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}

	
	return SUCCESS;
}

int DBOperation::InsertTemplateGroup(const TemplateLibID templatelib_id, const vector<bool> & filter_vec, vector<TemplateInfo> & template_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::insert_template_group, _1,
													boost::ref(templatelib_id),
													boost::ref(filter_vec),
													boost::ref(template_info_vec)));
}

int DBOperation::insert_template_group(mysqlpp::Query & query, const TemplateLibID templatelib_id, const vector<bool> & filter_vec, vector<TemplateInfo> & template_info_vec)
{
	if(filter_vec.size() != template_info_vec.size())
		return ERR_DB_TEMPLATE_VEC_NOT_RIGHT;
	
	int flag=1;
	if(0 == filter_vec.size())
	{
		return SUCCESS;
	}

	try
	{
		int count = 0;
		int tag = 1;
		int group_num = template_info_vec.size() / GROUPNUM;

		if(template_info_vec.size() < GROUPNUM)
			group_num = 1;
		else if( template_info_vec.size() % GROUPNUM > 0)
			group_num += 1;

		while(tag <= group_num)
		{
			query << "insert into templateiteminfo(tid, templatelib_id, src, tgt, isdeleted, isactive, ischecked, createtime) values ";

			for(size_t i=0; i<GROUPNUM && count+i < template_info_vec.size(); i++)
			{
				if(true == filter_vec[count+i])
				{
					query << "(";
					query << template_info_vec[count+i].template_id << ", ";
					query << templatelib_id << ", \"" ;
					query << mysqlpp::escape << template_info_vec[count+i].src << "\", \""; 
					query << mysqlpp::escape << template_info_vec[count+i].tgt << "\", " ;
					query << template_info_vec[count+i].is_deleted << ", " ;
					query << template_info_vec[count+i].is_active << ", ";
					query << template_info_vec[count+i].is_checked << ", ";

					if(tag == group_num)
					{
						if(template_info_vec.size()-1 == i+count)
						{
							query << "now() )";
							break;
						}
						else
						{
							query << "now() )";
							query << ", ";
						}
					}
					else
					{
						if(GROUPNUM-1 == i)
						{
							query << "now() )";
						}
						else
						{
							query << "now() )";
							query << ", ";
						}
					}
				}
			}
			
			//lout << query.str() << endl;
			query.execute();
			query.reset();

			count += GROUPNUM;
			tag++;
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}


	return SUCCESS;	
}

int DBOperation::DeleteTemplate(const TemplateID template_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::delete_template, _1,
													boost::ref(template_id)));
}

int DBOperation::delete_template(mysqlpp::Query & query, const TemplateID template_id)
{
	try
	{
		query << "update templateiteminfo set isdeleted=1 where tid = " << template_id;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}


int DBOperation::ModifyTemplate(const TemplateInfo & template_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::modify_template, _1,
													boost::ref(template_info)));
}

int DBOperation::modify_template(mysqlpp::Query & query, const TemplateInfo & template_info)
{
	try
	{
		string src = template_info.src;
		string tgt = template_info.tgt;
		
		query << "update templateiteminfo set src=\"" << mysqlpp::escape << src << "\", tgt=\"" << mysqlpp::escape << tgt << "\", isactive=" << template_info.is_active << ", ischecked=" << template_info.is_checked << "  where tid = " << template_info.template_id;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::CreateTemplateLib(TemplateLibInfo & templatelib_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::create_template_lib, _1,
													boost::ref(templatelib_info)));
}

int DBOperation::create_template_lib(mysqlpp::Query & query, TemplateLibInfo & templatelib_info)
{
	templatelib_info.is_deleted = 0;

	//入数据库之前转换为UTF-8
	string name = templatelib_info.templatelib_name;
	string description = templatelib_info.description;

	try
	{
		//执行插入
		query << "insert into templatelibinfo(username, templatelibname, isdeleted, isactive, issystem, description, type, srclanguage, tgtlanguage, createtime) values (" ;
		query << "\"" << templatelib_info.usr_id << "\", " ;
		query << "\"" << mysqlpp::escape << name << "\", "; 
		query << templatelib_info.is_deleted << ", " ;
		query << templatelib_info.is_active << ", ";
		query << templatelib_info.is_system << ", ";
		query << "\"" << mysqlpp::escape << description << "\", "; 
		query << "\"" << templatelib_info.domain_info.first << "\", "; 
		query << "\"" << templatelib_info.domain_info.second.first << "\", "; 
		query << "\"" << templatelib_info.domain_info.second.second << "\", "; 
		query << "now() )";

		//lout << query.str() << endl;

        query.execute();

		//获得insertid
		templatelib_info.templatelib_id = query.insert_id();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}

	
	return SUCCESS;
}

int DBOperation::ModifyTemplateLib(const TemplateLibInfo & templatelib_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::modify_template_lib, _1,
													boost::ref(templatelib_info)));
}

int DBOperation::modify_template_lib(mysqlpp::Query & query, const TemplateLibInfo & templatelib_info)
{
	try
	{
		string name = templatelib_info.templatelib_name;
		string description = templatelib_info.description;

		query << "update templatelibinfo set templatelibname=\"" << mysqlpp::escape << name ;
		query << "\", description=\"" << mysqlpp::escape << description ;
		query << "\", isactive=" << templatelib_info.is_active ;
		query << ", type=\"" << templatelib_info.domain_info.first ;
		query << "\", srclanguage=\"" << templatelib_info.domain_info.second.first;
		query << "\", tgtlanguage=\"" << templatelib_info.domain_info.second.second;
		query << "\"  where tid = " << templatelib_info.templatelib_id;

		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::DeleteTemplateLib(const TemplateLibInfo & templatelib_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::delete_template_lib, _1,
													boost::ref(templatelib_info)));
}

int DBOperation::delete_template_lib(mysqlpp::Query & query, const TemplateLibInfo & templatelib_info)
{
	try
	{
		query << "update templatelibinfo set isdeleted=1 where tid = " << templatelib_info.templatelib_id;
		//lout << query.str() << endl;
		query.execute();

		query.reset();
		query << "update templateiteminfo set isdeleted=1 where templatelib_id = " << templatelib_info.templatelib_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}
	
	return SUCCESS;
}


int DBOperation::LoadAllTemplateLib(vector<TemplateLibInfo> & templatelib_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::load_all_template_lib, _1,
													boost::ref(templatelib_info_vec)));
}

int DBOperation::load_all_template_lib(mysqlpp::Query & query, vector<TemplateLibInfo> & templatelib_info_vec)
{
	templatelib_info_vec.clear();

	try
	{
		query << "select * from templatelibinfo where isdeleted != 1 ";
		vector<templatelibinfo> lib_res;
		query.storein(lib_res);

		for(size_t i=0; i<lib_res.size(); ++i)
		{
			templatelibinfo & libinfo = lib_res[i];

			TemplateLibInfo info;

			info.templatelib_id = libinfo.tid;
			info.usr_id = libinfo.username;
			info.templatelib_name = libinfo.templatelibname.c_str();
			info.is_deleted = libinfo.isdeleted;
			info.is_active = libinfo.isactive;
			info.is_system = libinfo.issystem;
			info.description = libinfo.description.c_str();
			info.domain_info.first = libinfo.type;
			info.domain_info.second.first = libinfo.srclanguage;
			info.domain_info.second.second = libinfo.tgtlanguage;

			


			templatelib_info_vec.push_back(info);
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::LoadTemplate(const TemplateLibInfo & templatelib_info, vector<TemplateInfo> & template_info_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::load_template, _1,
													boost::ref(templatelib_info),
													boost::ref(template_info_vec)));
}

int DBOperation::load_template(mysqlpp::Query & query, const TemplateLibInfo & templatelib_info, vector<TemplateInfo> & template_info_vec)
{
	template_info_vec.clear();

	try
	{
		query << "select * from templateiteminfo where isdeleted != 1 and templatelib_id = " << templatelib_info.templatelib_id;
		vector<templateiteminfo> item_res;
		query.storein(item_res);

		for(size_t i=0; i<item_res.size(); ++i)
		{
			templateiteminfo & item = item_res[i];

			TemplateInfo info;

			info.template_id = item.tid;
			info.src = item.src.c_str();
			info.tgt = item.tgt.c_str();
			info.is_deleted = item.isdeleted;
			info.is_active = item.isactive;

			info.is_checked = item.ischecked;


			template_info_vec.push_back(info);
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::LoadSegWord(vector<seg_word_share_ptr> & sp_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::load_seg_word, _1,
													boost::ref(sp_vec)));
}

int DBOperation::load_seg_word(mysqlpp::Query & query, vector<seg_word_share_ptr> & sp_vec)
{
	try
	{
		query << "select * from chsegment where isdeleted != 1";
		vector<chsegment> item_res;
		query.storein(item_res);

		for(size_t i=0; i<item_res.size(); ++i)
		{
			chsegment & item = item_res[i];

			seg_word_share_ptr sp_word(new ChSegWord(item.segword.c_str()));
			sp_word->word_id = item.tid;
			sp_word->is_active = item.isactive;


			sp_vec.push_back(sp_word);
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::InsertSegWord(seg_word_share_ptr sp_word)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::insert_seg_word, _1,
													boost::ref(sp_word)));
}

int DBOperation::insert_seg_word(mysqlpp::Query & query, seg_word_share_ptr sp_word)
{
	assert(sp_word);

	try
	{
		//执行插入
		query << "insert into chsegment(segword, isdeleted, isactive, createtime) values (\"" ;
		query << sp_word->word << "\", "; 
		query << "0, " ;
		query << sp_word->is_active << ", " ;
		query << "now() )";

		//lout << query.str() << endl;
		
        query.execute();

		//获得insertid
		sp_word->word_id = query.insert_id();

		//lout << "Get word insert id = " << word_info.word_id << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "ERROR: Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_EXCEPTION;
	}

	
	return SUCCESS;
}

int DBOperation::DeleteSegWord(const int word_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::delete_seg_word, _1,
													boost::ref(word_id)));
}

int DBOperation::delete_seg_word(mysqlpp::Query & query, const int word_id)
{
	try
	{
		query << "update chsegment set isdeleted=1 where tid = " << word_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	//lout << "DB delete template lib finish." << endl;
	
	return SUCCESS;
}

int DBOperation::UpdateSegWord(seg_word_share_ptr sp_word)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::update_seg_word, _1,
													boost::ref(sp_word)));
}

int DBOperation::update_seg_word(mysqlpp::Query & query, seg_word_share_ptr sp_word)
{
	try
	{
		query << "update chsegment set segword=\"" << sp_word->word << "\", isactive=" << sp_word->is_active << " where tid = " << sp_word->word_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	//lout << "DB delete template lib finish." << endl;
	
	return SUCCESS;
}

int DBOperation::InsertSegWord(const vector<seg_word_share_ptr> & sp_word_vec, vector<bool> & filter_res_vec)
{
	assert(sp_word_vec.size() == filter_res_vec.size());

	for(size_t i=0; i<filter_res_vec.size(); ++i)
	{
		if(true == filter_res_vec[i])
		{
			if(SUCCESS != DBOperation::InsertSegWord(sp_word_vec[i]))
				filter_res_vec[i] = false;
		}
	}

	return SUCCESS;
}


int DBOperation::DeleteSegWord(const vector<int> & word_id_vec, vector<bool> & filter_res_vec)
{
	assert(word_id_vec.size() == filter_res_vec.size());

	for(size_t i=0; i<filter_res_vec.size(); ++i)
	{
		if(true == filter_res_vec[i])
		{
			if(SUCCESS != DBOperation::DeleteSegWord(word_id_vec[i]))
				filter_res_vec[i] = false;
		}
	}

	return SUCCESS;
}

int DBOperation::RecoverySegWord(const vector<int> & word_id_vec, vector<seg_word_share_ptr> & sp_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::recovery_seg_word, _1,
													boost::ref(word_id_vec),
													boost::ref(sp_vec)));
}

int DBOperation::recovery_seg_word(mysqlpp::Query & query, const vector<int> & word_id_vec, vector<seg_word_share_ptr> & sp_vec)
{
	for(size_t i=0; i < word_id_vec.size(); ++i)
	{
		try
		{
			query << "update chsegment set isdeleted=0 where tid=" << word_id_vec[i] << endl;

			query.execute();
			query.reset();

			query << "select * from chsegment where tid=" << word_id_vec[i] << endl;
			vector<chsegment> item_res;
			query.storein(item_res);

			for(size_t i=0; i<item_res.size(); ++i)
			{
				chsegment & item = item_res[i];

				seg_word_share_ptr sp_word(new ChSegWord(item.segword.c_str()));
				sp_word->word_id = item.tid;
				sp_word->is_active = item.isactive;


				sp_vec.push_back(sp_word);
			}

		}catch(mysqlpp::Exception e)
		{
			lerr<< "Failed with mysql exception : " << e.what() << endl;
			return ERR_DB_FAILED;
		}

	}
	
	return SUCCESS;
}

int DBOperation::GetRecoverTemplateLibTemplateInfo(const TemplateLibID & templatelib_id, vector<TemplateInfo> & template_vec)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_recover_template_lib_template_info, _1,
													boost::ref(templatelib_id),
													boost::ref(template_vec)));
}

int DBOperation::get_recover_template_lib_template_info(mysqlpp::Query & query, const TemplateLibID & templatelib_id, vector<TemplateInfo> & template_vec)
{
	template_vec.clear();

	try
	{
		query << "select * from templateiteminfo where templatelib_id = " << templatelib_id;
		vector<templateiteminfo> item_res;
		query.storein(item_res);

		if(item_res.size() != 0)
		{
			for(size_t i=0; i<item_res.size(); ++i)
			{
				templateiteminfo & item = item_res[i];

				TemplateInfo info;

				info.template_id = item.tid;
				info.src = item.src.c_str();
				info.tgt = item.tgt.c_str();
				info.is_deleted = item.isdeleted;
				info.is_active = item.isactive;

				info.is_checked = item.ischecked;


				template_vec.push_back(info);
			}
		}
		
		return ERR_NOTEMPLATE_GET;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::GetRecoverTemplateLibInfo(const TemplateLibID & templatelib_id, TemplateLibInfo & templatelib_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_recover_template_lib_info, _1,
													boost::ref(templatelib_id),
													boost::ref(templatelib_info)));
}

int DBOperation::get_recover_template_lib_info(mysqlpp::Query & query, const TemplateLibID & templatelib_id, TemplateLibInfo & templatelib_info)
{
	try
	{
		query << "select * from templatelibinfo where tid = " << templatelib_id;
		vector<templatelibinfo> lib_res;
		query.storein(lib_res);

		if(lib_res.size() != 0)
		{
			templatelibinfo & libinfo = lib_res[0];

			templatelib_info.templatelib_id = libinfo.tid;
			templatelib_info.usr_id = libinfo.username;
			templatelib_info.templatelib_name = libinfo.templatelibname.c_str();
			templatelib_info.is_deleted = libinfo.isdeleted;
			templatelib_info.is_active = libinfo.isactive;
			templatelib_info.is_system = libinfo.issystem;
			templatelib_info.description = libinfo.description.c_str();
			templatelib_info.domain_info.first = libinfo.type;
			templatelib_info.domain_info.second.first = libinfo.srclanguage;
			templatelib_info.domain_info.second.second = libinfo.tgtlanguage;
		}

		return ERR_NOTEMPLATELIB_GET;
	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetRecoverTemplateInfo(const TemplateID & template_id, TemplateInfo & template_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_recover_template_info, _1,
													boost::ref(template_id),
													boost::ref(template_info)));
}

int DBOperation::get_recover_template_info(mysqlpp::Query & query, const TemplateID & template_id, TemplateInfo & template_info)
{
	try
	{
		query << "select * from templateiteminfo where tid = " << template_id;
		vector<templateiteminfo> item_res;
		query.storein(item_res);

		if(item_res.size() != 0)
		{
			templateiteminfo & item = item_res[0];

			template_info.template_id = item.tid;
			template_info.src = item.src.c_str();
			template_info.tgt = item.tgt.c_str();
			template_info.is_deleted = item.isdeleted;
			template_info.is_active = item.isactive;

			template_info.is_checked = item.ischecked;
		}
		
		return ERR_NOTEMPLATE_GET;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::RecoverTemplateLib(const TemplateLibID & templatelib_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::recover_template_lib, _1,
													boost::ref(templatelib_id)));
}

int DBOperation::recover_template_lib(mysqlpp::Query & query, const TemplateLibID & templatelib_id)
{
	try
	{
		query << "update templatelibinfo set isdeleted=0 where tid = " << templatelib_id;
		//lout << query.str() << endl;
		query.execute();

		query.reset();
		query << "update templateiteminfo set isdeleted=0 where templatelib_id = " << templatelib_id;
		//lout << query.str() << endl;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	//lout << "DB delete template lib finish." << endl;
	
	return SUCCESS;
}

int DBOperation::RecoverTemplate(const TemplateID & template_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::recover_template, _1,
													boost::ref(template_id)));
}

int DBOperation::recover_template(mysqlpp::Query & query, const TemplateID & template_id)
{
	try
	{
		query << "update templateiteminfo set isdeleted=0 where tid = " << template_id;
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}


int DBOperation::GetImportTemplateInfo(const vector<int> & import_template_id, vector<TemplateInfo> & import_template_info)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_import_template_info, _1,
													boost::ref(import_template_id),
													boost::ref(import_template_info)));
}

int DBOperation::get_import_template_info(mysqlpp::Query & query, const vector<int> & import_template_id, vector<TemplateInfo> & import_template_info)
{
	import_template_info.clear();

	try
	{
		for(size_t i=0; i<import_template_id.size(); i++)
		{
			query << "select * from templateiteminfo where tid = " << import_template_id[i];
			vector<templateiteminfo> item;
			query.storein(item);

			TemplateInfo info;

			info.template_id = item[0].tid;
			info.src = item[0].src.c_str();
			info.tgt = item[0].tgt.c_str();
			info.is_deleted = item[0].isdeleted;
			info.is_active = item[0].isactive;

			info.is_checked = item[0].ischecked;
			import_template_info.push_back(info);

			query.reset();
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	
	return SUCCESS;
}

int DBOperation::ModifyStatus_T(const vector<int> & import_template_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::modify_status_t, _1,
													boost::ref(import_template_id)));
}

int DBOperation::modify_status_t(mysqlpp::Query & query, const vector<int> & import_template_id)
{
	try
	{
		for(size_t i=0; i<import_template_id.size(); i++)
		{
			query << "update templateimport set status=2, isdeleted=1 where fromid = " << import_template_id[i];
			//lout << query.str() << endl;
			query.execute();
		}

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}
	
	return SUCCESS;
}

int DBOperation::GetMaxTemplateID(TemplateLibID & templatelib_id, TemplateID & template_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_max_template_id, _1,
													boost::ref(templatelib_id),
													boost::ref(template_id)));
}

int DBOperation::get_max_template_id(mysqlpp::Query & query, TemplateLibID & templatelib_id, TemplateID & template_id)
{
	try
	{
		query << "select tid from templatelibinfo order by tid desc limit 0,1";
		vector<templatelibinfo> res;
		query.storein(res);

		if(res.size() == 0 )
		{
			templatelib_id = 0;
		}else
		{
			templatelib_id = res[0].tid;
		}

		lout << "templatelib_id = " << templatelib_id << endl;

		query.reset();

		query << "select tid from templateiteminfo order by tid desc limit 0,1";
		vector<templateiteminfo> item_res;
		query.storein(item_res);

		if(item_res.size() == 0 )
		{
			template_id = 0;
		}else
		{
			template_id = item_res[0].tid;
		}

		lout << "template_id = " << template_id << endl;

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}


int DBOperation::SubmitAbstract(const TextID & tid, const string & abstracts)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::submit_abstract, _1,
													boost::ref(tid),
													boost::ref(abstracts)));
}

int DBOperation::submit_abstract(mysqlpp::Query & query, const TextID & tid, const string & abstracts)
{
	try
	{
		query << "update filetrans set abstract=\"" << mysqlpp::escape << abstracts << "\" where guid = \"" << tid << "\"";
		query.execute();

	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::GetMaxSentID(long & max_sent_id)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_max_sent_id, _1,
													boost::ref(max_sent_id)));
}

int DBOperation::get_max_sent_id(mysqlpp::Query & query, long & max_sent_id)
{
	try
	{
		query << "select SentID from transsentence order by SentID desc limit 0,1";
		vector<transsentence> res;
		query.storein(res);

		if(res.size() == 0 )
		{
			max_sent_id = 0;
		}else
		{
			max_sent_id = res[0].SentID;
		}

		lout << "max_sent_id = " << max_sent_id << endl;


	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}


int DBOperation::GetSplitStatus(const string & src_language, int & split_status)
{
	return DBConnPool::GetInstance()->Runable(boost::bind(&DBOperation::get_split_status, _1,
				boost::ref(src_language),
				boost::ref(split_status)));
}

int DBOperation::get_split_status(mysqlpp::Query & query, const string & src_language, int & split_status)
{
	try
	{
		query << "select isspace from transdir where srclanguage = \"" << src_language << "\"";
		vector<transdir> res;
		query.storein(res);

		if(res.size() == 0 )
		{
			split_status = -1;
		}else
		{
			split_status = res[0].isspace;
		}
		lout << "split_status = " << split_status << endl;
	}catch(mysqlpp::Exception e)
	{
		lerr<< "Failed with mysql exception : " << e.what() << endl;
		return ERR_DB_FAILED;
	}

	return SUCCESS;
}

int DBOperation::ModefyExportStatus(const DictID dict_id, const vector<WordInfo> & word_info_vec)
{
	lout << "ModefyExportStatus = " << "test function" << endl;
	return SUCCESS;
}