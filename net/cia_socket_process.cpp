#include "cia_socket.h"
void CSocket::cia_epoll_process_events(const struct timespec *timeout){

    LOG_ERR(INFO, "CSocket::cia_epoll_process_events() 开始运行, 最大连接数为 %d, kqueue的文件描述符是 %d", work_connection, kqueue_fd);

    struct kevent events[work_connection];
    int nevents = 0;
    LOG_ERR(INFO, "CSocket::cia_epoll_process_events() 开始运行到event之前");
    nevents = kevent(kqueue_fd, NULL, 0, events, work_connection, timeout); 
    LOG_ERR(INFO, "CSocket::cia_epoll_process_events() 开始运行到event");
    if(nevents < 0){
        // 一堆判断条件先不管
        LOG_ERR(ERROR, "CSocket::cia_epoll_process_events()中kevent()返回值为负数，出错了！");
        return;
    }
    LOG_ERR(ERROR, "CSocket::cia_epoll_process_events()中kevent()返回值为nevents = %d", nevents);
    for(int i = 0; i<nevents; i++){
        Kevent_Node* eve = (Kevent_Node *)events[i].udata;
        LOG_ERR(INFO, "CSocket::cia_epoll_process_events() 有事件发生。");
        (this->*(eve->handler))(eve);
    }
}