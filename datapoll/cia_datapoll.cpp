#include "cia_datapoll.h"
#include "cia_log.h"
// 数据池

DataPoll::DataPoll(){}
DataPoll::~DataPoll(){}
void DataPoll::inMsgQueue(Kevent_Node* s){
    {
        std::lock_guard<std::mutex> sbguard(cia_mutex_message);
        msgQueue.push_back(s);
    }
    // LOG_ACC(INFO, "收到数据包：%s", s);
}
Kevent_Node* DataPoll::outMsgQueue(){
    Kevent_Node* buffer = NULL;
    {
        std::lock_guard<std::mutex> sbguard(cia_mutex_message);
        if(msgQueue.empty()) return NULL;
        buffer = msgQueue.front();
        msgQueue.pop_front();
    }
    // LOG_ACC(INFO, "开始处理数据包：%s, 数据包的长度为：%d", buffer, strlen(buffer));
    return buffer;
}

bool DataPoll::empty(){
    std::lock_guard<std::mutex> sbguard(cia_mutex_message);
    return msgQueue.empty();
}


