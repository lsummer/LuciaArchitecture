#ifndef __HEADERS_CIA_SOCKET_H__
#define __HEADERS_CIA_SOCKET_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <time.h>
#include "cia_conf.h"
#include "cia_log.h"
#include "cia_comm.h"
#include "http_parser.h"
#include "cia_parser.h"
#include "cia_response_header.h"
#ifdef __linux__
    #include <sys/epoll.h>
#endif

typedef class Kevent_Node Kevent_Node;

typedef class  CSocket           CSocket;

typedef void (CSocket::*cia_event_handler_ptr)(Kevent_Node* kn); //定义成员函数指针

class Kevent_Node{  // 有两种，一种是request，另一种是response，所以如何设置这两种不同的类别
public: 
    int fd;  // 描述符
    int port; // 端口号
    cia_event_handler_ptr handler;  // 回调函数
    Kevent_Node* next;

    Response* c_response;  // 这里并没有删除c_response
    
    // char* buf;  // 资源池，该连接发来的数据开辟的信息
    // int status;  // 状态，数据有一定的格式; -1(PKG_UNUSE)表示连接池中的数据，0(PKG_INIT)表示已不在连接池中，1，2，3是其他的数据状态
    // long last_active;  // 上次活跃时间

    // http_parser parser;  // 表示这个要初始化
    CParser* c_message;  //在哪里创建，在哪里删除
    

    Kevent_Node():fd(0), port(0), handler(NULL), next(NULL), c_response(NULL){
        // http_parser_init(&parser, HTTP_REQUEST);
        c_message = new CParser();
        if(c_message == NULL){
            LOG_ERR(ERROR, "空闲池中的数据区域Kevent_Node创建失败！");
            exit(-2);
        }
    }
    Kevent_Node(int fd, cia_event_handler_ptr handler):fd(fd), port(0), handler(handler), next(NULL), c_message(NULL), c_response(NULL){
        // http_parser_init(&parser, HTTP_REQUEST);
        c_message = new CParser();
        if(c_message == NULL){
            LOG_ERR(ERROR, "空闲池中的数据区域Kevent_Node创建失败！");
            exit(-2);
        }
    }
    ~Kevent_Node(){
        clear();
        delete c_message;
    }
    void clear(){ // 清理自己的数据，以放回到free_link时数据清除工作
        fd = 0;
        port = 0;
        handler = NULL;
        next = NULL;
        c_message->restore();  // 目的是清楚数据
        c_response = NULL;
        // status = PKG_UNUSE;
        // http_parser_init(&parser, HTTP_REQUEST);
    }
};

// 2019-3-22暂时未考虑多线程的情况，是否会用到多线程还未知，如果用到了的话会后续进行修改，一个典型的生产者消费者模型
class FreeLink{
public:
    FreeLink():header(NULL), node_number(0){}
    ~FreeLink(){ // 析构
        while(header){
            Kevent_Node* p = header;
            header = header->next;
            p->clear();
            delete p;
        }
    }

    FreeLink(int n):node_number(n), header(NULL){
        if(n <= 0) return;
        Kevent_Node* node1 = new Kevent_Node();
        header = node1;
        Kevent_Node* p = node1;
        for(int i=1; i<n; i++){
            Kevent_Node* node_flag = new Kevent_Node();
            p->next = node_flag;
            p = p->next;
        }
    }
    Kevent_Node* get_Node(){
        if(node_number <= 0) return NULL;
        Kevent_Node* res = header;
        header = header->next;

        node_number --;
        res->next = NULL;

        // res->last_active = time(0);   // 活跃时间
        // res->status = PKG_INIT;  // 状态变化，表示

        return res;
    }
    void insert_Node(Kevent_Node* node){
        node->clear();
        node->next = header;
        header = node;
        node_number ++;
    }
private:
    std::atomic<int> node_number;// 当前FreeLink中有多少个空闲链
    Kevent_Node* header; // 头结点
};
// TODO 要加一个字段，是读还是写
class FD_PORT{  
public:
    int fd;   // 描述符
    int port;  // 对应的监听端口
    bool wr_flag;  // 读写标记，因为kqueue自身是分离读和写的，所以要加上这个标记, 读是True， 写是false
    Kevent_Node* event;  //处理该描述符相关 的区域
    FD_PORT(int fd, bool wr_flag, int port):fd(fd),port(port),wr_flag(wr_flag), event(NULL){}
};

class CSocket{
public:
    CSocket();
    ~CSocket();

    bool setnoneblocking(int fd);

    bool socket_init();
    void closesocket();  // 关闭文件描述符，以及清楚fd_ports对应的内存

    // epoll_init()初始化失败的话返回false，直接退出子进程，当某个子进程退出的时候，应该再创建一个
    bool epoll_init();  //epoll的使用是linux only 而 macos是unix-like的，所以使用的是kqueue;

    bool epoll_init_linux();
    bool epoll_init_macos();  // 创建epoll ；主要三个函数 epoll_create(), epoll_ctl() 和 epoll_wait(); 这里使用epoll_create();
    void cia_socket_accept(Kevent_Node* kn);

    bool cia_add_epoll(FD_PORT* fd_port, int read, int write, cia_event_handler_ptr handler, Response* response=NULL);
    void cia_del_epoll(int fd, bool wr_flag);

    void cia_epoll_process_events(const struct timespec *timeout = NULL);
    void cia_wait_request_handler(Kevent_Node* kn);  // 处理发来的请求de回调函数
    void cia_wait_responese_handler(Kevent_Node* kn); // 回复请求的回调函数

    
    void sendResponse(Response* res);    // 发送请求
    void porcRequest(Message* message);  // 处理request
private:
    // 三件套，当多了一个需要处理的文件描述符时，需要将一个对应的FD_PORT添加到fd_ports中去，current_link需要+1，free_link需要getNode()一次
    // 当关闭一个文件描述符时， 需要删除一个fd_ports
    // 当前kqueue或者epollI/O多路复用的数量 = 监听端口描述+已连接的描述符数量，大小应该等于fd_ports.size();
    std::vector<FD_PORT*> fd_ports;  //当前连接数量，不止是fd+ports, 建立连接后的文件描述符也要存在这里，他的大小表示当前连接的数量。
    FreeLink* free_link;   

    int kqueue_fd;  // 建立起的kqueuq的文件描述符
    // struct kevent* kevents;  // 希望后续不会因为多线程的问题而导致出现问题
    int work_connection;   // epoll支持的最大连接数量，

    // ---- 为构造response服务的函数 -------
    std::string GetContentType(const std::string& url);
    void getRequestHeader(Message* message, Cia_Response_Header* header);
    int GetFile(Message* message, Cia_Response_Header* header);
    void ConstructResponse(Message* message);
    // -----------------------------------

    // ---- 为sendResponse服务的函数 -------
    int sendBuf(Response* res);
    int sendFile(Response* res);
    int sendProc(Response* res);
    void closeFile(Response* res, int fd);
    // ------------------------------------
};

#endif