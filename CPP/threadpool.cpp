#include "threadpool.h"
ThreadPool *threadPool_Create(int min,int max,int queueSize){

    //初始化线程池
    ThreadPool *threadPool=(ThreadPool*) malloc(sizeof (ThreadPool));
    if(threadPool==NULL){
        printf("线程池初始化失败");
        return NULL;
    }

    //初始化线程队列
    threadPool->threads=(pthread_t*) malloc(sizeof (pthread_t)*max);
    if(threadPool->threads==NULL){
        free(threadPool);
        printf("线程队列创建失败");
        return NULL;
    }

    memset(threadPool->threads,0,sizeof(pthread_t) * max);
    threadPool->busyNum=0;
    threadPool->exitNum=0;
    threadPool->liveNum=min;
    threadPool->maxNum=max;
    threadPool->minNum=min;

    //初始化锁和条件变量
    if(     pthread_mutex_init(&threadPool->mutexBusy,NULL)!=0||
            pthread_mutex_init(&threadPool->mutexPool,NULL)!=0||
            pthread_cond_init(&threadPool->notEmpty,NULL)!=0||
            pthread_cond_init(&threadPool->notFull,NULL)!=0
            ){
        free(threadPool->threads);
        free(threadPool);
        printf("线程锁初始化失败");
        return NULL;
    }

    //初始化任务队列
    threadPool->taskQ= (Task*)malloc(sizeof (Task)*queueSize);
    if(threadPool->taskQ==NULL){
        free(threadPool);
        free(threadPool->threads);
        printf("线程队列创建失败");
        return NULL;
    }
    threadPool->queueCapacity=queueSize;
    threadPool->queueSize=0;
    threadPool->queueFront=0;
    threadPool->queueRear=0;

    threadPool->shutdown=0;

    //启动线程
    pthread_create(&threadPool->manegerID,NULL,manager,threadPool);
    for (int i = 0; i < min; ++i) {
        pthread_create(&threadPool->threads[i],NULL,work,threadPool);
    }

    return threadPool;
}

void * work(void *arg){
    ThreadPool *threadPool=(ThreadPool*) arg;

    while (1){
        pthread_mutex_lock(&threadPool->mutexPool);

        //任务队列空时要阻塞线程
        while (threadPool->queueSize==0&&threadPool->shutdown==0){
            pthread_cond_wait(&threadPool->notEmpty,&threadPool->mutexPool);

            if(threadPool->exitNum!=0){
                threadPool->exitNum--;
                threadPool->liveNum--;
                pthread_mutex_unlock(&threadPool->mutexPool);
                threadExit(threadPool);
            }
        }

        if(threadPool->shutdown!=0){
            pthread_mutex_unlock(&threadPool->mutexPool);
            threadExit(threadPool);
        }

        //任务队列取任务
        Task task;
        task.function=threadPool->taskQ[threadPool->queueFront].function;
        task.arg=threadPool->taskQ[threadPool->queueFront].arg;
        threadPool->queueFront=(threadPool->queueFront+1)%threadPool->queueCapacity;
        threadPool->queueSize--;
        pthread_cond_signal(&threadPool->notFull);

        pthread_mutex_unlock(&threadPool->mutexPool);

        printf("线程%ld开始工作\n",pthread_self());
        pthread_mutex_lock(&threadPool->mutexBusy);
        threadPool->busyNum+=1;
        pthread_mutex_unlock(&threadPool->mutexBusy);

        task.function(task.arg);
        free(task.arg);
        task.arg=NULL;

        printf("线程%ld结束工作\n",pthread_self());
        pthread_mutex_lock(&threadPool->mutexBusy);
        threadPool->busyNum--;
        pthread_mutex_unlock(&threadPool->mutexBusy);
    }
}

