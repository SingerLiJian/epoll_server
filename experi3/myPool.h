#ifndef MYPOOL_H
#define MYPOOL_H

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include "Thread.h"
#include "cond.h"
#include "mutex.h"

using namespace std;

class ThreadPool{
private:
    struct NWORKER{
        Thread worker;
        bool terminate;
        int isWorking;
        ThreadPool *pool;
    } *m_workers;

    struct NJOB{
        void (*func)(void *arg);     //任务函数
        void *user_data;
    };
public:
    //线程池初始化
    //numWorkers:线程数量
    ThreadPool(int numWorkers, int max_jobs);
    //销毁线程池
    ~ThreadPool();
    //面向用户的添加任务
    int pushJob(void (*func)(void *data), void *arg, int len);
    int getLengthofJob();

private:
    //向线程池中添加任务
    bool _addJob(NJOB* job);
    //回调函数
    static void* _run(void *arg);
    void _threadLoop(void *arg);

private:
    std::list<NJOB*> m_jobs_list;
    int m_max_jobs;							//任务队列中的最大任务数
    int m_sum_thread;						//worker总数
    int m_free_thread;						//空闲worker数
    Cond* m_jobs_cond;           //线程条件等待
    Mutex* m_jobs_mutex;         //为任务加锁防止一个任务被两个线程执行等其他情况
};
//获取任务队列长度
int ThreadPool::getLengthofJob()
{
    return m_jobs_list.size();
}


//run为static函数
void* ThreadPool::_run(void *arg) {
    NWORKER *worker = (NWORKER *)arg;
    worker->pool->_threadLoop(arg);
}
//threadLoop为普通成员函数
void ThreadPool::_threadLoop(void *arg) {
    NWORKER *worker = (NWORKER*)arg;
    while (1){
        //线程只有两个状态：执行\等待
        //查看任务队列前先获取锁
        m_jobs_mutex->Lock();
        //当前没有任务
        while (m_jobs_list.size() == 0) {
        	//检查worker是否需要结束生命
            if (worker->terminate) break;
            //条件等待直到被唤醒
            m_jobs_cond->Wait(m_jobs_mutex);
        }
        //检查worker是否需要结束生命
        if (worker->terminate){
            m_jobs_mutex->Unlock();
            break;
        }
        //获取到job后将该job从任务队列移出，免得其他worker过来重复做这个任务
        struct NJOB *job = m_jobs_list.front();
        m_jobs_list.pop_front();
		//对任务队列的操作结束，释放锁
        m_jobs_mutex->Unlock();

        m_free_thread--;
        worker->isWorking = true;
        //执行job中的func
        job->func(job->user_data);
        worker->isWorking = false;

        free(job->user_data);
        free(job);
    }

}

bool ThreadPool::_addJob(NJOB *job) {
	//尝试获取锁
    m_jobs_mutex->Lock();
    //判断队列是否超过任务数量上限
    if (m_jobs_list.size() >= m_max_jobs){
        m_jobs_mutex->Unlock();
        return false;
    }
    //向任务队列添加job
    m_jobs_list.push_back(job);
    //唤醒休眠的线程
    m_jobs_cond->Signal();
    //释放锁
    m_jobs_mutex->Unlock();
	return true;
}

//面向用户的添加任务
int ThreadPool::pushJob(void (*func)(void *), void *arg, int len) {
    struct NJOB *job = (struct NJOB*)malloc(sizeof(struct NJOB));
    if (job == NULL){
        perror("malloc");
        return -2;
    }

    memset(job, 0, sizeof(struct NJOB));

    job->user_data = malloc(len);
    memcpy(job->user_data, arg, len);
    job->func = func;

    _addJob(job);

    return 1;
}

ThreadPool::ThreadPool(int numWorkers, int max_jobs = 10) : m_sum_thread(numWorkers), m_free_thread(numWorkers), m_max_jobs(max_jobs){   //numWorkers:线程数量
    if (numWorkers < 1 || max_jobs < 1){
        perror("workers num error");
    }
    //初始化jobs_cond
    m_jobs_cond = new Cond();

    //初始化jobs_mutex
    m_jobs_mutex = new Mutex();

    //初始化workers
    m_workers = new NWORKER[numWorkers];
    if (!m_workers){
        perror("create workers failed!\n");
    }
	//初始化每个worker
    for (int i = 0; i < numWorkers; ++i){
        m_workers[i].pool = this;
        int ret = m_workers[i].worker.run(_run, &m_workers[i]);
        if (ret){
            delete[] m_workers;
            perror("create worker fail\n");
        }
        if (m_workers[i].worker.ForDeath()){
            delete[] m_workers;
            perror("detach worder fail\n");
        }
        m_workers[i].terminate = 0;
    }
}

ThreadPool::~ThreadPool(){
	//terminate值置1
    for (int i = 0; i < m_sum_thread; i++){
        m_workers[i].terminate = 1;
    }
    //广播唤醒所有线程
    m_jobs_mutex->Lock();
    m_jobs_cond->Broadcast();
    m_jobs_mutex->Unlock();
    delete[] m_workers;
}

#endif