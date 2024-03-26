#include "threadpool.h"
void taskFunction(void *arg){
    int num=*(int*)arg;
    printf("num:%d  ID:%ld\n",num,pthread_self());
    usleep(1000);
}
int main() {
    ThreadPool *threadPool= threadPool_Create(5,10,100);
    for (int i = 0; i < 100; ++i) {
        int *num=(int*) malloc(sizeof (int));
        *num=i+100;
        taskAdd(threadPool,taskFunction,num);
    }
    sleep(3);
    threadDestroy(threadPool);
    return 0;
}
