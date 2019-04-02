#include "cia_operation.h"
#include "cia_comm.h"

#include <thread>
void porcMsg(Kevent_Node* knode){
    LOG_ACC(INFO,"线程threadid = %d 开始处理消息：%s",std::this_thread::get_id(), knode->buf);
    sleep(5);
    LOG_ACC(INFO,"线程threadid = %d 结束处理消息：%s",std::this_thread::get_id(), knode->buf)

    knode->status = PKG_BD_RECVING;
    memset(knode->buf, 0, strlen(knode->buf));  // 处理完数据要进行清空
}