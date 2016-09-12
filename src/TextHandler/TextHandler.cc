#include "TextHandler.h"
#include "LanguageTools/CLanguage.h"
#include "Common/ICUConverter.h"
#include "TinyXml/tinyxml.h"

#define BASE_TEXT_SOURCE_POSTFIX ".base.source"   //gb18030
#define BASE_TEXT_POS_POSTFIX    ".base.pos"
#define BASE_TEXT_TARGET_POSTFIX ".base.target"   //utf-8

#define DOC_SOURCE_POSTFIX  ".doc.source"
#define DOC_POS_POSTFIX     ".doc.pos"
#define HTML_SOURCE_POSTFIX ".html.source"
#define HTML_EXT_TXT_POSTFIX ".html.ext"
#define HTML_INFO_POSTFIX   ".html.info"
#define PDF_SOURCE_POSTFIX  ".pdf.source"
#define PDF_EXT_TXT_POSTFIX ".pdf.ext"

TextHandler TextHandler::ms_instance;

int TextHandler::build_base_source(const string & text, TransText & trans_text)
{
	string source_text = text;

	//编码识别和转换
	charset_handle(source_text);

	//文本格式解析
	trans_text.ResetSrcData(source_text);

	TxtParser txt;
	vector<string> para_vec;
	vector<pair<size_t,size_t> > pos_vec;

	if(false == txt.Parse(trans_text.GetSrcData(), para_vec, pos_vec) )
	{
		lerr << "Txt Format parse Failed." << endl;
		return ERR_PROC_TXT_FROMAT;
	}

	trans_text.SetFormatSrc(para_vec, pos_vec);

	//保存Base pos文件
	string pos_content;
	trans_text.GetFromat().Serialization(pos_content);

	string pos_file_path = m_default_file_path + trans_text.GetID() + BASE_TEXT_POS_POSTFIX;
	if(false == write_file(pos_file_path, pos_content.c_str(), pos_content.size()))
	{
		return ERR_SAVE_BASE_POS_FILE_FAIELD;
	}

	//保存Base source 文件
	string source_file_path = m_default_file_path + trans_text.GetID() + BASE_TEXT_SOURCE_POSTFIX;
	if(false == write_file(source_file_path, source_text.c_str(), source_text.size()) )
	{
		return ERR_SAVE_BASE_SOURCE_FILE_FAILED;
	}

    return SUCCESS;
}


void TextHandler::charset_handle(string & text)
{
	//string src_charset;
	//int score = ICUConverter::Detect(text.c_str(), text.size(), src_charset);
	//
	//lout << "Src detect sroce is = " << score << " src_charset = " << src_charset << endl;

	//del_utf8_bom(text);

	//if(score < BELIEVABLE_DETECT || "UTF-8" == src_charset)
	//{
	//	if(score < BELIEVABLE_DETECT)
	//		lout << "Src detect sroce is too low = " << score << " src_charset = " << src_charset << endl;

	//	//分数太低 或已经是UTF-8不作转换
	//	
	//}
	//else
	//{
	//	ICUConverter::Convert(src_charset, "UTF-8", text);
	//}
}

int TextHandler::convert_txt_to_text(const string & filepath, string & text)
{
	ByteBuffer data;

	if(false == read_file(filepath, data) )
	{
		return ERR_READ_TXT_INPUT_FILE_FAILED;
	}

	data.String(text);

	return SUCCESS;
}

int TextHandler::convert_word_to_text(const string & filepath, const TextID & tid, const string & file_type, string & text)
{
	string cmd = "java -jar ./mdata/word/WordDocumentExtractor.jar parse -t \"" + file_type + "\" -i \"" + filepath + "\" -m \"" + m_word_workspace + "/" + tid + ".word.tmp\" -x \"" + m_word_workspace + "/"  + tid + ".word.xml\"";
	system(cmd.c_str());

	//读取原文件
	string word_ext_result_file = m_word_workspace + "/" + tid + ".word.xml";
	ByteBuffer data;

	if(false == read_file(word_ext_result_file, data) )
	{
		return ERR_READ_WORD_EXT_FILE_FAILED;
	}

	//解析抽取出的XML
	stringstream ss;

	TiXmlDocument xmldoc;
	xmldoc.Parse(data.String().c_str());
	TiXmlHandle docHandle( &xmldoc );

	try
	{
		size_t idx = 0;
		TiXmlElement * elem = docHandle.FirstChild("document").Child("p", idx).ToElement();

		while(elem)
		{
			
			const char * tmp = elem->GetText();
			if(tmp)
				ss << tmp << endl;
			else
				ss << endl;

			++idx;
			elem = docHandle.FirstChild("document").Child("p", idx).ToElement();

		}
		
	}catch (...)
	{
		return ERR_PARSE_WORD_XML;
	}

	//清理XML资源
	xmldoc.Clear();

	text = ss.str();

	return SUCCESS;
}

