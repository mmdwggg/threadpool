#include "stdio.h"
#include "pthread.h"
#include "string.h"
#include <malloc.h>
#include <unistd.h>
//����ṹ��
typedef struct Task
{
    void (*function)(void* arg);
    void* arg;
}Task;

//�̳߳ؽṹ��
typedef struct ThreadPool{
    Task * taskQ;
    int queueCapacity;//����
    int queueSize;//��ǰ�����������
    int queueFront;//��β������
    int queueRear;//��ͷȡ����

    pthread_t manegerID;//�������߳�
    pthread_t *threads;//���е��߳�
    int maxNum;//����߳���
    int minNum;//��С�߳���
    int busyNum;//���ڹ������߳���
    int liveNum;//�Ѿ�Ͷ�����е��߳���
    int exitNum;//̫����Ҫֹͣ�������߳���

    pthread_mutex_t mutexPool;//�������̳߳�
    pthread_mutex_t mutexBusy;//��ס�����̣߳����ù�����Դ
    pthread_cond_t notFull;//��������Ƿ���
    pthread_cond_t notEmpty;//��������Ƿ��

    int shutdown;//�Ƿ������̳߳أ���1��0
}ThreadPool;


//�����̳߳ز���ʼ��
ThreadPool *threadPool_Create(int max,int min,int queueSize);

//�����ߺ���
void * work(void *arg);

//�����ߺ���
void * manager(void *arg);

//�߳��˳�����
void threadExit(ThreadPool* threadPool);

//�����̳߳�
int threadDestroy(ThreadPool *threadPool);

//���̳߳��������
void taskAdd(ThreadPool* threadPool,void(*function)(void*),void* arg);

//��ȡ�̳߳��й����̵߳�����
int threadBusy(ThreadPool *threadPool);

//��ȡ�̳߳������е��߳�����
int threadLive(ThreadPool *threadPool);