#ifndef __HEADERS_CIA_FUNC_H__
#define __HEADERS_CIA_FUNC_H__

#include "header.h"

// ----------- 字符串截取操作 -------------------
// 实现：../app/cia_func.cpp/

void Ltrim(std::string& s); // 去掉左侧的空格

void Ltrim(std::string& s); // 去掉右侧的空格

void LRtrim(std::string& s); // 去掉左右两侧的空格

void EraseAnnoation(std::string& s);  // 去掉‘#’代表的注释

// ---------------------------------------------

#endif