int TextHandler::convert_pdf_to_text(const string & filepath, const TextID & tid, string & text)
{
	//读取原文件
	ByteBuffer data;

	if(false == read_file(filepath, data) )
	{
		return ERR_READ_PDF_INPUT_FILE_FAILED;
	}

	//生成source文件
	string source_file_path = m_default_file_path + tid + PDF_SOURCE_POSTFIX;

	if(false == write_file(source_file_path, data.GetPtr(), data.GetLength()) )
	{
		return ERR_WRITE_PDF_SOURCE_FILE;
	}
	
	string pdf_ext_filepath = m_default_file_path + tid + PDF_EXT_TXT_POSTFIX;

	//call xpdf
	string cmd = "./mdata/pdf/pdftotext -cfg ./mdata/pdf/xpdf-chinese-simplified/xpdfrc -enc GBK " + source_file_path + " " + pdf_ext_filepath;
	system(cmd.c_str());

	//读取抽取的文件
	ByteBuffer tmp_data;
	if(false == read_file(pdf_ext_filepath, tmp_data))
	{
		return ERR_READ_PDF_EXT_FILE_FAILED;
	}

	tmp_data.String(text);

	return SUCCESS;
}

int TextHandler::convert_html_to_text(const string & filepath, const TextID & tid, string & text)
{
	//读取原文件
	ByteBuffer data;

	if(false == read_file(filepath, data) )
	{
		return ERR_READ_HTML_INPUT_FILE_FAILED;
	}

	//生成source文件
	string source_file_path = m_default_file_path + tid + HTML_SOURCE_POSTFIX;

	if(false == write_file(source_file_path, data.GetPtr(), data.GetLength()) )
	{
		return ERR_WRITE_HTML_SOURCE_FILE;
	}
	
	string html_ext_filepath = m_default_file_path + tid + HTML_EXT_TXT_POSTFIX;
	string html_info_filepath = m_default_file_path + tid + HTML_INFO_POSTFIX;

	//call html parser
	string cmd = "java -jar ./mdata/html/htmlAnalysis.jar -g \"" + source_file_path + "\" \"" + html_ext_filepath + "\" \"" + html_info_filepath + "\"";
	system(cmd.c_str());

	//读取抽取的文件
	ByteBuffer tmp_data;
	if(false == read_file(html_ext_filepath, tmp_data))
	{
		return ERR_READ_HTML_EXT_FILE_FAILED;
	}

	tmp_data.String(text);

	return SUCCESS;
}



bool TextHandler::Start(const size_t thread_num, const string & default_file_path)
{
	
	m_default_file_path = default_file_path + "\\";
	m_word_workspace = m_default_file_path + "wordworkspace\\"; 

	if(!PathIsDirectory(m_word_workspace.c_str()))
	{
		lout << "Path Error ..., mkdir path = " << m_word_workspace << endl;
		CreateDirectory(m_word_workspace.c_str(), NULL);
	}

	if( !EventEngine::Start(thread_num) )
		return false;

	lout << " TextHandler this_ptr = " << (unsigned long) this << endl;

	return true;
}


bool TextHandler::write_file(const string & file_path, const char * p_data, const size_t len)
{
	if(NULL == p_data && 0 != len)
	{
		lerr << "Input data is null, and len is not zero." << endl;
		return false;
	}

	ofstream file(file_path.c_str(), ios::binary);

	if(false == file.is_open())
	{
		return false;
	}

	file.write(p_data, len);

	file.close();

	return true;
}


bool TextHandler::read_file(const string & file_path, ByteBuffer & data)
{
	int length = 0;
	ifstream file(file_path.c_str(), ios::binary);

	if(false == file.good())
	{
		cerr << "Read txt result file failed, path = " << file_path << endl;
		return false;
	}

	file.seekg (0, ios::end);
	length = file.tellg();
	file.seekg (0, ios::beg);

	data.Resize(length);
	file.read(data.GetPtr(), length);

	file.close();

	return true;
}

