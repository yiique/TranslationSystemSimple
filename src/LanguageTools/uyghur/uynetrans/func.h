#ifndef FUNK_H
#define FUNK_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "NEMain.h"
using namespace std;
namespace UYNE
{
const string namedic1= "mrk_la2ch.txt";                 
const string namedic2= "fname_la2ch.txt";
const string namedic3= "sname1_la2ch.txt";
const string namedic4= "sname2_la2ch.txt";

class func
{
public:
	//func(const string& numc, const string& necf);

	bool init(const string& loc);

	vector<string> process(const string& src);

private:
	string numcfg;
	string necfg;
	NEMain*  min_ne_rec;
};
}
#endif