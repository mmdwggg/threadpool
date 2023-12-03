#include <iostream>
#include "thread"
#include "mutex"
#include "string"
#include "queue"
#include "condition_variable"
#include "future"
using namespace std;

queue<int> queue1;
condition_variable cond;
mutex m;

void producer(){
    for (int i = 0; i < 10; ++i) {
        unique_lock<mutex>lk (m);
        queue1.push(i);
        cout<<"生产者生产了"<<i<<endl;
        lk.unlock();
        cond.notify_one();
        this_thread::sleep_for(chrono::seconds (1));
    }
}

void cosumer(){
    for (int i = 0; i < 10; ++i) {
        unique_lock<mutex>lk (m);
        cond.wait(lk,[] () {return !queue1.empty();});
        int value=queue1.front();
        queue1.pop();
        cout<<"消费者消费了"<<value<<endl;
        lk.unlock();
    }
}

int func(){
    int i=0;
    for (i = 0; i < 1000; ++i) {
        i++;
    }
    return i;
}

void yibu(){
    future<int>future_result= async(launch::async,func);
    cout<<func()<<endl;
    cout<<future_result.get()<<endl;
}
int main() {
    yibu();
    return 0;
}
