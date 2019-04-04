#ifndef __HEADERS_CIA_DATA_POLL_H__
#define __HEADERS_CIA_DATA_POLL_H__

#include <vector>
#include <string>
#include <list>
#include <mutex>
#include "cia_socket.h"
// 数据池
class DataPoll{
public:
    DataPoll();
    ~DataPoll();
    void inMsgQueue(Message* s);
    Message* outMsgQueue();

    bool empty();
private:
    std::list<Message *> msgQueue;
    std::mutex cia_mutex_message;
};

#endif 