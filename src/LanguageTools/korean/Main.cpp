#ifndef MAX_PATH
#define MAX_PATH 1024
#endif


#if !defined(_WIN32)	
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include "stdio.h"
#include "string.h"
char *get_exe_path()
{
    static char buf[MAX_PATH];
    int rslt=readlink("/proc/self/exe",buf,MAX_PATH);
    if (rslt<0||rslt>=MAX_PATH) return NULL;
    while (buf[--rslt]!='/') { buf[rslt]='\0'; }
    return buf;
}

#else
#include "stdio.h"
#include "string.h"
#endif


class KMA_DLL{
public:
    KMA_DLL();
    virtual ~KMA_DLL();
    char *proc_sen(const char *sen);
    bool  create(const char *ini_file);
};    


int main(int argc, char* argv[]) 
{
	char path[MAX_PATH];
	KMA_DLL  kma;

#if !defined(_WIN32)	
	sprintf(path, "%skma.ini", get_exe_path());
#else
	strcpy(path, "kma.ini");
#endif

	if(!kma.create(path))
	{	printf("Failed in opening dictionar!\n");
		return -1;
	}

	if(argc != 3)
	{	printf("Incorrect command, please enter ./kma inputfilename  outputfilename.\n");
		return -1;
	}
	FILE *fp = fopen(argv[1], "rt");
	if(fp == NULL)
	{	printf("No inputfile!\n");
		return -1;
	}
  //      char* outfile="out";
	FILE *wp = fopen(argv[2], "wb");
	if(wp == NULL)
	{	printf("Incorrect path for output file!\n");
		return -1;
	}
	char in_buff[2001];
	char *out_buff;
	int n = 0;
	while(fgets(in_buff, 2000, fp))
	{
               n++;
              if(n%10000 == 0 )
                        printf("%d\n", n);
                in_buff[strlen(in_buff)-1] = 0;
		out_buff = kma.proc_sen(in_buff);
		strcat(out_buff, "\n");
		fwrite(out_buff, strlen(out_buff), 1, wp);
                        //printf("\n");
	}
#if !defined(_WIN32)	
	printf("\nFinished! Total sentence = %d\n", n);
#else
	printf("\nFinished! Total sentence = %d\nInput <Enter> key.", n);
	scanf("%c",in_buff);
#endif
	fclose(fp);
	fclose(wp);
	return 0;
}