int TextHandler::build_detail_result_file(const TextID & tid, string & result_file_name)
{
	int ret = SUCCESS;

	//从数据库读取翻译结果
	vector<boost::tuple<string, string, string, string, string> > result_vec;
	string domain;
	string src_language;
	string tgt_language;

	ret = DBOperation::GetTransResultDetail(tid, result_vec, domain, src_language, tgt_language);

	if(SUCCESS != ret)
	{
		return ERR_GET_TGT_VEC_FROM_DB;
	}

	//生成TMX 格式数据
	string tmx_data;

//
//[Language]
//[Total Number]
//
//[Sent 0 ]
//	[   Pre     ]
//  [   Rules   ]
//  [  Decode  ]
//  [ UsrModify ]
//  [   Final   ]
//[-------]
//
//[Sent 1 ]
//	[   Pre     ]
//  [   Rules   ]
//  [  Decode  ]
//  [ UsrModify ]
//  [   Final   ]
//[-------]

//[END]
	
	stringstream data;
	data << "[Domain] : " << domain << "\r\n";
	data << "[Language] " << src_language << " --> " << tgt_language << "\r\n";
	data << "[Total Number] " << result_vec.size() << "\r\n";
	data << "\r\n";
	data << "\r\n";

	for(size_t i=0; i<result_vec.size(); ++i)
	{
		data << "[Sent " << i << " ]" << "\r\n";
		data << "   [   Pre     ] " << result_vec[i].get<0>() << "\r\n";
		data << "   [   Rules   ] " << result_vec[i].get<1>() << "\r\n";
		data << "   [  Decode   ] " << result_vec[i].get<2>() << "\r\n";
		data << "   [ UsrModify ] " << result_vec[i].get<3>() << "\r\n";
		data << "   [   Final   ] " << result_vec[i].get<4>() << "\r\n";
		data << "[-------]" << "\r\n";
		data << "\r\n";

	}

	data << "[END]" << "\r\n" ;

	//写入到文件
	string build_id = GenerateUUID();
	result_file_name = tid + "." + build_id + ".txt";
	string tmx_filepath = m_default_file_path + result_file_name;

	if(false == write_file(tmx_filepath, data.str().c_str(), data.str().size()))
	{
		return ERR_OPEN_RESULT_FILE_FAILED;
	}

	return SUCCESS;

}

