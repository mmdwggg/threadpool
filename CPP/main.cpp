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
    //5.��ӡ�ͻ���Ϣ
    char cip[32];
    cout<<"�ͻ���IP:"<<inet_ntop(AF_INET,&cliet->addr.sin_addr.s_addr,cip, sizeof (cip))<<"�˿ں�:"<<ntohs(cliet->addr.sin_port)<<endl;

    //6.ͨ��
    while (1){
        //��������
        char buffer[1024];
        int len= recv(cliet->cfd,buffer, sizeof (buffer),0);
        if (len>0){
            cout<<"�ͻ�˵:"<<buffer<<endl;
            send(cliet->cfd,buffer, len,0);
        } else if (len==0){
            cout<<"�ͻ��˶Ͽ�����"<<endl;
            break;
        } else{
            perror("recvʧ��");
            break;
        }
    }
    close(cliet->cfd);
}

void con(void *arg){
    //4.�������ȴ��ͻ�������
    Sock *sock=(Sock*)arg;
    while (1){
        Cliet *cliet=new Cliet;
        socklen_t caddrlen = sizeof(cliet->addr);
        int cfd = accept(sock->fd, (sockaddr*)&cliet->addr, &caddrlen);
        if (cfd==-1){
            perror("accept����ʧ��");
            break;
        }
        cliet->cfd=cfd;
        taskAdd(sock->threadPool,working,cliet);
    }
    //7.�ر��ļ�������
    close(sock->fd);
}

int main() {
    //1.�����׽���
    int fd= socket(AF_INET,SOCK_STREAM,0);
    if (fd==-1){
        perror("socket����ʧ��");
        return -1;
    }

    //2.��IP
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port= htons(8888);
    addr.sin_addr.s_addr=INADDR_ANY;//�Զ����������������IP
    int ret= bind(fd,(sockaddr*) &addr,sizeof (addr));
    if (ret==-1){
        perror("bind����ʧ��");
        return -1;
    }

    //3.���ü���
    ret= listen(fd,128);
    if (ret==-1){
        perror("listen����ʧ��");
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