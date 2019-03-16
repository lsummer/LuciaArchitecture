#ifndef __HEADERS_CIA_GLOBAL_H__
#define __HEADERS_CIA_GLOBAL_H__

#include <string>

//外部全局量声明
extern int g_environment; // 环境变量的长度
extern char **environ;    // 环境变量。  
extern char **g_os_argv; // 命令行参数 

#endif
