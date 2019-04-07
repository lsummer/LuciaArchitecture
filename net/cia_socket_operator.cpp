
#include <thread>
#include <sys/socket.h>
#include <algorithm>
#include <iostream>
#include "cia_socket.h"
#include "cia_comm.h"
#include "cia_global.h"

int CSocket::sendProc(Response* res){
    int n;
    for(;;){
        n = send(res->fd, res->begin, res->left_len, 0);
        
        if(n > 0){
            return n;
        }

        if(n == 0){  // 表示客户端主动断开连接
            return 0;
        }

        int err = errno;

        if(err == EAGAIN || err == EWOULDBLOCK){
            return -1;
        }
        if(err == EINTR){
            ;
        }else{
            return -2;
        }
    }
}

// 和recv是对应着的
// 返回值 >0表示发送对应大小的数据， 0表示客户端断开， -1 表示缓冲区满， -2表示出错;当收到返回值>0和-1时需要考虑加入到epoll写中，此外-2表示出错，0表示断开连接，析构；
void CSocket::sendResponse(Response* res){
    int n = sendProc(res);
    LOG_ACC(INFO, "------发送数据-------");
    LOG_ACC(INFO, "%s", res->begin);
    // char* msg;  // 指向response，可能需要构造一下response的结构，之后再议
    // char* begin;
    // size_t left_len;
    // std::atomic<int> send_count;
    // int fd; // 发送到文件描述符fd处去

    if(n > 0){  
        if(n >= res->left_len ){ // 发送成功
            if(res->send_count > 0){     // 表示加入到epoll中去了，要删掉
                // delete from epoll;
                cia_del_epoll(res->fd, false);
            }
            LOG_ACC(INFO, "------发送完成------");
            delete res;
        }else{
            res->left_len -= n;
            res->begin += n;
            if(res->send_count == 0){
                LOG_ACC(INFO, "----尚未发送完整， 放到epoll中去-----");
                // 加入到epoll中去
                // add epolll
                FD_PORT* fd_port = new FD_PORT(res->fd, false, 0);
                if(cia_add_epoll(fd_port, fd_port->wr_flag?1:0, fd_port->wr_flag?0:1,  &CSocket::cia_wait_responese_handler, res) == false){
                    delete fd_port;
                    LOG_ACC(ERROR, "CSocket::sendResponse() 中向epoll中添加节点时出错[n>0]");
                    return;
                } 
                fd_ports.push_back(fd_port);
            }
            ++res->send_count;
        }

    }else if(n == -1){ // EAGAIN || EWOULDBLOCK,需要加入到epoll中去
        if(res->send_count == 0){
            LOG_ACC(INFO, "----尚未发送完整， 放到epoll中去-----");
            // 加入到epoll中去
            // add epoll
            FD_PORT* fd_port = new FD_PORT(res->fd, false, 0);
            if(cia_add_epoll(fd_port, fd_port->wr_flag?1:0, fd_port->wr_flag?0:1,  &CSocket::cia_wait_responese_handler, res) == false){
                delete fd_port;
                LOG_ACC(ERROR, "CSocket::sendResponse() 中向epoll中添加节点时出错[n==-1]");
                return;
            } 
            fd_ports.push_back(fd_port);
        }
        ++res->send_count;
    }else{  // 表示发送出错或者断开连接 n==0||n==-2，清空response即可
        if(res->send_count > 0){
            cia_del_epoll(res->fd, false);
        }
        delete res; 
    }
}

// recv
void CSocket::porcRequest(Message* message){
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
    // sleep(5);
    LOG_ACC(INFO,"线程threadid = %d 结束处理消息",std::this_thread::get_id());

    int fd = message->fd;
    delete message;

    // 发送数据


    char value[] = "HTTP/1.1 404 Not Found\r\n"
         "Date: Sat, 31 Dec 2005 23:59:59 GMT\r\n"
         "Server: Lucia/0.0.1 (Unix) PHP/5.05\r\n"
         "Content-Type: text/html;charset=utf-8\r\n"
         "Connection: Keep-Alive\r\n"
         "Keep-Alive: timeout=15, max=100\r\n"
         "\r\n"
        "HELLO WORLD!\n";
    
    char* buf = new char[strlen(value)+2];

    memcpy(buf, value, strlen(value));
    
    Response* response = new Response(buf, strlen(buf));
    response->fd = fd;
    datapoll.inResQueue(response);
}

