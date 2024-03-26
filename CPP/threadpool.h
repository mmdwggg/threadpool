#include "stdio.h"
#include "pthread.h"
#include "string.h"
#include <malloc.h>
#include <unistd.h>
//任务结构体
typedef struct Task
{
    void (*function)(void* arg);
    void* arg;
}Task;

//线程池结构体
typedef struct ThreadPool{
    Task * taskQ;
    int queueCapacity;//容量
    int queueSize;//当前队列任务个数
    int queueFront;//队尾加任务
    int queueRear;//队头取任务

    pthread_t manegerID;//管理者线程
    pthread_t *threads;//运行的线程
    int maxNum;//最大线程数
    int minNum;//最小线程数
    int busyNum;//正在工作的线程数
    int liveNum;//已经投入运行的线程数
    int exitNum;//太多需要停止工作的线程数

    pthread_mutex_t mutexPool;//锁整个线程池
    pthread_mutex_t mutexBusy;//锁住工作线程，常用共享资源
    pthread_cond_t notFull;//任务队列是否满
    pthread_cond_t notEmpty;//任务队列是否空

    int shutdown;//是否销毁线程池，是1否0
}ThreadPool;


//创建线程池并初始化
ThreadPool *threadPool_Create(int max,int min,int queueSize);

//工作者函数
void * work(void *arg);

//管理者函数
void * manager(void *arg);

//线程退出函数
void threadExit(ThreadPool* threadPool);

//销毁线程池
int threadDestroy(ThreadPool *threadPool);

//给线程池添加任务
void taskAdd(ThreadPool* threadPool,void(*function)(void*),void* arg);

//获取线程池中工作线程的数量
int threadBusy(ThreadPool *threadPool);

//获取线程池中运行的线程数量
int threadLive(ThreadPool *threadPool);