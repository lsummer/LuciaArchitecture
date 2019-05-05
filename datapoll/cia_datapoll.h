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
    // 请求消息的队列
    void inMsgQueue(Message* s);
    Message* outMsgQueue();
    bool empty();

    // 响应消息的队列
    void inResQueue(Response* s);
    Response* outResQueue();
    bool resEmpty();

private:
    //  请求消息的队列
    std::list<Message *> requestQueue;
    std::mutex cia_mutex_message;  // 用的互斥量

    // 响应消息的队列
    std::list<Response *> responseQueue;
    std::mutex cia_mutex_response;  
};

#endif 