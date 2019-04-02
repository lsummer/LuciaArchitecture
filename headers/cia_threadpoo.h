#ifndef __HEADERS_CIA_THREAD_POLL_H__
#define __HEADERS_CIA_THREAD_POLL_H__

#include <thread>
#include <vector>
class CThreadPoll{
public:
    CThreadPoll();
    ~CThreadPoll();

    bool create(int num);

    void stopAll();  // 结束所有
    void call();    // 召唤线程池干活

private:
    // 线程池中的线程结构
    class ThreadItem{
        public:
        bool isrunning;  // 是否运行起来的标记
        std::thread* m_thread;  // 线程指针
        CThreadPoll* _this; // 指向线程池的指针
    
        ThreadItem(std::thread* thre, CThreadPoll* cp):isrunning(false), m_thread(thre), _this(cp){}
        ~ThreadItem(){}  // 注意这里并没有释放线程指针，会统一在CthreadPoll的析构函数中或者释放线程池的成员函数中释放
    };

    // std::mutex cia_mutex_message ; // 线程的互斥锁, message锁是为了锁住数据读取存入，con锁是为了条件变量
    static std::mutex cia_mutes_con;
    static std::condition_variable cia_con_var;         // 线程池的条件变量
    static bool shutdown;                          // 线程池结束标记

    std::atomic<int> cia_running_thread;         // 运行中的线程数目

    int cia_thread_num;                     // 线程池中的线程数量
    std::vector<ThreadItem*> cia_threads;  // 线程池

public:
    static void thread_func(ThreadItem& item);   // 线程初始化函数
};

#endif