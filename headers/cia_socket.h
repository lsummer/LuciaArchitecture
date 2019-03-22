#ifndef __HEADERS_CIA_SOCKET_H__
#define __HEADERS_CIA_SOCKET_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include "cia_conf.h"
#include "cia_log.h"
class FD_PORT{
public:
    int fd;   // 描述符
    int port;  // 对应的监听端口
    FD_PORT(int fd, int port):fd(fd),port(port){}
};

class CSocket{
public:
    CSocket();
    ~CSocket();

    bool setnoneblocking(int fd);

    bool socket_init();
    void closesocket();  // 关闭文件描述符，以及清楚fd_ports对应的内存
private:

    std::vector<FD_PORT*> fd_ports;  // 主要目的是为了关闭fd
    
};

#endif