#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include "Segmentor.h"
#include "splitutf.h"
//#include "MultiThreadPack.h"

using namespace std;


/*struct TestPara 
{
	ifstream* fin;
	ofstream* fout;
};
class ConvThreadClass : public TemplateThreadClass<TestPara, string, string>
{
public:

	static void init()
	{
		conv.Initialize();//sunm
	}
	static bool read(__para_type* p_para, __in_type* out_data)
	{
		ifstream* p_fin = ((TestPara*)p_para)->fin;
		string line;
		if (getline(*p_fin, line) == false)
			return false;

// 		stringstream sstream(line);
// 		sstream >> *out_data;
		*out_data = line;
		return true;
	}

	static bool run(thread_data* data)
	{
//		stringstream sstream;
//		sstream << "\"" << *(data->in_value) << "+1=" << *(data->in_value)+1 << "\"";
		
//		data->out_value = sstream.str();
		string strToken;
		conv.token(((string)(data->in_value)),strToken);//sunm
		data->out_value = strToken;
		return true;
	}

	static bool out(__para_type* p_para, __out_type* out_data)
	{
		*(p_para->fout) << *out_data << endl;
//		cout << *out_data << endl;
		return true;
	}
private:
	static CSegmentor conv;
};

CSegmentor ConvThreadClass::conv;
*/

int main(int argc ,char *argv[])
{
	titoken::CSegmentor segmentor;
	segmentor.Initialize(".");

	ifstream fin(argv[1]);
	ofstream fo(argv[2]);

	string line;

	while(getline(fin,line))
	{
		string tgt;
		segmentor.token(line,tgt);
		fo<<tgt<<endl;
	}

	fin.close();
	fo.close();

	/*ifstream fin("dev");
    ofstream fout("out");
    MultiThreadPack<ConvThreadClass> test_multi_thread; //sunm
    TestPara para;
    para.fin = &fin;
    para.fout = &fout;
    test_multi_thread.main_func(&para, 5);
    test_multi_thread.wait2quit();*/

	return 0;
}
