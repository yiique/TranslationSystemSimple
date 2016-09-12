#include"func.h"

//func::func(const string& numc, const string& necf)
//	:numcfg(numc), necfg(necf), min_ne_rec(numc, necf){}
namespace UYNE
{

bool func::init(const string& loc)
{
	numcfg = loc + "/UyNeNum.cfg";
	necfg = loc + "/UyNe.cfg";
	min_ne_rec = new NEMain(numcfg, necfg);
	return (*min_ne_rec).flag;
}

vector<string> func::process(const string& srcStr)
{
	vector<string> result;
	//ifstream srcin(fileIn.c_str());
	ofstream logout("log.txt");
	string sent = srcStr;
	string out_ne;
	out_ne = (*min_ne_rec).NETrans(sent/*,lineid*/);
	map<string,string>::iterator iter;
	for( iter = (*min_ne_rec).vMaplog.begin(); iter != (*min_ne_rec).vMaplog.end(); iter++)
	{	
		pair<int,int> span;
		int a = sent.find(iter->first);
		string pre = sent.substr(0,a);
		int countPre = 0;
		int countNe = 0;
		stringstream iss(pre);
		while(!iss.eof())
		{
			string a;
			iss >> a;
			countPre++;					
		}
		stringstream iss1(iter->first);
		while(!iss1.eof())
		{
			string a;
			iss1 >> a;
			countNe++;					
		}
		span.first = countPre ;
		span.second = span.first + countNe - 1;
		/*span.first = sent.find(iter->first);
		span.second = span.first + iter->first.size();*/
		//logout << iter->first << '\t' << iter->second << '\t' << "(" << span.first << ", " << span.second << ")" << endl;
		stringstream tmpss;
		tmpss << span.first << ":" << span.second << " ||| " << iter->first << " ||| " << iter->second << " ||| " << "1 1 1 1" << endl ;
		//logout << span.first << ":" << span.second << " ||| " << iter->first << " ||| " << iter->second << " ||| " << "1 1 1 1" << endl;
		//logout <<tmpss.str() << endl;
		//return tmpss.str();
		//cout << span.first << ":" << span.second << endl;
		//cout << iter->first << " ||| " << endl;
		//cout << iter->second << " ||| " << endl;
		//cout << "1 1 1 1" << endl ;
		//cout << tmpss.str() << endl;
		result.push_back(tmpss.str());
	}
	return result;
	//tgtout << out_ne << endl;
}
}