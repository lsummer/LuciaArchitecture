#include "cia_threadpoo.h"
#include "cia_log.h"

std::mutex CThreadPoll::cia_mutes_con;
std::condition_variable CThreadPoll::cia_con_var;         // 线程池的条件变量
bool CThreadPoll::shutdown = false;                          // 线程池结束标记


CThreadPoll::CThreadPoll(){
    cia_running_thread = 0;         // 运行中的线程数目
    shutdown = false;                          // 线程池结束标记
}
CThreadPoll::~CThreadPoll(){
    // shutdown = true;
    // notify_all();
    // 删除cia_threads
}

bool CThreadPoll::create(int num){
    cia_thread_num  = num;
    for(int i=0; i<cia_thread_num; i++){
        std::thread* t = NULL;
        ThreadItem* item = new ThreadItem(t, this);
        t = new std::thread(thread_func, std::ref(*item));

        if(t == NULL || item == NULL){
            LOG_ERR(ERROR, "创建线程失败");
            return false;
        }else{
            item->m_thread = t;
            cia_threads.push_back(item);
        }
    }
    
    auto itre = cia_threads.begin();
    for(; itre != cia_threads.end(); itre++){
        while((*itre)->isrunning == false){
            usleep(100 * 1000);  // 100ms
        }
    }
    LOG_ERR(INFO, "线程启动成功");
    return true;
}

void CThreadPoll::stopAll(){}

void CThreadPoll::call(){}

void CThreadPoll::thread_func(ThreadItem& item){
    LOG_ERR(INFO, "线程池开始执行thread_pid = %d", std::this_thread::get_id());
    for(;;){
        while(shutdown == false){
            if(item.isrunning == false){
                item.isrunning = true;
            }

            std::unique_lock<std::mutex> ulock(cia_mutes_con);
            cia_con_var.wait(ulock);
        }
    }
}