int TextHandler::build_tmx_result_file(const TextID & tid, string & result_file_name)
{
	int ret = SUCCESS;

	//从数据库读取翻译结果
	vector<boost::tuple<string, string, string> > result_vec;
	string tgt_language;

	ret = DBOperation::GetTransResultPair(tid, result_vec, tgt_language);

	if(SUCCESS != ret)
	{
		return ERR_GET_TGT_VEC_FROM_DB;
	}

	//生成TMX 格式数据
	string tmx_data;

//<?xml version="1.0" encoding=UTF-8" ?>
//
//<tmx version="1.4">
//	<header creationdate=""			    <!--创建日期-->
//			creationtool="" 			<!--创建工具-->
//			creationtoolversion=""      <!--创建工具的版本-->
//			datatype="unknown"          <!--数据类型：有很多种，一般使用默认-->
//			o-tmf="unknown"				<!--源存储格式-->
//			segtype="block"				<!--段值：block,paragraph,sentence,phrase-->
//			srclang="*all*"				<!--源语言:常使用*all*-->
//			adminlang="en"/>            <!--Administrative language-->
//					
//	<body>
//		<tu changedate="">              <!--修改日期-->
//			<tuv xml:lang="en"><seg>introduction</seg></tuv>
//			<tuv xml:lang="zh-cn"><seg>简介</seg></tuv>
//		</tu>
//		<tu changedate="">
//			<tuv xml:lang="en"><seg>conclusion</seg></tuv>
//			<tuv xml:lang="zh-cn"><seg>总结</seg></tuv>
//		</tu>
//	</body>
//</tmx>
	
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 
	
	try
    {
		//声明
		TiXmlDeclaration* dec = new TiXmlDeclaration("1.0", "UTF-8", "");
		xmlDocs->LinkEndChild(dec);

		//tmx 节点
		TiXmlElement * tmxElem = new TiXmlElement("tmx");
		xmlDocs->LinkEndChild(tmxElem);   

		//header节点
		TiXmlElement * headerElem = new TiXmlElement("header"); 
		headerElem->SetAttribute("creationdate", "");
		headerElem->SetAttribute("creationtool", "ict_trans");
		headerElem->SetAttribute("creationtoolversion", "1.0");
		headerElem->SetAttribute("datatype", "unknown");
		headerElem->SetAttribute("o-tmf", "unknown");
		headerElem->SetAttribute("segtype", "sentence");
		headerElem->SetAttribute("srclang", "en");
		headerElem->SetAttribute("srclang", "zh-cn");
		tmxElem->LinkEndChild(headerElem);

		//body节点
		TiXmlElement * bodyElem = new TiXmlElement("body");
		tmxElem->LinkEndChild(bodyElem);  

		//loop tu 节点
		for(size_t i=0; i<result_vec.size(); ++i)
		{
			//tu节点
			TiXmlElement * tuElem = new TiXmlElement("tu");
			bodyElem->LinkEndChild(tuElem);

			//tuv 节点对
			string * p_english = NULL;
			string * p_chinese = NULL;

			if(LANGUAGE_CHINESE == tgt_language)
			{
				p_english = &result_vec[i].get<0>();

				if(result_vec[i].get<2>().size() > 0)
				{
					CLanguage::En2ChPost(result_vec[i].get<2>());
					p_chinese = &result_vec[i].get<2>();
				}else
				{
					p_chinese = &result_vec[i].get<1>();
				}
				
			}else
			{
				p_chinese = &result_vec[i].get<0>();

				if(result_vec[i].get<2>().size() > 0)
				{
					CLanguage::Ch2EnPost(result_vec[i].get<2>());
					p_english = &result_vec[i].get<2>();
				}else
				{
					p_english = &result_vec[i].get<1>();
				}
			}

			//tuv-en
			TiXmlElement * tuvenElem = new TiXmlElement("tuv");
			TiXmlText * tuvenText = new TiXmlText(p_english->c_str()); 
			tuvenElem->SetAttribute("xml:lang", "en");
			tuvenElem->LinkEndChild(tuvenText);
			tuElem->LinkEndChild(tuvenElem);

			//tuv-ch
			TiXmlElement * tuvchElem = new TiXmlElement("tuv");
			TiXmlText * tuvchText = new TiXmlText(p_chinese->c_str()); 
			tuvenElem->SetAttribute("xml:lang", "zh-ch");
			tuvchElem->LinkEndChild(tuvchText);
			tuElem->LinkEndChild(tuvchElem);

		}

		
		//输出到xmldata
		TiXmlPrinter print;
		xmlDocs->Accept(&print);
		tmx_data = print.CStr();

    }catch (...)
    {
		lerr << "Create xml-tmx failed." << endl;
		delete xmlDocs;

		return ERR_USR_XML_PARSER;
    }

    delete xmlDocs;

	//写入到文件
	string build_id = GenerateUUID();
	result_file_name = tid + "." + build_id + ".tmx";
	string tmx_filepath = m_default_file_path + result_file_name;

	if(false == write_file(tmx_filepath, tmx_data.c_str(), tmx_data.size()))
	{
		return ERR_OPEN_RESULT_FILE_FAILED;
	}

	return SUCCESS;

}

int TextHandler::build_text_result_file(const TextID & tid, const bool is_bilingual, string & result_file_name)
{
	int ret = SUCCESS;

	//从数据库读取翻译结果
	vector<boost::tuple<string, string, string, size_t> > result_vec;
	string file_type;
	string tgt_language;

	ret = DBOperation::GetTransResult(tid, result_vec, file_type, tgt_language);

	CLanguage::EnLowercase(file_type);

	if(SUCCESS != ret)
	{
		return ERR_GET_TGT_VEC_FROM_DB;
	}

	//后处理辅助翻译的修改结果
	bool is_english = (LANGUAGE_ENGLISH == tgt_language ) ? true : false; 
	for(size_t i=0; i<result_vec.size(); ++i)
	{
		string & modify_tgt = result_vec[i].get<1>();
		if(modify_tgt.size() > 0)
		{
			if(is_english)
				CLanguage::Ch2EnPost(modify_tgt);
			else
				CLanguage::En2ChPost(modify_tgt);
		}
	}

	//返回的result_vec顺序由DBOperation保证，根据SentID大小决定

	//生成结果文件
	string build_id = GenerateUUID();
	string base_result_file_path;
	string result_text;
	bool is_blank_between_sent = (LANGUAGE_ENGLISH == tgt_language ) ? true : false; 

	if( SUCCESS != (ret = build_base_result_file(tid, build_id, result_vec, is_blank_between_sent, is_bilingual, base_result_file_path, result_text)) )
		return ret;

	return build_txt_result_file(tid, build_id, result_text, result_file_name);
}


