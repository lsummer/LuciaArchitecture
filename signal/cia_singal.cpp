
#include "cia_signal.h"
#include "cia_global.h"

#include <sys/wait.h>
static std::vector<lucia_signal_t>  signals = {
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
            LOG_ERR(INFO, "信号%s设置成功", &sina->name);
        }
    }
    return 0;
}


void cia_singal_handel(int signo, siginfo_t *siginfo, void* ucontext){
    // 父进程和子进程对信号的处理不一样
    if(process_type == CIA_PROCESS_MASTER){  // 父进程
        switch (signo)
        {
            case SIGCHLD: 
            // 当子进程结束的时候会向父进程发送SIGCHLD信号
            // 父进程需要捕获并调用waitpid()来避免子进程成为僵尸进程,其实子进程也需要处理，但是父进程可能需要处理一下
                /* code */

                break;
        
            default:
                break;
        }
    }else{ // 子进程

    }


    switch (signo)
    {
        case SIGCHLD:
            /* code */
            cia_process_sigchld();
            break;
    
        default:
            break;
    }
    

    LOG_ERR(INFO, "接收到信号 %d, 信号已经处理", signo);
    // std::cout << "信号处理程序退出" << std::endl;
}

void cia_process_sigchld(){
    int staloc;
    int err; 
    int one = 0; // 作为标记，如果不为0表示被处理过
    pid_t pid;
    for(;;){
        pid = waitpid(-1, &staloc, WNOHANG);
        if(pid == 0){ //表示子进程还没有结束
            return;
        }else if(pid == -1){
            // 抄自nginx
            //这里处理代码抄自官方nginx，主要目的是打印一些日志。考虑到这些代码也许比较成熟，所以，就基本保持原样照抄吧；
            err = errno;
            if(err == EINTR){          //调用被某个信号中断
                continue;
            }

            if(err == ECHILD  && one){  //没有子进程
                return;
            }

            if (err == ECHILD)         //没有子进程
            {
                LOG_ERR(WARN, "waitpid() failed and err = ECHILD!");
                return;
            }

            LOG_ERR(WARN, "waitpid() failed and err = %d!", err);
            return;
        } 
        one = 1; // 表示信号处理过一次了
        /**
         * 子进程的结束状态返回后存于status，底下有几个宏可判别结束情况
         * WIFEXITED(status)如果子进程正常结束则为非0值。
         * WEXITSTATUS(status)取得子进程exit()返回的结束代码，一般会先用WIFEXITED 来判断是否正常结束才能使用此宏。
         * WIFSIGNALED(status)如果子进程是因为信号而结束则此宏值为真
         * WTERMSIG(status)取得子进程因信号而中止的信号代码，一般会先用WIFSIGNALED 来判断后才使用此宏。
         * WIFSTOPPED(status)如果子进程处于暂停执行情况则此宏值为真。一般只有使用WUNTRACED 时才会有此情况。
         * WSTOPSIG(status)取得引发子进程暂停的信号代码.
         * */

        if(WIFEXITED(staloc)){
            LOG_ERR(INFO, "进程pid = %d 安全退出！", pid);
        }else{
            LOG_ERR(WARN, "进程pid = %d 异常退出, 退出代码exitcode = %d", pid, WEXITSTATUS(staloc) );
            if(WIFSIGNALED(staloc)){
                LOG_ERR(WARN, "进程pid = %d 异常退出，使得该进程退出的信号代码sig_id = %d", pid, WTERMSIG(staloc));
            }
        }
        return;
    }
    
}