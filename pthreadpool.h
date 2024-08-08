#ifndef PTHREAD_POOL_H
#define PTHREAD_POOL_H
#include <list>
#include <stdexcept> 
#include <exception>
#include "locker.h"
#include <pthread.h>
#include <stdio.h>
template <typename T>
class pthreadpool{
public:
    pthreadpool(int thread_num = 8, int max_request = 10000);
    ~pthreadpool();
    bool append(T* request);
private:
    static void* worker(void* arg);
    void run();

private:
    int m_thread_num;
    int m_max_request;
    pthread_t *m_threads;
    std::list<T*> m_workqueue;
    sem m_queuestat;
    locker m_queuelocker;
    bool m_stop;
};

template <typename T>
pthreadpool<T>::pthreadpool(int thread_num, int max_request)
    : m_thread_num(thread_num), m_max_request(max_request), m_threads(nullptr), m_stop(false)
{
    if (m_thread_num <= 0 || m_max_request <= 0) {
        throw std::invalid_argument("Thread number and max request must be greater than 0");
    }

    m_threads = new pthread_t[m_thread_num];
    if (m_threads == nullptr) {
        throw std::bad_alloc();
    }

    for (int i = 0; i < m_thread_num; ++i) {
        printf("create the %dth thread\n", i);
        if (pthread_create(m_threads + i, NULL, worker, this) != 0) {
            delete[] m_threads;
            throw std::runtime_error("Failed to create thread");
        }
        // Detach the thread after creation
        if (pthread_detach(m_threads[i]) != 0) {
            delete[] m_threads;
            throw std::runtime_error("Failed to detach thread");
        }
    }
}


template <typename T>
pthreadpool<T>::~pthreadpool(){
    delete[] m_threads;
    m_stop = true;
}

template <typename T>
bool pthreadpool<T>::append(T* request){
    m_queuelocker.lock();
    if(m_workqueue.size() > m_max_request){
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
void* pthreadpool<T>::worker(void* arg){
    pthreadpool* pool = (pthreadpool* )arg;
    pool->run();
    return pool;
}

template <typename T>
void pthreadpool<T>::run(){
    while(! m_stop){
        m_queuestat.wait();
        m_queuelocker.lock();
        if(m_workqueue.empty()){
            m_queuelocker.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        
        if(!request){
            continue;
        }
        printf("发送的为%d\n",*request);
        m_queuelocker.unlock();
    }
}

#endif