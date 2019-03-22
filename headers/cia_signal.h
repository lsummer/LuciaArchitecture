#ifndef __HEADERS_CIA_SIGNAL_H__
#define __HEADERS_CIA_SIGNAL_H__

#include "header.h"
#include "cia_log.h"

#define CIA_PROCESS_MASTER 0

typedef struct{
    int signo;  // 信号编号
    std::string name; // 信号名字
    void (*handle)(int signo, siginfo_t* siginfo, void* handel);  // 信号执行的操作
}lucia_signal_t;

void cia_process_sigchld();

void cia_singal_handel(int signo, siginfo_t *siginfo, void* ucontext);


int cia_init_signals();

#endif