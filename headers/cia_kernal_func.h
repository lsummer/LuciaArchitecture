#ifndef __HEADERS_KERNAL_CIA_FUNC_H__
#define __HEADERS_KERNAL_CIA_FUNC_H__
#include "cia_log.h"

// ---------- 日志文件初始化 -------------------
void cia_logs_init();
// ---------- 信号初始化 ----------------------
int cia_init_signals(); 
// ---------- 开始执行创建子进程 ---------------
void cia_master_process_cycle();
// ---------- 设置守护进程 --------------------
int cia_daemon();

#endif
