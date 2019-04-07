#include "cia_threadpoo.h"
#include "cia_log.h"
// #include "cia_operation.h"
#include "cia_global.h"
std::mutex CThreadPoll::cia_mutes_con;
std::condition_variable CThreadPoll::cia_con_var;         // 线程池的条件变量

std::mutex CThreadPoll::cia_mutex_response_con;
std::condition_variable CThreadPoll::cia_res_var;         // 线程池的条件变量

bool CThreadPoll::shutdown = false;                          // 线程池结束标记


CThreadPoll::CThreadPoll(){
    cia_running_thread = 0;         // 运行中的线程数目
}
CThreadPoll::~CThreadPoll(){
    // 统一放在stopAll()里面去执行
    // shutdown = true;
    // notify_all();
    // 删除cia_threads
    stopAll();
}
// 输入参数是处理request请求的线程池数量，和发送response线程池的数量（一般为1）
bool CThreadPoll::init(int request_num, int response_num){
    return create(request_num, CREQUEST) && create(request_num, CRESPONSE);
}

bool CThreadPoll::create(int num, enum thread_type type){
    cia_thread_num  = num;
    for(int i=0; i<cia_thread_num; i++){
        std::thread* t = NULL;
        ThreadItem* item = new ThreadItem(t, this);
        if(item == NULL){
            LOG_ERR(ERROR, "创建线程失败");
            return false;
        }
        
        item->type = type;
        t = new std::thread(threads_func, std::ref(*item));

        if(t == NULL){
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
    LOG_ERR(INFO, "子进程 pid=%d 的 %d 线程池启动成功", getpid(), type);
    return true;
}

void CThreadPoll::stopAll(){
    if(shutdown == true){
        return;  // 表示执行过了
    }
    shutdown = true;
    LOG_ERR(INFO, "子进程 pid=%d 线程池准备关闭", getpid());

    cia_con_var.notify_all(); // 唤醒所有等待接受请求的线程
    cia_res_var.notify_all(); // 唤醒所有等待发送请求的线程

    // 屏障 join
    auto itre = cia_threads.begin();
    for(; itre != cia_threads.end(); itre++){
        if((*itre)->m_thread == NULL){
            LOG_ERR(ERROR, "进程 pid=%d 的线程池的每项元素中的线程指针出现错误", getpid());
        }else{
            (*itre)->m_thread->join();
        }
    }

    // 到这里一定所有线程都执行完了
    // 删除cia_threads
    itre = cia_threads.begin();
    for(; itre != cia_threads.end(); itre++){
        delete (*itre);
        (*itre) = NULL;
    }
    cia_threads.clear();

    LOG_ERR(INFO, "子进程 pid=%d 线程池已全部关闭", getpid());
}

void CThreadPoll::call_request(){
    // 获得一个线程去执行
    cia_con_var.notify_one();
    LOG_ERR(INFO, "进程pid=%d的线程运行数量为%d", getpid(),cia_running_thread.load());
    if(cia_running_thread >= cia_thread_num * 0.9){
        LOG_ERR(WARN, "进程线程测试目前已经快满了");
    }
}

void CThreadPoll::call_response(){
    // 获得一个线程去执行
    cia_res_var.notify_one();
}

void CThreadPoll::threads_func(ThreadItem& item){
    switch (item.type)
    {
        case CREQUEST:
            /* code */
            threads_request_func(item); 
            break;
        case CRESPONSE:
            /* code */
            threads_response_func(item);
            break;
        default:
            break;
    }
}


void CThreadPoll::threads_request_func(ThreadItem& item){
    // LOG_ERR(INFO, "线程池开始执行thread_pid = %d", std::this_thread::get_id()); 
    for(;;){
        while(shutdown == false && datapoll.empty()){
            if(item.isrunning == false){
                item.isrunning = true;
            }

            std::unique_lock<std::mutex> ulock(cia_mutes_con);
            cia_con_var.wait(ulock);
        }
        //是否执行到这里
        // LOG_ERR(INFO, "线程向下执行到这里");
        // 线程向下执行才是王道

        if(shutdown == true){
            LOG_ERR(INFO, "线程结束运行");
            break; // 表示结束运行
        }
        // 走到这里是真正开始执行
        item._this->cia_running_thread++;
        
        Message* request = datapoll.outMsgQueue(); // 成功读入数据，返回指针，否则返回NULL；
        if(request != NULL){
            socket_ctl.porcRequest(request);
        }
        
        item._this->cia_running_thread--;
    }
}

void CThreadPoll::threads_response_func(ThreadItem& item){
    // LOG_ERR(INFO, "线程池开始执行thread_pid = %d", std::this_thread::get_id()); 
    for(;;){
        while(shutdown == false && datapoll.resEmpty()){
            if(item.isrunning == false){
                item.isrunning = true;
            }

            std::unique_lock<std::mutex> ulock(cia_mutex_response_con);
            cia_res_var.wait(ulock);
        }
        //是否执行到这里
        // LOG_ERR(INFO, "线程向下执行到这里");
        // 线程向下执行才是王道

        if(shutdown == true){
            LOG_ERR(INFO, "线程结束运行");
            break; // 表示结束运行
        }
        // 走到这里是真正开始执行
        item._this->cia_running_response_thread++;
        
        Response* response = datapoll.outResQueue(); // 成功读入数据，返回指针，否则返回NULL；
        if(response != NULL){
            // sendMessage(Response*);
            socket_ctl.sendResponse(response);
        }
        
        item._this->cia_running_response_thread--;
    }
}
