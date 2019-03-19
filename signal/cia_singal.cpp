
#include "header.h"
#include "cia_log.h"
void cia_singal_handel(int signo, siginfo_t *siginfo, void* ucontext){
    std::cout << "来信号了" << std::endl;
    
    std::cout << "信号处理程序退出" << std::endl;
}

typedef struct{
    int signo;  // 信号编号
    std::string name; // 信号名字
    void (*handle)(int signo, siginfo_t* siginfo, void* handel);  // 信号执行的操作
}lucia_signal_t;

std::vector<lucia_signal_t>  signals = {
    // signo      signame             handler
    { SIGHUP,    "SIGHUP",           cia_singal_handel },        //终端断开信号，对于守护进程常用于reload重载配置文件通知--标识1
    { SIGINT,    "SIGINT",           cia_singal_handel },        //标识2   
	{ SIGTERM,   "SIGTERM",          cia_singal_handel },        //标识15
    { SIGCHLD,   "SIGCHLD",          cia_singal_handel },        //子进程退出时，父进程会收到这个信号--标识17
    { SIGQUIT,   "SIGQUIT",          cia_singal_handel },        //标识3
    { SIGIO,     "SIGIO",            cia_singal_handel },        //指示一个异步I/O事件【通用异步I/O信号】
    { SIGSYS,    "SIGSYS, SIG_IGN",  NULL               },        //我们想忽略这个信号，SIGSYS表示收到了一个无效系统调用，如果我们不忽略，进程会被操作系统杀死，--标识31
                                                                  //所以我们把handler设置为NULL，代表 我要求忽略这个信号，请求操作系统不要执行缺省的该信号处理动作（杀掉我）
    //...日后根据需要再继续增加
    // { 0,         NULL,               NULL               }         //信号对应的数字至少是1，所以可以用0作为一个特殊标记
};

int cia_init_signals(){
    struct sigaction sa;

    for(auto sina = signals.begin(); sina != signals.end(); sina++){
        // std::cout << sina->signo << sina->name << std::endl;
        memset(&sa, 0, sizeof(sa));
        sigemptyset(&sa.sa_mask);
        if(sina->handle  != NULL){
            sa.sa_sigaction = sina->handle;
            sa.sa_flags  = SA_SIGINFO;
        }else{
            sa.sa_handler = SIG_IGN;
        }
        if(sigaction(sina->signo, &sa, NULL) == -1){
            LOG_ERR(ERROR, "设置监听信号出现问题，返回");
            return -1;
        }else{
            LOG_ERR(INFO, "信号 %s 设置成功", &sina->name);
        }
    }
    return 0;
}