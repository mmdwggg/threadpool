#include "threadpool.h"
ThreadPool *threadPool_Create(int min,int max,int queueSize){

    //��ʼ���̳߳�
    ThreadPool *threadPool=(ThreadPool*) malloc(sizeof (ThreadPool));
    if(threadPool==NULL){
        printf("�̳߳س�ʼ��ʧ��");
        return NULL;
    }

    //��ʼ���̶߳���
    threadPool->threads=(pthread_t*) malloc(sizeof (pthread_t)*max);
    if(threadPool->threads==NULL){
        free(threadPool);
        printf("�̶߳��д���ʧ��");
        return NULL;
    }

    memset(threadPool->threads,0,sizeof(pthread_t) * max);
    threadPool->busyNum=0;
    threadPool->exitNum=0;
    threadPool->liveNum=min;
    threadPool->maxNum=max;
    threadPool->minNum=min;

    //��ʼ��������������
    if(     pthread_mutex_init(&threadPool->mutexBusy,NULL)!=0||
            pthread_mutex_init(&threadPool->mutexPool,NULL)!=0||
            pthread_cond_init(&threadPool->notEmpty,NULL)!=0||
            pthread_cond_init(&threadPool->notFull,NULL)!=0
            ){
        free(threadPool->threads);
        free(threadPool);
        printf("�߳�����ʼ��ʧ��");
        return NULL;
    }

    //��ʼ���������
    threadPool->taskQ= (Task*)malloc(sizeof (Task)*queueSize);
    if(threadPool->taskQ==NULL){
        free(threadPool);
        free(threadPool->threads);
        printf("�̶߳��д���ʧ��");
        return NULL;
    }
    threadPool->queueCapacity=queueSize;
    threadPool->queueSize=0;
    threadPool->queueFront=0;
    threadPool->queueRear=0;

    threadPool->shutdown=0;

    //�����߳�
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

        //������п�ʱҪ�����߳�
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

        //�������ȡ����
        Task task;
        task.function=threadPool->taskQ[threadPool->queueFront].function;
        task.arg=threadPool->taskQ[threadPool->queueFront].arg;
        threadPool->queueFront=(threadPool->queueFront+1)%threadPool->queueCapacity;
        threadPool->queueSize--;
        pthread_cond_signal(&threadPool->notFull);

        pthread_mutex_unlock(&threadPool->mutexPool);

        printf("�߳�%ld��ʼ����\n",pthread_self());
        pthread_mutex_lock(&threadPool->mutexBusy);
        threadPool->busyNum+=1;
        pthread_mutex_unlock(&threadPool->mutexBusy);

        task.function(task.arg);
        free(task.arg);
        task.arg=NULL;

        printf("�߳�%ld��������\n",pthread_self());
        pthread_mutex_lock(&threadPool->mutexBusy);
        threadPool->busyNum--;
        pthread_mutex_unlock(&threadPool->mutexBusy);
    }
}

void* manager(void* arg){
    ThreadPool *threadPool=(ThreadPool*) arg;

    while (threadPool->shutdown==0){
        sleep(3);

        //��ȡ��ǰ���У��������߳����Լ���������
        pthread_mutex_lock(&threadPool->mutexPool);
        int queueSize=threadPool->queueSize;
        int liveNum=threadPool->liveNum;
        pthread_mutex_unlock(&threadPool->mutexPool);
        pthread_mutex_lock(&threadPool->mutexBusy);
        int busyNum=threadPool->busyNum;
        pthread_mutex_unlock(&threadPool->mutexBusy);

        //����δ�������߳�
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

        //ֹͣ�̹߳���
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
            printf("%ld���߳�ȡ������\n",tid);
            break;
        }
    }
    pthread_exit(NULL);
}

void taskAdd(ThreadPool* threadPool,void(*function)(void*),void* arg){
    pthread_mutex_lock(&threadPool->mutexPool);

    //���������ʱ����
    while (threadPool->queueSize==threadPool->queueCapacity&&threadPool->shutdown==0){
        pthread_cond_wait(&threadPool->notFull,&threadPool->mutexPool);
    }

    if(threadPool->shutdown!=0){
        pthread_mutex_unlock(&threadPool->mutexPool);
        return;
    }

    //�������ȡ����
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
