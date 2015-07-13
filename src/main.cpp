/*************************************************************************
	> File Name: main.cpp
	> Author: Arwen
	> Mail:745529725@qq.com 
	> Created Time: Sun 29 Mar 2015 09:09:42 PM CST
 ************************************************************************/

#include<iostream>
#include<unistd.h>
#include<time.h>
#include<sys/epoll.h>
#include"pool.h"
#include"inetaddress.h"
#include"socket.h"
#include"socketIO.h"
#include"epoll.h"
//#include"redis.h"


int main(int argc, char **argv)
{
	if(argc < 2 ){
		perror("No input file!"); 
		exit(EXIT_FAILURE);
	}
	
	
	//conf
	MyConf conf(argv[1]);      //配置类

	
	//Cache                     //缓存
	Cache cache(conf.get_word_path());

	/*//Redis                   //使用Redis缓存
	 * Redis *r = new redis();
	 * if(!r->connect("127.0.0.1", 6379)){
	 *		perror("connect error\n");
	 *		return 0;
	 * }
	 * r->read_from_file();
	 * */

	//pool
	Pool_t pool(conf.get_queueSize(), conf.get_threadNum());          //线程池
	pool.start();

	//inetAddress
	InetAddress addr(conf.get_IP(), conf.get_PORT()); //地址连接
	
	//socket
	int fd_listen;
	Socket sock;                //socket连接
	fd_listen = sock.init(0);
	sock.bindAddress(addr);
	sock.listen();

	//epoll
	
	Epoll init(fd_listen);       //监听函数

	while(1){
		init.epoll_loop();
		init.epoll_handle_fd(pool, conf, cache); //redis
	}
	init.epoll_destroy();
	return 0;
}