void* manager(void* arg){
    ThreadPool *threadPool=(ThreadPool*) arg;

    while (threadPool->shutdown==0){
        sleep(3);

        //获取当前运行，工作的线程数以及任务数量
        pthread_mutex_lock(&threadPool->mutexPool);
        int queueSize=threadPool->queueSize;
        int liveNum=threadPool->liveNum;
        pthread_mutex_unlock(&threadPool->mutexPool);
        pthread_mutex_lock(&threadPool->mutexBusy);
        int busyNum=threadPool->busyNum;
        pthread_mutex_unlock(&threadPool->mutexBusy);

        //启动未工作的线程
        if(queueSize>liveNum&&liveNum<threadPool->maxNum){
            pthread_mutex_lock(&threadPool->mutexPool);
            for (int i = 0; i < threadPool->maxNum; ++i) {
                if(threadPool->threads[i]==0){
                    pthread_create(&threadPool->threads[i],NULL,work,threadPool);
                    threadPool->liveNum++;
                }
            }
            pthread_mutex_unlock(&threadPool->mutexPool);
        }

        //停止线程工作
        if(busyNum*2<liveNum&&liveNum>threadPool->minNum){
            pthread_mutex_lock(&threadPool->mutexPool);
            threadPool->exitNum=1;
            pthread_mutex_unlock(&threadPool->mutexPool);
            pthread_cond_signal(&threadPool->notEmpty);
        }
    }
    pthread_exit(&threadPool->manegerID);
}

void threadExit(ThreadPool* threadPool){
    pthread_t tid=pthread_self();
    for (int i = 0; i < threadPool->maxNum; ++i) {
        if(threadPool->threads[i]==tid){
            threadPool->threads[i]=0;
            printf("%ld号线程取消运行\n",tid);
            break;
        }
    }
    pthread_exit(NULL);
}

void taskAdd(ThreadPool* threadPool,void(*function)(void*),void* arg){
    pthread_mutex_lock(&threadPool->mutexPool);

    //任务队列满时阻塞
    while (threadPool->queueSize==threadPool->queueCapacity&&threadPool->shutdown==0){
        pthread_cond_wait(&threadPool->notFull,&threadPool->mutexPool);
    }

    if(threadPool->shutdown!=0){
        pthread_mutex_unlock(&threadPool->mutexPool);
        return;
    }

    //任务队列取任务
    threadPool->taskQ[threadPool->queueRear].function=function;
    threadPool->taskQ[threadPool->queueRear].arg=arg;
    threadPool->queueRear=(threadPool->queueRear+1)%threadPool->queueCapacity;
    threadPool->queueSize++;
    pthread_cond_signal(&threadPool->notEmpty);
    pthread_mutex_unlock(&threadPool->mutexPool);
}

int threadBusy(ThreadPool *threadPool){
    pthread_mutex_lock(&threadPool->mutexBusy);
    int busyNum=threadPool->busyNum;
    pthread_mutex_unlock(&threadPool->mutexBusy);
    return busyNum;
}

int threadLive(ThreadPool *threadPool){
    pthread_mutex_lock(&threadPool->mutexPool);
    int liveNum=threadPool->liveNum;
    pthread_mutex_unlock(&threadPool->mutexPool);
    return liveNum;
}

int threadDestroy(ThreadPool *threadPool) {
    if(threadPool==NULL){
        return -1;
    }

    threadPool->shutdown=1;
    pthread_join(threadPool->manegerID,NULL);
    pthread_mutex_lock(&threadPool->mutexPool);
    for (int i = 0; i < threadPool->liveNum; ++i) {
        pthread_cond_signal(&threadPool->notEmpty);
    }
    pthread_mutex_unlock(&threadPool->mutexPool);

    if(threadPool->taskQ) free(threadPool->taskQ);
    if(threadPool->threads) free(threadPool->threads);
    pthread_mutex_destroy(&threadPool->mutexPool);
    pthread_mutex_destroy(&threadPool->mutexBusy);
    pthread_cond_destroy(&threadPool->notEmpty);
    pthread_cond_destroy(&threadPool->notFull);
    free(threadPool);
    threadPool=NULL;
}
