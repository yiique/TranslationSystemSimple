#include "Common/f_utility.h"
#include "Common/ICUConverter.h"

#include <fstream>
#include <string>
#include <iostream>
using namespace std;

int main()
{
	//string uuid = GenerateUUID();
	//cout << uuid << endl;
	//cout << GenerateUUID() << endl;

	ifstream file("in.txt");
	ofstream outfile("out.txt");
	string line;
	getline(file, line);
	
	file.close();
	
	outfile << line << endl;
	ICUConverter::Convert("BIG5", "UTF-8", line);
	outfile << line << endl;
	
	outfile.close();

	ICUConverter::PrintAllCharset();
	
	return 0;
}