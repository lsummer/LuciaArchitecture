#ifndef __HEADERS_CIA_SETPROCTITLE_H__
#define __HEADERS_CIA_SETPROCTITLE_H__

#include "header.h"

void cia_init_setprcotitle();   // 环境变量搬家

void cia_setproctitle(const std::string& procname);   // 设置进程的名称

#endif