int TextHandler::build_result_file(const TextID & tid, const bool is_bilingual, string & result_file_name)
{
	int ret = SUCCESS;

	//从数据库读取翻译结果
	vector<boost::tuple<string, string, string, size_t> > result_vec;
	string file_type;
	string tgt_language;

	ret = DBOperation::GetTransResult(tid, result_vec, file_type, tgt_language);

	CLanguage::EnLowercase(file_type);

	if(SUCCESS != ret)
	{
		return ERR_GET_TGT_VEC_FROM_DB;
	}

	//后处理辅助翻译的修改结果
	bool is_english = (LANGUAGE_ENGLISH == tgt_language ) ? true : false; 
	for(size_t i=0; i<result_vec.size(); ++i)
	{
		string & modify_tgt = result_vec[i].get<1>();
		if(modify_tgt.size() > 0)
		{
			if(is_english)
				CLanguage::Ch2EnPost(modify_tgt);
			else
				CLanguage::En2ChPost(modify_tgt);
		}
	}

	//返回的result_vec顺序由DBOperation保证，根据SentID大小决定

	//生成结果文件
	string build_id = GenerateUUID();
	string base_result_file_path;
	string result_text;
	bool is_blank_between_sent = (LANGUAGE_ENGLISH == tgt_language ) ? true : false; 

	if( SUCCESS != (ret = build_base_result_file(tid, build_id, result_vec, is_blank_between_sent, is_bilingual, base_result_file_path, result_text)) )
		return ret;

	if(TEXT_TYPE_TXT == file_type)
	{
		ret = build_txt_result_file(tid, build_id, result_text, result_file_name);

	}else if( TEXT_TYPE_DOC == file_type || TEXT_TYPE_DOCX == file_type)
	{
		ret = build_word_result_file(tid, build_id, file_type, result_text, result_file_name);
	}else if( TEXT_TYPE_HTML == file_type )
	{
		ret = build_html_result_file(tid, build_id, base_result_file_path, result_file_name);
	}else if( TEXT_TYPE_PDF == file_type )
	{
		ret = build_pdf_result_file(tid, build_id, result_text, result_file_name);
	}else
	{
		ret = ERR_UNSUPORT_FILE;
	}

	return ret;

}

int TextHandler::build_pdf_result_file(const TextID & tid, 
							  const string & build_id,
							  const string & result_text,
							  string & result_file_name)
{
	//pdf 不进行还原
	return build_txt_result_file(tid, build_id, result_text, result_file_name);
}

int TextHandler::build_txt_result_file(const TextID & tid, 
							  const string & build_id,
							  const string & result_text,
							  string & result_file_name)
{
	result_file_name = tid + "." + build_id + ".result.txt";

	string result_path = this->m_default_file_path + result_file_name;
	if(false == write_file(result_path, result_text.c_str(), result_text.size()))
	{
		return ERR_OPEN_RESULT_FILE_FAILED;
	}

	return SUCCESS;
}


int TextHandler::build_word_result_file(const TextID & tid, 
									   const string & build_id,
									   const string & file_type,
									   const string & result_text,
									   string & result_file_name)
{

	string word_uuid = tid + "-" + build_id;

	//生成xml
	stringstream ss;
	ss << result_text;

	string xmldata;
	TiXmlDocument * xmlDocs = new TiXmlDocument(); 

	try
	{
		TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "");  
		xmlDocs->LinkEndChild(pDeclaration);  
		
		TiXmlElement * docElem = new TiXmlElement("document");   
		xmlDocs->LinkEndChild(docElem);

		string line;
		size_t i=1;
		while(getline(ss, line))
		{
			TiXmlElement * pElem = new TiXmlElement("p");
			docElem->LinkEndChild(pElem);
			pElem->SetAttribute("index", i);

			TiXmlText * contentText = new TiXmlText(line);
			pElem->LinkEndChild(contentText);

			++i;
		}

		docElem->SetAttribute("id", word_uuid);

		//输出到xmldata
		TiXmlPrinter print;
		xmlDocs->Accept(&print);
		xmldata = print.CStr();

	}catch(...)
	{
		return ERR_CREATE_WORD_RESULT_XML;
	}


	//输出到word解析工作目录下进行还原
	string cmd;
	
#ifdef WINDOWS
	cmd = "copy \"" + m_word_workspace + "/" + tid + ".word.tmp\"  \"" + m_word_workspace + "/" + word_uuid + ".word.tmp\"";
