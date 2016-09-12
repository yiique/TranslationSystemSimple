#include "HttpHeader.h"
#include <assert.h>
#include <iostream>

#define MAX_HTTP_HEADER_SIZE 15000


const string HttpHeader::HEAD_REQUEST_GET("GET");
const string HttpHeader::HEAD_REQUEST_POST("POST");
const string HttpHeader::HEAD_VERSION("HTTP/1.1");
const string HttpHeader::HEAD_RESPONSE_200("200 OK");
const string HttpHeader::HEAD_RESPONSE_400("400 Bad Request");
const string HttpHeader::HEAD_RESPONSE_403("400 Forbidden");
const string HttpHeader::HEAD_RESPONSE_404("404 Not Found");
const string HttpHeader::HEAD_RESPONSE_500("500 Internal Server Error");
const string HttpHeader::HEAD_SEP_TAG("\r\n");


HttpHeaderStream::HttpHeaderStream(): msp_http_header(new HttpHeader()), m_unhandle_size(0)
{
}

boost::logic::tribool HttpHeaderStream::Write(const char * data, const size_t len)
{
	assert(data);
	enum { R1, N1, R2, N2, SUC};

	if(msp_http_header->completion())
		return true;

	size_t state = R1;

	for(size_t i=0; i<len; ++i)
	{
		const char c = data[i];

		if(c == 0)
		{
			cerr << "Head cant content NULL value." << endl;
			m_unhandle_size = len - i -1;
			return false;
		}

		msp_http_header->m_data += c;

		if(msp_http_header->m_data.size() > MAX_HTTP_HEADER_SIZE)
		{
			cerr << "Max http header size full : " << msp_http_header->m_data.size() << endl;
			m_unhandle_size = len - i -1;
			return false;
		}

		if(R1 == state && c == '\r')
		{
			state = N1;
		}else if(N1 == state && c == '\n')
		{
			state = R2;
		}else if(R2 == state && c == '\r')
		{
			state = N2;
		}else if(N2 == state && c == '\n')
		{
			state = SUC;
		}else
		{
			state = R1;
		}

		if(state == SUC)
		{
			m_unhandle_size = len - i -1;
			return msp_http_header->check_create();
		}

	}


	m_unhandle_size = 0;
	return boost::indeterminate;
}


HttpHeader::HttpHeader(): m_is_request(true), m_completion(false)
{
}

HttpHeader::HttpHeader(const string & head): m_completion(true)
{
	//fix it : 如果不输入指定的类型
	if(head == HEAD_REQUEST_GET || head == HEAD_REQUEST_POST)
	{
		m_head_line = head + " / " + HEAD_VERSION;
		m_field_map["Method"] = head;
		m_field_map["Url"] = "/";

		m_is_request = true;

	}else
	{
		m_head_line = HEAD_VERSION + " " + head;
		m_is_request = false;
	}
}

const string & HttpHeader::Data()
{
	if(m_is_request)
		m_data = m_field_map["Method"] + " " + m_field_map["Url"] + " " + HEAD_VERSION + HttpHeader::HEAD_SEP_TAG;
	else
		m_data = m_head_line + HttpHeader::HEAD_SEP_TAG;

	for(map<string, string>::iterator iter = m_field_map.begin(); iter != m_field_map.end(); ++iter)
	{
		if(iter->first != "Method" && iter->first != "Url")
			m_data += iter->first + ": " + iter->second + HttpHeader::HEAD_SEP_TAG;
	}

	m_data += HttpHeader::HEAD_SEP_TAG;

	return m_data;
}

string HttpHeader::GetField(const string & name)
{
	return m_field_map[name];
}

void HttpHeader::Host(const string & host)
{
	m_field_map["Host"] = host;
}

string HttpHeader::Method()
{
	return m_field_map["Method"];
}

void HttpHeader::Method(const string & method)
{
	if(!m_is_request) return;

	if(method == HEAD_REQUEST_GET || method == HEAD_REQUEST_POST)
	{
		m_field_map["Method"] = method;
		m_data.clear();
	}
}

