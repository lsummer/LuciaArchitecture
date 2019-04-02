#ifndef __HEADERS_CIA_FUNC_H__
#define __HEADERS_CIA_FUNC_H__
/*
    对字符串操作的一些函数会在这里
*/
// #include "header.h"

#define LOGS_SINGLE_MAX_CHARACTERS 500

// ----------- 字符串截取操作 -------------------
// 实现：../app/cia_func.cpp/

void Ltrim(std::string& s); // 去掉左侧的空格

void Ltrim(std::string& s); // 去掉右侧的空格

void LRtrim(std::string& s); // 去掉左右两侧的空格

void EraseAnnoation(std::string& s);  // 去掉‘#’代表的注释

// ------------ 可变参数拼接操作 ------------------
int args_printf(std::string& result, const char* fmt, va_list args);



#endif