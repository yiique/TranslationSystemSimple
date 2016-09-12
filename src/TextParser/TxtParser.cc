#include "TxtParser.h"

TxtParser::TxtParser(void)
{
}

TxtParser::~TxtParser(void)
{
}


bool TxtParser::Parse(const string & src,
					  vector<string> & para_vec, 
					  vector<pair<size_t,size_t> > & pos_vec)
{

	pair<size_t, size_t> line_pos;
	pair<size_t, size_t> content_pos;

	size_t offset = 0;

	while(get_line(src, offset, line_pos))
	{
		if(parse_line(src, line_pos, content_pos))
		{
			para_vec.push_back( src.substr(content_pos.first, content_pos.second) );
			pos_vec.push_back( content_pos );
		}

		offset = line_pos.first + line_pos.second;
	}

	
	return true;
}

bool TxtParser::parse_line(const string & src, 
						   const pair<size_t, size_t> & line_pos, 
						   pair<size_t, size_t> & content_pos)
{
	if(line_pos.first + line_pos.second > src.size() 
		|| line_pos.second == 0 )
		return false;

	//过滤掉头部空格和TAB
	size_t beg = line_pos.first;
	for(; beg < line_pos.first + line_pos.second; ++beg)
	{
		if(src[beg] != ' '
			&& src[beg] != '\t'
			&& src[beg] != '\r'
			&& src[beg] != '\n')
			break;
	}

	//过滤尾部字符
	size_t end = line_pos.first + line_pos.second - 1;
	for(; end > beg; --end)
	{
		if(src[end] != ' '
			&& src[end] != '\t'
			&& src[end] != '\r'
			&& src[end] != '\n')
			break;
	}

#ifdef IGNORE_TEXT_FLAG
	beg = ignore_flag(src, beg);
#endif //IGNORE_TEXT_FLAG
	
	if(beg <= end)
	{
		content_pos.first = beg;
		content_pos.second = end - beg + 1;
		return true;
	}else
	{
		return false;
	}
}


bool TxtParser::get_line(const string & src, 
						 const size_t offset, 
						 pair<size_t, size_t> & line_pos)
{
	//只需识别\n，后面有parse_line再作过滤
	if(offset >= src.size())
		return false;
	
	size_t beg = offset;

	while(beg < src.size())
	{
		line_pos.first = beg;
		size_t pos = src.find('\n', beg);

		if(string::npos == pos)
		{
			pos = src.size();
		}

		if( pos > beg )
		{
			line_pos.second = pos - beg;
			break;
		}else
		{
			beg = pos + 1;
		}
	}

	return true;
}

size_t TxtParser::ignore_flag(const string & src, const size_t offset)
{
	if(offset >= src.size() || src.size() < 8)
		return offset;

	const char flag1[] = {0x2a, 0x2a, 0x2e, 0x00};
	const char flag2[] = {0x2a, 0x2a, 0xe3, 0x80, 0x82, 0x00};

	if(src[offset] != '*' || src[offset+1] != '*')
		return offset;

	size_t pos = offset+2;

	while(pos < src.size())
	{
		if(src[pos] < '0' || src[pos] > '9')
		{
			if(pos == src.find(flag1, pos))
				return pos+3;
			else if(pos == src.find(flag2, pos))
				return pos+5;

			return offset;
		}
		
		++pos;
	}

	return offset;
}