#else
	cmd = "cp \"" + m_word_workspace + "/" + tid + ".word.tmp\"  \"" + m_word_workspace + "/" + word_uuid + ".word.tmp\"";
#endif
	//lout << cmd << endl;
	system(cmd.c_str());

	string xml_data_path = m_word_workspace + "/" + word_uuid + ".word.xml";
	if(false == write_file(xml_data_path, xmldata.c_str(), xmldata.size()))
	{
		return ERR_SAVE_WORD_RECOVER_XML;
	}

	result_file_name = tid + "." + build_id + ".result." + file_type;
	string result_path = this->m_default_file_path + result_file_name;
	cmd = "java -jar ./mdata/word/WordDocumentExtractor.jar generate -t \"" + file_type + "\" -o \"" + result_path + "\" -m \"" + m_word_workspace + "/" + word_uuid + ".word.tmp\" -x \"" + m_word_workspace + "/" + word_uuid + ".word.xml\"";
	//lout << "Word generate cmd: " << cmd << endl;
	system(cmd.c_str());

	

	return SUCCESS;
}

int TextHandler::build_html_result_file(const TextID & tid, 
										const string & build_id,
										const string & base_result_file_path,
										string & result_file_name)
{
	string html_source_filepath = m_default_file_path + tid + HTML_SOURCE_POSTFIX;
	string html_info_filepath = m_default_file_path + tid + HTML_INFO_POSTFIX;
	result_file_name = tid + "." + build_id + ".result.html";
	string html_result_filepath = m_default_file_path + result_file_name;

	//call html parser
	string cmd = "java -jar ./mdata/html/htmlAnalysis.jar -r \"" + html_source_filepath + "\" \"" + base_result_file_path + "\" \"" + html_result_filepath + "\" utf-8";
	system(cmd.c_str());

	//todo 校验文件
	return SUCCESS;
}

int TextHandler::build_base_result_file(const TextID & tid, 
										const string & build_id,
										const vector<boost::tuple<string, string, string, size_t> > & result_vec,
										const bool is_blank_between_sent,
										const bool is_bilingual,
										string & base_result_file_path,
										string & result_text)
{
	//读取base source文件
	ByteBuffer source;
	string source_file_path = m_default_file_path + tid + BASE_TEXT_SOURCE_POSTFIX;

	if(false == read_file(source_file_path, source) )
	{
		return ERR_READ_BASE_SOURCE_FILE;
	}

	//读取base pos文件
	ByteBuffer pos_data;
	string pos_file_path = m_default_file_path + tid + BASE_TEXT_POS_POSTFIX;

	if(false == read_file(pos_file_path, pos_data) )
	{
		return ERR_READ_BASE_POS_FILE;
	}

	//生成结果文件
	TextFormat format;
	if(false == format.UnSerialization(pos_data.String()) )
	{
		return ERR_PARSE_RESULT_FORMAT;
	}

	//为每个段落生成tgt
	string curr_para_tgt;
	vector<string> para_tgt_vec;
	para_tgt_vec.resize(format.Size(), "");

	size_t i = 0;

	for(i=0; i<result_vec.size(); ++i)
	{

		string final_tgt;
		
		if(result_vec[i].get<2>().size() > 0)
			final_tgt += result_vec[i].get<2>();
		else
			final_tgt += result_vec[i].get<1>();  

        if(result_vec[i].get<3>() >= format.Size())
        {
            lerr << "formate restor failed. para_size = " << format.Size() << " result_vec[" << i << "].para_idx = " << result_vec[i].get<2>() << endl;
            continue;
        }

		if(i+1 < result_vec.size() && is_blank_between_sent)
			para_tgt_vec[result_vec[i].get<3>()] += final_tgt + " ";
		else
			para_tgt_vec[result_vec[i].get<3>()] += final_tgt;

	}
	
	//重新生成原文
	result_text.clear();
	size_t para_idx = 0;
	size_t offset = 0;

	while(offset < source.GetLength())
	{
        //lout << "loop beg:" << endl;
        //lout << "offset = " << offset << " , para_idx = " << para_idx << endl;
		if(para_idx < format.Size())
		{
            //lout << "format[para_idx]._offset = " << format[para_idx]._offset  << " offset = " << offset << endl;
			size_t len = format[para_idx]._offset - offset;

			//输出非翻译段落
			if(len > 0)
			{
				result_text += source.String().substr(offset, len);
			}

			//输出翻译段落
			if(is_bilingual)
				result_text += source.String().substr(format[para_idx]._offset, format[para_idx]._len) + " [";

			result_text += para_tgt_vec[para_idx];

			if(is_bilingual)
				result_text += "] ";

			//更新偏移量
			offset = format[para_idx]._offset + format[para_idx]._len;
			++para_idx;

           // lout << "result_text = [" << result_text << "]" << endl;

		}else
		{
			size_t len = source.GetLength() - offset;

			//输出非翻译段落
			if(len > 0)
			{
				result_text += source.String().substr(offset, len);
			}

			break;
		}
	}

	//输出结果到文件
	base_result_file_path =  m_default_file_path + tid + "." + build_id + BASE_TEXT_TARGET_POSTFIX;
	
	if(false == write_file(base_result_file_path, result_text.c_str(), result_text.size()) )
	{
		return ERR_WRITE_BASE_RESULT_FILE;
	}

	return SUCCESS;

}


