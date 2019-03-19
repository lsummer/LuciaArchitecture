
// 创建守护进程步骤如下
// (1)调用umask(0);
//     umask是个函数，用来限制（屏蔽）一些文件权限的。
// (2)fork()一个子进程(脱离终端)出来,然后父进程退出( 把终端空出来，不让终端卡住)；固定套路
//     fork()的目的是想成功调用setsid()来建立新会话，目的是
//     子进程有单独的sid；而且子进程也成为了一个新进程组的组长进程；同时，子进程不关联任何终端了；
// (3) 将输入、输出文件描述父重定向到/dev/null；用dup2  
#include "cia_setproctitle.h"
#include "signal.h"
#include "cia_log.h"
int cia_daemon(){
    pid_t pid;
    pid = fork();
    for(;;){
        if(pid == 0){
            break;
        }else if(pid == -1){
            LOG_ERR(WARN, "守护进程启动失败，结束程序运行：fork()出现错误");
            return -1;
        }else{
            return 1;
        }
    }

    umask(0);
    if(setsid() == -1){
        LOG_ERR(WARN, "守护进程启动失败，结束程序运行：设置守护进程出现错误");
        return -1;
    }
    int fd = open("/dev/null", O_RDWR);
    if(fd == -1){
        LOG_ERR(WARN, "守护进程启动失败，结束程序运行：文件空洞/dev/null打开错误");
        return -1;
    }
    if(dup2(fd, STDERR_FILENO) == -1){
        LOG_ERR(WARN, "守护进程启动失败，结束程序运行：文件空洞/dev/null赋值STDERR_FILENO错误");
        return -1;
    }
    if(dup2(fd, STDIN_FILENO) == -1){
        LOG_ERR(WARN, "守护进程启动失败，结束程序运行：文件空洞/dev/null赋值STDIN_FILENO错误");
        return -1;
    }
    if(dup2(fd, STDOUT_FILENO) == -1){
        LOG_ERR(WARN, "守护进程启动失败，结束程序运行：文件空洞/dev/null赋值STDOUT_FILENO错误");
        return -1;
    }
    if(close(fd) == -1){
        LOG_ERR(WARN, "守护进程启动失败，结束程序运行：关闭文件空洞/dev/null错误");
        return -1;
    }
    return 0;
}