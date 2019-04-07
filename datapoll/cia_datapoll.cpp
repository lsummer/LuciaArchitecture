#include "cia_datapoll.h"
#include "cia_log.h"
#include "cia_global.h"
// 数据池

DataPoll::DataPoll(){}
DataPoll::~DataPoll(){}
void DataPoll::inMsgQueue(Message* s){
    {
        std::lock_guard<std::mutex> sbguard(cia_mutex_message);
        requestQueue.push_back(s);
    }
    threadpoll.call_request();
    // LOG_ACC(INFO, "收到数据包：%s", s);
}
Message* DataPoll::outMsgQueue(){
    Message* buffer = NULL;
    {
        std::lock_guard<std::mutex> sbguard(cia_mutex_message);
        if(requestQueue.empty()) return NULL;
        buffer = requestQueue.front();
        requestQueue.pop_front();
    }
    // LOG_ACC(INFO, "开始处理数据包：%s, 数据包的长度为：%d", buffer, strlen(buffer));
    return buffer;
}

bool DataPoll::empty(){
    std::lock_guard<std::mutex> sbguard(cia_mutex_message);
    return requestQueue.empty();
}


// 响应消息
void DataPoll::inResQueue(Response* s){
    {
        std::lock_guard<std::mutex> sbguard(cia_mutex_response);
        responseQueue.push_back(s);
    }
    threadpoll.call_response();
}

Response* DataPoll::outResQueue(){
    Response* buffer = NULL;
    {
        std::lock_guard<std::mutex> sbguard(cia_mutex_response);
        if(responseQueue.empty()) return NULL;
        buffer = responseQueue.front();
        responseQueue.pop_front();
    }
    return buffer;
}
bool DataPoll::resEmpty(){
    std::lock_guard<std::mutex> sbguard(cia_mutex_response);
    return responseQueue.empty();
}