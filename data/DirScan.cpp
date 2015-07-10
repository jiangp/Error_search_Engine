#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <map>
#include <utility>
#include <set>
#include <functional>
#include <algorithm>

//扫描目录类，该类扫描指定目录下的项并将属于普通文件的项的绝对路径保存下来。
class DirScan
{
	public:
		//带参数的构造函数，传入一个vector容器用于保存文件的绝对路径
		DirScan(std::vector<std::string>& vec):m_vec(vec)
		{
		
		}
		//重载函数调用操作符，传入一个路径。
		void operator()(const std::string &dir_name)
		{
			traverse(dir_name);
		}
		//遍历路径，在遍历的过程中将属于文件类型的项的绝对路径保存到vector容器中。
		//遍历算法：
		// 打开该目录，进入该目录，依次遍历该目录中的项，判断该项的属性，如果该项的类型是文件则保存该项的绝对路径，如果该项是目录，则递归的遍历该目录。最后遍历完目录后切换到该目录的上一级目录。
		void traverse(const std::string& dir_name)
		{
			//打开指定的目录
			DIR* pdir = opendir(dir_name.c_str());
			if(pdir == NULL)
			{
				std::cout << "dir open" << std::endl ;
				exit(-1);
			}
			//进入指定的目录
			chdir(dir_name.c_str());
			struct dirent* mydirent ;
			struct stat mystat ;
			//依次遍历该目录中的相关项
			while((mydirent =readdir(pdir) ) !=NULL)
			{
				//获取目录中项的属性
				stat(mydirent->d_name, &mystat);
				//判断该项是不是目录。
				if(S_ISDIR(mystat.st_mode))
				{
					//如果该目录是'.'和‘..’（每个目录下都有这两项，如果不排除这两项程序会进入无限循环），则跳过该次循环继续下一次
					if(strncmp(mydirent->d_name, ".", 1)== 0 || strncmp(mydirent->d_name,"..", 2) == 0)
					{
						continue ;
					}else//如果该目录不是前二者，则递归的遍历目录 
					{
						traverse(mydirent->d_name);
					}
				}else //如果该项不是目录（是文件）,则保存该项的绝对路径
				{
					std::string file_name="";
					file_name = file_name + getcwd(NULL,0)+"/"+ mydirent->d_name ;
					m_vec.push_back(file_name);
				}
			}
			chdir("..");
			closedir(pdir);
		}
	private:
		//类外一个vector容器的引用，用于保存文件的绝对路径
		std::vector<std::string>& m_vec ;
};
//文件处理类，该类将多个文件已某种格式格式化文件并将各个文件统一保存到一个文件形成网页库文件。
//每个文件被处理成<doc><docid>id</docid><doctitle>title</doctitle><docurl>url</docurl><doccontent>content</doccontent></doc>

