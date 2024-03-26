#include <iostream>
#include <netdb.h>
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "arpa/inet.h"
#include "pthread.h"
#include "threadpool.h"
using namespace std;

struct Cliet{
    sockaddr_in addr;
    int cfd;
};

struct Sock{
    ThreadPool *threadPool;
    int fd;
};

void working(void*arg){
    Cliet *cliet=(Cliet*)arg;
    //5.打印客户信息
    char cip[32];
    cout<<"客户端IP:"<<inet_ntop(AF_INET,&cliet->addr.sin_addr.s_addr,cip, sizeof (cip))<<"端口号:"<<ntohs(cliet->addr.sin_port)<<endl;

    //6.通信
    while (1){
        //接收数据
        char buffer[1024];
        int len= recv(cliet->cfd,buffer, sizeof (buffer),0);
        if (len>0){
            cout<<"客户说:"<<buffer<<endl;
            send(cliet->cfd,buffer, len,0);
        } else if (len==0){
            cout<<"客户端断开连接"<<endl;
            break;
        } else{
            perror("recv失败");
            break;
        }
    }
    close(cliet->cfd);
}

void con(void *arg){
    //4.阻塞并等待客户端连接
    Sock *sock=(Sock*)arg;
    while (1){
        Cliet *cliet=new Cliet;
        socklen_t caddrlen = sizeof(cliet->addr);
        int cfd = accept(sock->fd, (sockaddr*)&cliet->addr, &caddrlen);
        if (cfd==-1){
            perror("accept创建失败");
            break;
        }
        cliet->cfd=cfd;
        taskAdd(sock->threadPool,working,cliet);
    }
    //7.关闭文件描述符
    close(sock->fd);
}

int main() {
    //1.创建套接字
    int fd= socket(AF_INET,SOCK_STREAM,0);
    if (fd==-1){
        perror("socket创建失败");
        return -1;
    }

    //2.绑定IP
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port= htons(8888);
    addr.sin_addr.s_addr=INADDR_ANY;//自动锁定服务器自身的IP
    int ret= bind(fd,(sockaddr*) &addr,sizeof (addr));
    if (ret==-1){
        perror("bind创建失败");
        return -1;
    }

    //3.设置监听
    ret= listen(fd,128);
    if (ret==-1){
        perror("listen创建失败");
        return -1;
    }

    ThreadPool *threadPool=threadPool_Create(3,10,100);
    Sock *sock=new Sock;
    sock->threadPool=threadPool;
    sock->fd=fd;
    taskAdd(threadPool,con,sock);

    pthread_exit(NULL);
    return 0;
}