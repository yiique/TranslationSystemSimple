#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

//BOOST
#include <boost/enable_shared_from_this.hpp>
#include <boost/logic/tribool.hpp>

//MAIN
#include "f_utility.h"

//STL
#include <string>
#include <map>
#include <sstream>
using namespace std;

class HttpHeaderStream;

class HttpHeader
{
public:

	static const string HEAD_REQUEST_GET;
	static const string HEAD_REQUEST_POST;
	static const string HEAD_VERSION;
	static const string HEAD_RESPONSE_200;
	static const string HEAD_RESPONSE_400;
	static const string HEAD_RESPONSE_403;
	static const string HEAD_RESPONSE_404;
	static const string HEAD_RESPONSE_500;
	static const string HEAD_SEP_TAG;

private:
	friend class HttpHeaderStream;

	HttpHeader();

public:

	HttpHeader(const string & head);

	string GetField(const string & name); 

	string Method();
	void Method(const string & method);

	string Url();
	void Url(const string & url);
	string UrlPath();
	string UrlArgv(const string & name);

	size_t ContentLength();
	void ContentLength(const size_t length);

	void Host(const string & host);

	const string & Data();

private:
	string m_data;
	bool m_is_request;
	string m_head_line;

	map<string, string> m_field_map;
	bool m_completion;

	bool completion() const { return m_completion; }
	bool check_create();
	string & conv_url(string & url);
	
};

typedef boost::shared_ptr<HttpHeader> http_header_ptr;

class HttpHeaderStream
{
public:
	HttpHeaderStream();

	boost::logic::tribool Write(const char * data, const size_t len);

	http_header_ptr Header() const { return msp_http_header; }

	size_t UnhandleSize() const { return m_unhandle_size; }

private:

	size_t m_unhandle_size;
	http_header_ptr msp_http_header;

};

#endif //HTTP_HEADER_H

