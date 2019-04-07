#ifndef __HEADERS_CIA_THREAD_POLL_H__
#define __HEADERS_CIA_THREAD_POLL_H__

#include <thread>
#include <vector>
enum thread_type { CREQUEST, CRESPONSE, CNONE };

class CThreadPoll{
public:
    CThreadPoll();
    ~CThreadPoll();

    
    bool init(int request_num, int response_num); // 输入参数是处理request请求的线程池数量，和发送response线程池的数量（一般为1）
    bool create(int num,  enum thread_type type);

    void stopAll();  // 结束所有
    void call_request();    // 召唤线程池干活
    void call_response();    // 召唤线程池干活
private:
    
    // 线程池中的线程结构
    class ThreadItem{
        public:
        bool isrunning;  // 是否运行起来的标记
        std::thread* m_thread;  // 线程指针
        CThreadPoll* _this; // 指向线程池的指针
        enum thread_type type;
        ThreadItem(std::thread* thre, CThreadPoll* cp):isrunning(false), m_thread(thre), _this(cp), type(CNONE){}
        ~ThreadItem(){}  // 注意这里并没有释放线程指针，会统一在CthreadPoll的析构函数中或者释放线程池的成员函数中释放
    };

    static bool shutdown;                          // 线程池结束标记
    std::vector<ThreadItem*> cia_threads;  // 有两种类别：处理request的线程池，处理response的线程池

    // 处理request的相关请求的线程池的相关数据，其实和reponse一样
    // std::mutex cia_mutex_message ; // 线程的互斥锁, message锁是为了锁住数据读取存入，con锁是为了条件变量
    static std::mutex cia_mutes_con; 
    static std::condition_variable cia_con_var;         // 线程池的条件变量
    std::atomic<int> cia_running_thread;         // 运行中的线程数目
    int cia_thread_num;                     // 线程池中的线程数量
    

    // 处理response的相关的区别，虽然使用信号量比较好，但是使用互斥量和条件变量完全可以是实现信号量，且更加安全
    static std::mutex cia_mutex_response_con;
    static std::condition_variable cia_res_var;         // 线程池的条件变量
    std::atomic<int> cia_running_response_thread;         // 运行中的线程数目
    int cia_thread_res_num;              // 线程池中处理response线程的数量，默认应该是1

public:
    static void threads_func(ThreadItem& item);   // 线程初始化函数
    static void threads_request_func(ThreadItem& item);
    static void threads_response_func(ThreadItem& item);
};

#endif