void TextHandler::LoadTransText(TransText * p_trans_text, const CallID cid, EventEngine * p_caller)
{
	if(!p_trans_text)
	{
		lerr << "Input text is null." << endl;
		return;
	}

	if(!p_caller)
	{
		lerr << "Input caller is null." << endl;
		return;
	}

	TextLoadReq * p_req = new TextLoadReq(cid, p_trans_text);

	Event e(p_req, p_caller);
	this->PostEvent(e);
}

void TextHandler::CreateResult(const TextID & tid, const CallID cid, const string & download_type, const bool is_bilingual, EventEngine * p_caller)
{

	if(!p_caller)
	{
		lerr << "Input caller is null." << endl;
		return;
	}

	TextCreateReq * p_req = new TextCreateReq(cid);
	p_req->SetTextID(tid);
	p_req->SetCreateType(download_type);
	p_req->SetBilingual(is_bilingual);

	Event e(p_req, p_caller);
	this->PostEvent(e);
}

void TextHandler::SubmitResult(TransText * p_text, const CallID cid, EventEngine * p_caller)
{
	if(!p_text)
	{
		lerr << "Input text is null." << endl;
		return;
	}

	if(!p_caller)
	{
		lerr << "Input caller is null." << endl;
		return;
	}

	TextSubmitReq * p_req = new TextSubmitReq(cid, p_text);

	Event e(p_req, p_caller);
	this->PostEvent(e);
}


void TextHandler::on_event(Event & e)
{

	if(!e._p_edata)
	{
		lerr << "e._data is null." << endl;
		return;
	}

	if(!e._p_caller)
	{
		lerr << "e.caller is null." << endl;
		delete e._p_edata;
		return;
	}


	const EDType e_type = e._p_edata->GetType();
	EventData * p_edata_re = NULL;

	switch(e_type)
	{
	case EDTYPE_LOAD_TEXT_REQ:
		p_edata_re = on_load_text(e._p_edata);
		break;
	case EDTYPE_CREATE_RESULT_REQ:
		p_edata_re = on_create_result(e._p_edata);
		break;
	case EDTYPE_SUBMIT_RESULT_REQ:
		p_edata_re = on_submit_result(e._p_edata);
		break;
	default:
		lerr << "TextHandler can't handle this event , type = " << e_type << endl;
	}
	

	if(p_edata_re)
	{
		//返回结果
		Event re(p_edata_re, this);
		e._p_caller->PostEvent(re);
	}else
	{
		lerr << "TextHandler handle faield. set p_edata_re is NULL." << endl;
	}
	
	//销毁Req
	delete e._p_edata;


}



EventData * TextHandler::on_submit_result(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//转换为TextCreateReq
	TextSubmitReq * p_req = dynamic_cast<TextSubmitReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TextSubmitReq failed." << endl;
		return NULL;
	}

	//生成res
	TextSubmitRes * p_res = new TextSubmitRes(p_req->GetCallID());

	//开始处理
	int ret = SUCCESS;

	try
	{

		//Step 1 提交结果
		string result_file_name;
		if(SUCCESS != (ret = DBOperation::SubmitTransResult(p_req->GetTransText())))
			throw -1;

	}catch(...)
	{
		lerr << "Submit result text failed. err = " << ret << endl;
	}

	//处理完毕
	p_res->SetResult(ret);

	return p_res;
}


