// nginx中创建worker子进程
//官方nginx ,一个master进程，创建了多个worker子进程；
// master process ./nginx 
// worker process
//(i)ngx_master_process_cycle()        //创建子进程等一系列动作
//(i)    ngx_setproctitle()            //设置进程标题    
//(i)    ngx_start_worker_processes()  //创建worker子进程   
//(i)        for (i = 0; i < threadnums; i++)   //master进程在走这个循环，来创建若干个子进程
//(i)            ngx_spawn_process(i,"worker process");
//(i)                pid = fork(); //分叉，从原来的一个master进程（一个叉），分成两个叉（原有的master进程，以及一个新fork()出来的worker进程
//(i)                //只有子进程这个分叉才会执行ngx_worker_process_cycle()
//(i)                ngx_worker_process_cycle(inum,pprocname);  //子进程分叉
//(i)                    ngx_worker_process_init();
//(i)                        sigemptyset(&set);  
//(i)                        sigprocmask(SIG_SETMASK, &set, NULL); //允许接收所有信号
//(i)                        ngx_setproctitle(pprocname);          //重新为子进程设置标题为worker process
//(i)                        for ( ;; ) {}. ....                   //子进程开始在这里不断的死循环
//(i)    sigemptyset(&set); 
//(i)    for ( ;; ) {}.                //父进程[master进程]会一直在这里循环
#include "cia_setproctitle.h"
#include "signal.h"
#include "cia_log.h"
#include "cia_kernal_func.h"
#include "cia_global.h"
// 清楚子进程的信号监听
void clearsigmask(){
    sigset_t set;
    sigemptyset(&set);
    if(sigprocmask(SIG_SETMASK, &set, NULL) == -1){
        LOG_ERR(WARN, "重设子进程 pid=%d 的信号屏蔽字失败", getpid());
    }
}

// 参数i表示是第几个worker进程
void worker_process_init(int i){

    process_type = i; // 通过process_type表示这个进程是父进程还是子进程

    LOG_ERR(WARN, "子进程 pid=%d 创建成功", getpid());

    if(socket_ctl.epoll_init()== false){  // 设置epoll
        LOG_ERR(WARN, "子进程 pid=%d 设置epoll_init()失败，退出", getpid());
        return;
    }
    // 清空信号
    clearsigmask();
    // 子进程初始化
    cia_setproctitle("lucia: worker process");  // 修改进程名称为lucia: worker process

    for(;;){
        
        cia_process_events_and_timers();
        // sleep(3);
        // 
    }
}

void cia_create_worker_process(int workers){
    pid_t ppid = getpid();
    for(int i=0; i<workers; i++){
        /// 在这循环中创建子进程
        pid_t pid;
        pid = fork();
        if(pid == -1){
            LOG_ERR(WARN, "master进程创建子进程失败");
        }else if(pid == 0){ //创建的子进程
            
            worker_process_init(i+1);
            
        }
    }
}

void  cia_master_process_cycle(){
    sigset_t sig_new;
   
    sigfillset(&sig_new);
    // 设置信号屏蔽字
    if(sigprocmask(SIG_BLOCK, &sig_new, NULL) == -1){
        LOG_ERR(WARN, "恢复父进程 pid=%d 的信号屏蔽字失败", getpid());
    }
    process_type = 0;
    int worker_process_number = CConfig::getInstance()->GetIntDefault("workers");

    // 创建子进程
    cia_create_worker_process(worker_process_number);

    // 修改主进程名
    cia_setproctitle("lucia: master process");  // 修改进程名称为lucia: master process

    sigemptyset(&sig_new);

    for(;;){
        // 阻塞在这里等待一个信号,等待一个信号，此时进程是挂起的，不会占用cpu时间，只有收到信号才会被唤醒
        // 原子操作
        // sigsuspend()
        //a)根据给定的参数设置新的mask 并 阻塞当前进程【因为是个空集，所以不阻塞任何信号】
        //b)此时，一旦收到信号，便恢复原先的信号屏蔽【我们原来的mask在上边设置的，阻塞了多达10个信号，从而保证我下边的执行流程不会再次被其他信号截断】
        //c)调用该信号对应的信号处理函数
        //d)信号处理函数返回后，sigsuspend返回，使程序流程继续往下走
        sigsuspend(&sig_new); 
        LOG_ERR(INFO, "父进程执行一次信号。。。");
    }
}