class FileProcess
{
	public:
		//带参数的构造函数，第一个参数为保存着个文件路径的vector容器，第二个参数为传入的字符串用于提取文档中的‘标题’
		FileProcess(std::vector<std::string>& vec, std::string& str):m_vec(vec)
		{
			m_title = str ;
		}
		//重载函数调用操作符，传入两个文件名用于保存建好的网页库和单个文档在网页库中的偏移位置
		void operator()(const std::string &file_name, const std::string & offset_file)
		{
			
			do_some(file_name, offset_file) ;
		}
		//建立网页库，并将其以及文档在库中的偏移保存到文件中。
		void do_some(const std::string& file_name, const std::string& offset_file)
		{
			//用于保存网页库的文件指针
			FILE* fp = fopen(file_name.c_str(),"w");
			//用于保存文档在网页库中偏移的文件指针
			FILE* fp_offset = fopen(offset_file.c_str(), "w");
			if(fp == NULL || fp_offset == NULL)
			{
				std::cout << "file open" << std::endl ;
				exit(0); 
			}
			int index ;
			//动态创建一个字符数组，用于保存从文件中读取的全部内容
			char* mytxt = new char[1024*1024]() ;
			int mydocid ;
			char myurl[256] = "" ;
			//动态创建一个字符数组，用于保存文件内容
			char* mycontent = new char[1024 * 1024]() ;
			//保存文档标题
			char* mytitle = new char[1024]() ;
			//依次处理各个文档。处理包括：生成文档id（该id具有全局唯一性），提取文档标题，生成文档url（文档的绝对路径），提取文档内容
			for(index = 0 ; index != m_vec.size(); index ++)
			{
				memset(mytxt, 0, 1024*1024);
				memset(myurl, 0, 256);
				memset(mycontent, 0, 1024* 1024);
				memset(mytitle, 0, 1024);
				//打开指定的文档
				FILE * fp_file = fopen(m_vec[index].c_str(), "r");
				//读取文档，并将标题保存到mytitle
				read_file(fp_file, mycontent, mytitle);
				fclose(fp_file);
				mydocid = index + 1 ;
				strncpy(myurl, m_vec[index].c_str(), m_vec[index].size());
				//将文档格式化成指定格式的串
				sprintf(mytxt, "<doc><docid>%d</docid><docurl>%s</docurl><doctitle>%s</doctitle><doccontent>%s</doccontent></doc>\n", mydocid, myurl, mytitle, mycontent);
				//算出文档在网页库的起始位置
				int myoffset = ftell(fp);
				int mysize = strlen(mytxt);
				char offset_buf[128]="";
				//文档的偏移通过 （文档id  文档在网页库的起始位置  文档的大小）这三个数字来确定， 在文件中占一行
				//将文档偏移信息写入到偏移文件中去
				fprintf(fp_offset,"%d\t%d\t%d\n",mydocid, myoffset, mysize);
				//将格式化后的文档写入网页库中
				write_to_file(fp, mytxt); 
			}
			fclose(fp);
		}
		//读取文档的内容，并提取标题，分别把内容和标题保存到mycontent 和 mytitle所指向的空间中去
		void read_file(FILE* fp ,  char* mycontent, char* mytitle )
		{
			int iret ;
			const int size = 1024 * 1024 ;
			char* line = new char[1024]() ;
			int pos = 0 ;
			//  循环读取文档内容
			while(1)
			{
				int iret = fread( mycontent + pos, 1, size - pos, fp);
				if(iret == 0)//如果文档读完，则跳出循环
				{
					break ;
				}else //如果没有读完，则接着原来的地方继续读
				{
					pos += iret ;
				}
			}
			//将文件指针重新回到文档的开头，用于提取标题
			rewind(fp) ;
			//count   记录当前读到的行数，flag记录是否已经找到标题（0代表没有找到，1代表已经找到）。
			int count = 0, flag = 0 ; ;
			//依次取出文档的前11行 ， 看看每行有没有 ‘标题’二字，如果有则将改行作为标题，如果没有则直接将下一行（第12行）作为标题
			//如果整篇文档没有11行，则直接将第一行作为标题
			while(count <=10 && fgets(line, 512, fp) != NULL)
			{
				std::string str_line(line);
					//如果改行有‘标题’ 二字
				if( str_line.find(m_title.c_str(), 0) != std::string::npos)
				{
					//将该行赋值给mytitle，作为标题
					strncpy(mytitle, str_line.c_str(), str_line.size());
					flag = 1 ;
					break ;
				} 
				count ++ ;
					
			}
			if(count < 11 && flag == 0)// 如果文档没有12行，将第一行作为标题
			{
				rewind(fp);
				fgets(mytitle,1024, fp );
			}else if(count == 11 && flag == 0)//如果有12行，则将12行作为标题
			{
				
				fgets(mytitle,1024, fp );
			}
			
			
		}
		//将格式化后的文档写到网页库文件中
		void write_to_file(FILE* fp, char* mytxt)
		{
			int iret , pos = 0 ;
			int len = strlen(mytxt);
			//循环写到网页库文件中，直到写完
			while(1)
			{
				iret = fwrite(mytxt + pos, 1, len - pos, fp);
				len = len - iret ;
				if(len == 0)
				{
					break ;
				}  
			}
		}
	private:
		//保存文件路径的容器的引用。
		std::vector<std::string>& m_vec ;
			//用于保存提取标题
		std::string m_title ;
		std::map<int, std::pair<int, int> > m_offset ;
};
void show(std::vector<std::string>::value_type& val)
{
	std::cout << val << std::endl ;
}
int main(int argc, char* argv[])//exe  src_txt_dir  ripepage_filename  offset_file_name
{
	// 初始化一个容器，用于保存文档的路径
	std::vector<std::string> str_vec ;
	// 定义一个扫描目录的对象
	DirScan mydirscan(str_vec);
	mydirscan(argv[1]);
	//for_each(str_vec.begin(), str_vec.end(),show );
	std::cout << str_vec.size() << std::endl ;
	std::string title ;
	std::cin >> title ;
	std::cout << title << std::endl ;
	// 定义一个文件处理对象
	FileProcess myfileprocess(str_vec, title);
	myfileprocess(argv[2], argv[3]);
	std::cout << "Over" << std::endl ;
	return 0 ;
}