EventData * TextHandler::on_create_result(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//转换为TextCreateReq
	TextCreateReq * p_req = dynamic_cast<TextCreateReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TextCreateReq failed." << endl;
		return NULL;
	}

	//生成res
	TextCreateRes * p_res = new TextCreateRes(p_req->GetCallID());

	//开始处理
	int ret = SUCCESS;

	try
	{
		//Step 1 生成结果文件
		string result_file_name;

		if(DOWNLOAD_TYPE_TMX == p_req->CreateType())
		{
			if(SUCCESS != (ret = build_tmx_result_file(p_req->GetTextID(), result_file_name)))
				throw -1;
		}else if( DOWNLOAD_TYPE_NORMAL == p_req->CreateType())
		{
			if(SUCCESS != (ret = build_result_file(p_req->GetTextID(), p_req->IsBilingual(), result_file_name)))
				throw -2;
		}else if( DOWNLOAD_TYPE_TEXT == p_req->CreateType())
		{
			if(SUCCESS != (ret = build_text_result_file(p_req->GetTextID(), p_req->IsBilingual(), result_file_name)))
				throw -2;
		}else //DOWNLOAD_TYPE_DETAIL == p_req->CreateType()
		{
			if(SUCCESS != (ret = build_detail_result_file(p_req->GetTextID(), result_file_name)))
				throw -3;
		}
		

		p_res->SetResultFileName(result_file_name);

	}catch(int ex)
	{
		lerr << "Create result text failed. ex = " << ex << " err = " << ret << endl;
	}

	//处理完毕
	p_res->SetResult(ret);

	return p_res;

}


EventData * TextHandler::on_load_text(EventData * p_edata)
{
	if(!p_edata)
	{
		lerr << "Input EventData is null." << endl;
		return NULL;
	}

	//转换为TextLoadReq
	TextLoadReq * p_req = dynamic_cast<TextLoadReq*>(p_edata);

	if(!p_req)
	{
		lerr << "Convert to TextLoadReq failed." << endl;
		return NULL;
	}

	//生成res
	
	TextLoadRes * p_res = new TextLoadRes(p_req->GetCallID(), p_req->GetTransText());

	if(!p_res->GetTransText())
	{
		lerr << "Input TextLoadRes's TransText is null." << endl;
		return NULL;
	}

	//开始处理
	TransText & trans_text = *(p_res->GetTransText());
	int ret = SUCCESS;
	char * p_data = NULL;

	try
	{
		//Step 1 从数据库读取信息 //同步调用
		string file_type;
		string file_path;
		size_t len = 0;
		if(SUCCESS != (ret = DBOperation::GetTransFileInfo(trans_text.GetID(), file_type, file_path)))
			throw -1;


		file_path = m_default_file_path + file_path;
		trans_text.SetFilePath(file_path);
		trans_text.SetTextType(file_type);
		CLanguage::EnLowercase(file_type);

		//lout << " File GetID:" << trans_text.GetID() << endl;
		lout << " File Path:" << file_path << endl;
		//Step 2 转换格式
		string text;

		if(TEXT_TYPE_TXT == file_type)
		{
			if(SUCCESS != (ret = convert_txt_to_text(file_path, text)) )
				throw -1;
		}else if (TEXT_TYPE_DOC == file_type || TEXT_TYPE_DOCX == file_type)
		{
			if(SUCCESS != (ret = convert_word_to_text(file_path, trans_text.GetID(), file_type, text)) )
				throw -1;
		}else if (TEXT_TYPE_HTML == file_type)
		{
			if(SUCCESS != (ret = convert_html_to_text(file_path, trans_text.GetID(), text)) )
				throw -1;
		}else if (TEXT_TYPE_PDF == file_type)
		{
			if(SUCCESS != (ret = convert_pdf_to_text(file_path, trans_text.GetID(), text)) )
				throw -1;
		}else
		{
			ret = ERR_UNSUPORT_FILE;
			throw -1;
		}
		
		//Step 3 解析内容
		if(SUCCESS != (ret = build_base_source(text, trans_text)) )
			throw -1;
            
		//Step 4 提交摘要
		DBOperation::SubmitAbstract(trans_text.GetID(), trans_text.Abstract());

	}catch(...)
	{
		lerr << "Load text failed. err = " << ret << endl;
	}

	if(p_data)
		delete p_data;

	//处理完毕
	p_res->SetResult(ret);
	//lout << "cid = " << p_res->GetCallID() << endl;

	return p_res;
}

string & TextHandler::del_utf8_bom(string & src)
{
	//去掉utf-8 bom头

	if(src.size() < 3)
		return src;

	if((unsigned char)src[0] == 0xEF
		&& (unsigned char)src[1] == 0xBB
		&& (unsigned char)src[2] == 0xBF)
	{
		src.erase(0,3);
	}

	return src;
}