string HttpHeader::Url()
{
	return m_field_map["Url"];
}


void HttpHeader::Url(const string & url)
{
	m_field_map["Url"] = url;
	m_data.clear();
}

size_t HttpHeader::ContentLength()
{
	map<string, string>::const_iterator iter = m_field_map.find("Content-Length");

	if(iter == m_field_map.end())
		return 0;

	stringstream ss;
	ss << iter->second;

	size_t length = 0;
	ss >> length;

	return length;
}

void HttpHeader::ContentLength(const size_t length)
{
	stringstream ss;
	ss << length;
	ss >> m_field_map["Content-Length"];
	
}

string & HttpHeader::conv_url(string & url)
{
	string tgt;

	char buf[3];
	buf[2] = 0;

	size_t pos = 0;

	while(pos < url.size())
	{

		if(url[pos] == '%' && pos+2 < url.size())
		{
			buf[0] = url[pos+1];
			buf[1] = url[pos+2];

			//cout << "url buf: " << buf << endl;

			char * stop = NULL;
			char c = (char) strtol(buf, &stop, 16);
			//cout << "c: " << c << endl;
			if(stop == buf+2)
			{
			   tgt += c;
			   pos += 3;
			   continue;
			}

		}


		tgt += url[pos];
		++pos;
	}

	return (url = tgt);
}

bool HttpHeader::check_create()
{

	//cout << m_data << endl;
	stringstream ss;

	ss << m_data;

	//first line
	string line;
	getline(ss, line);
	filter_head_tail(line);

	vector<string> content_vec;
	split_string_by_tag(line.c_str(), content_vec, ' ');

	if(content_vec.size() != 3)
		return (m_completion = false);

	if(content_vec[0] == HEAD_REQUEST_GET || content_vec[0] == HEAD_REQUEST_POST)
	{
		m_is_request = true;
		m_field_map["Method"] = content_vec[0];
		m_field_map["Url"] = conv_url(content_vec[1]);
		m_is_request = true;
	}else if(content_vec[0] == HEAD_VERSION)
	{
		string response;
		for(size_t i=1; i<content_vec.size(); ++i)
		{
			response += content_vec[i];

			if(i+1 != content_vec.size())
				response += " ";
		}

		m_field_map["Response"] = response;
		m_is_request = false;
	}else
	{
		cerr << "Head first line error: " << line << endl;
		return (m_completion = false);
	}

	while(getline(ss, line))
	{
		filter_head_tail(line);

		if(line.size() == 0)
			continue;

		size_t pos = line.find(':');

		if(pos == string::npos)
		{
			cerr << "Head line error: " << line << endl;
			continue;
		}
		
		string field = line.substr(pos+1);
		m_field_map[line.substr(0, pos)] = filter_head_tail(field);
	}

	/*map<string, string>::iterator iter = m_field_map.begin();

	for(; iter !=m_field_map.end(); ++iter)
	{
		cout << "[" << iter->first << "] - [" << iter->second << "]" << endl;
	}
*/
	return (m_completion = true);


}


string HttpHeader::UrlPath()
{
	const string & full_url = m_field_map["Url"];

	size_t pos = full_url.find('?');

	if(pos == string::npos)
		return full_url;
	else
		return full_url.substr(0, pos);
}

string HttpHeader::UrlArgv(const string & name)
{
	const string & full_url = m_field_map["Url"];

	size_t pos = full_url.find('?');

	if(pos == string::npos)
		return "";

	vector<string> argv_vec;

	split_string_by_tag(full_url.substr(pos+1).c_str(), argv_vec, '&');

	for(size_t i=0; i<argv_vec.size(); ++i)
	{
		size_t epos = argv_vec[i].find('=');
		
		if(pos != string::npos)
		{
			string tmp = argv_vec[i].substr(0, epos);
			string tmp2 = argv_vec[i].substr(epos+1);
			if(name ==  filter_head_tail(tmp))
				return filter_head_tail(tmp2);
		}
	}

	return "";
	
}

