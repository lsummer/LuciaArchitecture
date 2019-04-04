#include <thread>
#include <sys/socket.h>
#include "cia_operation.h"
#include "cia_comm.h"
// 要重写
void porcMsg(Message* message){
    LOG_ACC(INFO,"线程threadid = %d 开始处理消息",std::this_thread::get_id());
    LOG_ACC(INFO,"------- 消息开始 ---------");
    // char buf[11] = "Received!";
    // int n = send(knode->fd, buf, strlen(buf), 0);
    LOG_ACC(INFO, "URL: %s", (message->url).c_str());
    LOG_ACC(INFO, "Headers: ");
    for(auto itre = (message->headers).begin(); itre != (message->headers).end(); itre++){
        LOG_ACC(INFO, "%s:  %s", ((*itre)[0]).c_str(), ((*itre)[1]).c_str());
    }


    LOG_ACC(INFO,"------- 消息结束 ---------");
    sleep(5);
    LOG_ACC(INFO,"线程threadid = %d 结束处理消息",std::this_thread::get_id());

    delete message;
    char buf[] = "HTTP/1.1 200 OK\r\n"
         "Date: Tue, 04 Aug 2009 07:59:32 GMT\r\n"
         "Server: Apache\r\n"
         "X-Powered-By: Servlet/2.5 JSP/2.1\r\n"
         "Content-Type: text/xml; charset=utf-8\r\n"
         "Connection: close\r\n"
         "\r\n";
    int n = send(message->fd, buf, strlen(buf), 0);
}