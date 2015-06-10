/*************************************************************************
	> File Name: data/init_word_fre.cpp
	> Author: Arwen
	> Mail:745529725@qq.com 
	> Created Time: Sun 29 Mar 2015 03:54:53 PM CST
 ************************************************************************/



#include<iostream>
#include<string.h>
#include<stdio.h>
#include<map>
#include<dirent.h>
#include<unistd.h>
#include<fstream>
#include<sstream>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
using namespace std;

map<string, int> m_map;
/*
void get_dir(char *path);
void init_word(char *path);

void get_dir(char *path)
{
	DIR *pdir = opendir(path);
	if(pdir == NULL)
	{
		perror("dir open fail!");
		exit(1);
	}
	chdir(path);
	struct dirent* mydirent;
	struct stat mystat;
	char name[20] = {0};

	while((mydirent = readdir(pdir)) != NULL)
	{
		stat(mydirent->d_name, &mystat);
		if(S_ISDIR(mystat.st_mode))
  		{
			if((strncmp(mydirent->d_name , "..", 2) == 0 )|| (strncmp(mydirent->d_name , ".", 1) == 0))
				continue;
			else
			{
		
				get_dir(mydirent->d_name);
			}
		}else
		{
	
			init_word(mydirent->d_name);
		}

	}

}
*/
void init_word(char *path)
{
	ifstream ifs(path);
	string line ,word;
	string::iterator iter;
	while(getline(ifs, line))
	{
		for(iter = line.begin();iter != line.end(); ++iter)
		{
			if(ispunct(*iter))
			{
				(*iter) = ' ';
			}else if(isupper(*iter))
			{
				*iter = tolower(*iter);
			}
		}
		istringstream is(line);
		while(is >>word)
		{
			bool flag = true;
			for(iter = word.begin(); iter != word.end(); ++iter)
			{
				if(!isalpha(*iter))
					flag = false;
			}
			if(flag)
				++m_map[word];
		}

	}
	ifs.close();
}

int main(int argc, char *argv[])
{
	ofstream ofs("word.txt");
	init_word(argv[1]);
	map<string, int>::iterator it = m_map.begin();
	for(; it != m_map.end(); ++it)
	{
		 ofs << it->first << " " << it ->second << endl;
	}
	ofs.close();
}
