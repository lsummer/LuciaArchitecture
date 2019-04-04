#ifndef __HEADERS_CIA_OPERATION_H__
#define __HEADERS_CIA_OPERATION_H__
// ------------ 业务相关的处理函数 --------------------------
#include "cia_log.h"
#include "cia_socket.h"

// 处理消息
void porcMsg(Message* knode);

void httpHeaderProc();
#endif //__HEADERS_CIA_OPERATION_H__